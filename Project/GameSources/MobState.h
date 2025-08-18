/*!
 @file   MobState.h
 @brief  Mob 用ステート管理の宣言
*/

#pragma once

#include "stdafx.h"
#include "Mob.h"

#include <cfloat>
#include <vector>

namespace basecross {

    // 定数定義
    static constexpr float AVOID_DURATION = 0.8f;   ///< 回避行動持続時間（秒）
    static constexpr float RELOAD_DURATION = 3.0f;   ///< リロード時間（秒）
    static constexpr float EFFECT_THRESHOLD1 = 0.4f;   ///< エフェクト予告サイン再生閾値１（秒）
    static constexpr float EFFECT_THRESHOLD2 = 0.2f;   ///< エフェクト予告サイン再生閾値２（秒）

    //―――――――――――――――――――――――――――
    // EnemyState（Mob 用ステートの基底クラス）
    //―――――――――――――――――――――――――――
    /**
     * @brief すべての Mob ステートが継承する基底クラス。
     * @tparam T ステートを持つエネミーの型（この場合は Mob）。
     */
    template<typename T>
    class EnemyState {
    protected:
        std::shared_ptr<T>         m_Enemy;      ///< このステートが制御する Mob インスタンス
        std::shared_ptr<Stage>     m_Stage;      ///< 現在のステージ
        std::shared_ptr<Transform> m_Transform;  ///< Mob の Transform コンポーネント
        std::shared_ptr<Character> m_Player;     ///< プレイヤー（侵入者）キャラクター

        /**
         * @brief 経過時間を取得（ゲームの時間率を乗算済み）。
         * @return 経過時間（秒）
         */
        float DeltaTime() const {
            return App::GetApp()->GetElapsedTime()
                * GameManager::GetInstance().GetTimeRate();
        }

        /**
         * @brief 指定されたグループ内で最も近いオブジェクトを探す。
         * @param groupName 検索対象のオブジェクトグループ名
         * @param outDir     呼び出し元に返される Mob からオブジェクトへの方向ベクトル
         * @param outDist    呼び出し元に返される最小距離
         * @return 最も近いオブジェクトへの shared_ptr（存在しない場合は nullptr）
         */
        std::shared_ptr<Object> FindNearest(
            const std::wstring& groupName,
            Vec3& outDir,
            float& outDist)
        {
            auto grp = m_Stage->GetSharedObjectGroup(groupName)
                ->GetGroupVector();
            Vec3 myPos = m_Enemy->GetPosition();
            outDist = FLT_MAX;
            std::shared_ptr<Object> nearest;

            for (auto& weakObj : grp) {
                if (auto obj = weakObj.lock()) {
                    Vec3 diff = obj->GetComponent<Transform>()
                        ->GetPosition() - myPos;
                    float dist = diff.length();
                    if (dist < outDist) {
                        outDist = dist;
                        outDir = diff;
                        nearest = std::dynamic_pointer_cast<Object>(obj);
                    }
                }
            }
            return nearest;
        }

        /**
         * @brief Y軸回転で指定方向を向く。
         * @param dir 向きたい方向ベクトル
         */
        void RotateTo(const Vec3& dir) {
            float ang = atan2f(dir.x, dir.z);
            m_Transform->SetRotation({ 0.0f, ang, 0.0f });
        }

        /**
         * @brief プレイヤーを検知しているかどうかを取得。
         * @return 検知フラグ true=警戒中
         */
        bool IsAlert() const {
            return m_Enemy->GetIntruderAlert();
        }

    public:
        /**
         * @brief コンストラクタ
         * @param enemy このステートを持つ Mob の shared_ptr
         */
        EnemyState(std::shared_ptr<T>& enemy)
            : m_Enemy(enemy)
        {
        }

        virtual ‾EnemyState() = default;

        /**
         * @brief ステート遷移時に一度だけ呼ばれる初期化処理。
         */
        virtual void Enter() {
            m_Stage = m_Enemy->GetStage();
            m_Transform = m_Enemy->GetComponent<Transform>();
            m_Player = m_Enemy->m_Intruder;
        }

        /**
         * @brief ステート中に毎フレーム呼ばれるロジック。
         */
        virtual void Execute() = 0;

        /**
         * @brief ステートを抜けるときに呼ばれる処理。
         */
        virtual void Exit() = 0;
    };

    //―――――――――――――――――――――――――――
    // MobSearch（市民やプレイヤーを探索するステート）
    //―――――――――――――――――――――――――――
    /**
     * @brief 市民およびプレイヤーを探索し、追跡動作を行う Mob のステート。
     */
    class MobSearch : public EnemyState<Mob> {
        std::vector<Vec3> m_Path; ///< （未使用／将来の経路探索用）

    public:
        /**
         * @brief コンストラクタ
         * @param enemy ステートを持つ Mob の shared_ptr
         */
        MobSearch(std::shared_ptr<Mob>& enemy)
            : EnemyState<Mob>(enemy)
        {
        }

        void Enter()   override;  ///< ステート開始時の初期化
        void Execute() override;  ///< 毎フレームの動作
        void Exit()    override;  ///< ステート終了時の後処理
    };

    //―――――――――――――――――――――――――――
    // MobAlert（プレイヤーを発見して攻撃するステート）
    //―――――――――――――――――――――――――――
    /**
     * @brief プレイヤーを検知した際に射撃・回避・リロードを行う Mob のステート。
     */
    class MobAlert : public EnemyState<Mob> {
        int   m_BulletRemain = 0;            ///< 残弾数
        float m_BulletReloadTime = RELOAD_DURATION; ///< リロード残り時間
        bool  m_BulletEffect = false;       ///< エフェクト予告再生済みフラグ
        bool  m_BulletSound = false;       ///< サウンド予告再生済みフラグ

        std::shared_ptr<EffectManager> m_Effect; ///< エフェクトマネージャ
        Effekseer::Handle              m_EyeHandle = -1; ///< 予告サインハンドル
        Effekseer::Handle              m_FlashHandle = -1; ///< フラッシュハンドル

    public:
        /**
         * @brief コンストラクタ
         * @param enemy ステートを持つ Mob の shared_ptr
         */
        MobAlert(std::shared_ptr<Mob>& enemy)
            : EnemyState<Mob>(enemy)
        {
        }

        void Enter()   override;  ///< ステート開始時の初期化（弾数やエフェクト取得）
        void Execute() override;  ///< 毎フレームの攻撃／回避／リロード処理
        void Exit()    override;  ///< ステート終了時のクリーンアップ（エフェクト停止）
    };

} // namespace basecross