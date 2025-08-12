/*!
 * @file Enemy.h
 * @brief Mob クラスの宣言 (敵キャラクターの AI、移動、攻撃、フェード演出、HPバー管理を実装)
 */

#pragma once

#include "stdafx.h"
#include "MyDrawComponents.h"

namespace basecross {

    /**
     * @brief Mob クラス用の状態遷移テンプレート基底クラス
     * @tparam T 対象となる Mob クラス型
     */
    template <typename T>
    class EnemyState;

    class Enemy;
    class Tube;
    class HPBar;
    class RootPointer;

    /**
     * @class Mob
     * @brief Enemy クラスを継承し、移動・射撃・フェード演出・HPバーなど
     *        敵キャラクター固有の機能を実装するクラス
     */
    class Mob : public Enemy
    {
    public:
        /** @brief 現在のステートビットマスク (ALER, WALK, ATTACK) */
        int m_MobState;

        /**
         * @enum State
         * @brief Mob の行動ステートビット
         */
        enum State {
            ALER = 0b00000001,  /**< 警戒状態 */
            WALK = 0b00000010,  /**< 歩行状態 */
            ATTACK = 0b00000100   /**< 攻撃状態 */
        };

        /** @brief 仮ステートビットマスク (musi, hakai) */
        int m_kariState;

        /**
         * @enum kariState
         * @brief Mob の一時的な内部サブステート
         */
        enum kariState {
            musi = 0b00000001,   /**< 無視状態 */
            hakai = 0b00000010    /**< 破壊状態 */
        };

        /** @brief 弾丸の速度 */
        float m_BalletSpeed;

        /** @brief 射撃の有効射程 */
        float m_BalletRange;

        /** @brief マズル（発射位置）のオフセット量 */
        float m_MuzzleOffset;

        /** @brief 次の発射までのインターバル時間 */
        float m_BalletInterval;

        /** @brief ノックバック後の復帰までのインターバル時間 */
        float m_KnockBackInterval;

        /** @brief インターバル調整用の難易度係数 */
        float m_IntervalDifficulty;

        /** @brief 発射インターバルの最大値 (定数) */
        const float MAX_BALLET_INTERVAL;

        /** @brief 発射タイミング調整用の乱数インターバル */
        float m_ShotRandomInterval;

        /** @brief インターバル処理の開始フラグ */
        bool m_IntervalStart;

        /** @brief 前フレームの位置記録用 */
        Vec3 m_Before = Vec3(0);

        /** @brief 残弾数 */
        int m_BulletRemain;

        /** @brief 探索エフェクト用の SharpFan コンポーネント */
        shared_ptr<SharpFan> m_SearchFan;

        /** @brief 表示用 HP バーオブジェクト */
        shared_ptr<HPBar> m_HpBar;

        /** @brief 現在のナビゲーションターゲットポイント */
        shared_ptr<GameObject> m_NearPoint;

        /** @brief すべてのポイントデータ */
        vector<shared_ptr<GameObject>> m_PointData;

        /** @brief 直前に選択したポイント */
        shared_ptr<GameObject> m_BeforPoint;

        /** @brief 現在アクティブな状態オブジェクト */
        unique_ptr<EnemyState<Mob>> m_currentState;

        /** @brief 次に遷移予定の状態オブジェクト */
        unique_ptr<EnemyState<Mob>> m_nextState;

        /** @brief 更新処理を行うかどうかのフラグ */
        bool m_Update;

        /** @brief レイキャスト実行のクールダウンタイマー */
        float m_RayCastCooldown = 0.0f;

        /** @brief レイキャストの最小実行間隔 (定数) */
        const float MAX_RAYCAST_INTERVAL = 0.2f;

        /** @brief 回避行動中かどうかのフラグ */
        bool m_IsAvoiding = false;

        /** @brief 回避行動を継続する残り時間 */
        float m_AvoidTime = 0.0f;

        /** @brief 回避時の移動方向 */
        Vec3 m_AvoidDirection;

    public:
        /**
         * @brief デフォルトコンストラクタ
         * @param stage 所属するステージへの共有ポインタ
         */
        Mob(const shared_ptr<Stage>& stage);

        /**
         * @brief 引数付きコンストラクタ
         * @param stage    所属するステージへの共有ポインタ
         * @param position 初期位置ベクトル
         * @param scale    初期スケールベクトル
         */
        Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);

        /**
         * @brief デストラクタ
         */
        ‾Mob();

        /**
         * @brief オブジェクト生成直後の初期化処理
         */
        virtual void OnCreate() override;

        /**
         * @brief 毎フレーム呼ばれる更新処理
         */
        virtual void OnUpdate() override;

        /**
         * @brief シーンにスポーンされたときの処理 (フェードインなど)
         */
        virtual void OnSpawn() override;

        /**
         * @brief 完全生成後に呼ばれる後処理 (HPバー生成など)
         */
        virtual void OnAfterCreate() override;

        /**
         * @brief 非同期ステート更新や状態遷移を安全に実行するメソッド
         */
        virtual void AsyncUpdate() override;

        /**
         * @brief 死亡時の演出とクリーンアップ処理
         */
        virtual void Dead() override;

        /**
         * @brief 衝突判定時のコールバック
         * @param other 衝突した他の GameObject への共有ポインタ
         */
        void OnCollisionEnter(shared_ptr<GameObject>& other);

        /**
         * @brief アニメーションシーケンスを登録する
         */
        void AddAnimation();

        /**
         * @brief ナビゲーション用にランダムポイントを選択して返す
         * @return 選択されたポイントのワールド座標
         */
        Vec3 RootNaviGate();

        /**
         * @brief 所属ステージへのアクセサ
         * @return Stage への共有ポインタ
         */
        shared_ptr<Stage> GetStage();

        /**
         * @brief 自身の Transform コンポーネントへのアクセサ
         * @return Transform への共有ポインタ
         */
        shared_ptr<Transform> GetTransfrom();

        /**
         * @brief ズームタイム（時間倍率）を取得する
         * @return ゾーン補正後の経過時間倍率
         */
        float GetZoomTime() {
            return m_ZoneElapsedTime;
        }

        /**
         * @brief 現在設定されているターゲットポイントを取得する
         * @return ターゲットポイントの GameObject 共有ポインタ
         */
        shared_ptr<GameObject> GetNearPoint() {
            return m_NearPoint;
        }

        /**
         * @brief 全ナビゲーションポイント情報を取得する
         * @return GameObject 共有ポインタ配列
         */
        vector<shared_ptr<GameObject>> GetPointData() {
            return m_PointData;
        }

        /**
         * @brief 指定オブジェクトを向くように Y 軸回転を設定する
         * @param obj 注視対象のオブジェクト共有ポインタ
         */
        void AlartMove(shared_ptr<Object> obj);

        /**
         * @brief direction ベクトル方向へ移動量を加算する
         * @param direction 正規化された移動方向ベクトル
         */
        void SetMoveDirection(const Vec3& direction);

        /**
         * @brief テンプレートによるステート遷移
         * @tparam NextState 遷移先ステートクラス
         */
        template <class NextState>
        void ChangeState() {
            m_currentState->Exit();
            m_currentState.reset();
            m_currentState = make_unique<NextState>(GetThis<Mob>());
            m_currentState->Enter();
        }

        /**
         * @brief アニメーション切り替えユーティリティ
         * @param animname 再生するアニメーション名
         * @param time      再生開始オフセット時間
         * @param enforce   true ならループ中でも強制的に切り替え
         */
        const void SetAnim(wstring animname, float time = 0.0f, bool enforce = false) {
            auto draw = GetComponent<DissolveDraw>();
            if (draw->GetCurrentAnimation() != animname)
                if (draw->GetAnimeLoop())
                    draw->ChangeCurrentAnimation(animname, time);
                else {
                    if (draw->IsTargetAnimeEnd() || enforce)
                        draw->ChangeCurrentAnimation(animname, time);
                }
        }

        /**
         * @brief 線形補間 (Lerp) を行う
         * @param a 開始ベクトル
         * @param b 終了ベクトル
         * @param t 補間係数 [0,1]
         * @return 補間後のベクトル
         */
        Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
            return a * (1.0f - t) + b * t;
        }

        /**
         * @brief wide 文字列を float に変換する
         * @param data 変換対象の文字列
         * @return 変換後の浮動小数点数 (空文字列は NULL)
         */
        float WstrToFlt(const wstring& data) {
            if (data == L"") return NULL;
            return stof(data);
        }

        /**
         * @brief アンダースコア区切り文字列を整数ベクトルに変換する
         * @param data 例: "1_2_3" などのアンダースコア区切り文字列
         * @return 変換後の整数ベクトル
         */
        vector<int> WstrToVecInt(const wstring& data) {
            vector<wstring> vec3Str = {};
            vector<int> num = {};
            Util::WStrToTokenVector(vec3Str, data, L'_');
            for (int i = 0; i < vec3Str.size(); i++) {
                if (WstrToFlt(vec3Str[i]) != NULL) {
                    num.push_back(WstrToFlt(vec3Str[i]));
                }
            }
            return num;
        }

    };

} // namespace basecross