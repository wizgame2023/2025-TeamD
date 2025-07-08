/*!
 * @file   Player.h
 * @brief  プレイヤーなど
 */

#pragma once

#include "stdafx.h"

namespace basecross {

    class Character;
    class TargetBoard;

    /**
     * @class  Player
     * @brief  Character を継承したプレイヤーキャラクラス
     */
    class Player : public Character
    {
    private:
        float m_MoveSpeed;                 ///< 移動速度
        float m_EnergyCharge;              ///< ゾーンエネルギーチャージ量
        float m_ZoneTime;                  ///< ゾーン継続時間
        float m_ParryTime;                 ///< パリィ受付時間
        float m_ParryDamageIntervalTime;   ///< パリィ後ダメージインターバル
        float m_TotalTime;                 ///< 全体タイマー
        float m_BoostTime;                 ///< ブースト継続時間
        float m_BoostInterval;             ///< ブースト再使用インターバル
        float m_Attacktime;                ///< 攻撃アニメーション再生時間
        float m_AttackInterval;            ///< 攻撃発生インターバル
        float m_DamageInterval;            ///< 被ダメージ無敵時間
        float m_BlinkingInterval;          ///< 被ダメージ点滅間隔
        float m_Damage;                    ///< 蓄積ダメージ量
        bool  m_ParryJudge;                ///< パリィ受付中フラグ
        bool  m_ParryDamageInterval;       ///< パリィ後ダメージ無効インターバル中フラグ
        bool  m_DamageIntervalStart;       ///< ダメージ無敵開始フラグ
        bool  m_IsGoal;                    ///< ゴール到達フラグ
        bool  m_IsPerfectParry;            ///< 完璧パリィ成功フラグ
        bool  m_IsParry;                   ///< パリィモード中フラグ
        float m_PerfectParrySecond;        ///< 完璧パリィ受付許容時間[s]
        float m_ParryDamage;               ///< パリィ時反撃ダメージ
        float m_zoneAnim;                  ///< ゾーン演出進行度
        float m_SearchDistance;            ///< 敵探索距離
        float m_Length;                    ///< モデル長さ

        Vec3 m_BoostAngle;                 ///< ブースト移動方向ベクトル
        Vec3 m_HitScale;                   ///< 被ヒット時スケール補正
        Vec3 m_BulletDire;                 ///< 弾丸飛行方向
        Vec3 m_EffectVec;                  ///< エフェクト位置補正
        shared_ptr<EffectManager> m_Effect;///< エフェクト管理
        Effekseer::Handle m_Handle;        ///< メインエフェクトハンドル
        Effekseer::Handle m_BrinkHandle;   ///< 点滅エフェクトハンドル
        Effekseer::Handle m_ParryHandle;   ///< パリィエフェクトハンドル
        shared_ptr<TargetBoard> m_TargetBoard;///< 照準ボード

        wstring m_AttackAnim;              ///< 攻撃アニメーション名
        bool    m_ParryComboActive;        ///< コンボ猶予中フラグ
        int     m_ParryComboCount;         ///< 連続パリィ回数
        float   m_ParryComboTimer;         ///< 連続パリィ猶予時間
        float   m_time;                    ///< 汎用タイマー
        float   m_ParryComboWindow;        ///< 完璧パリィから次パリィ受付猶予[s]

    public:
        int m_PlayerStateNum;              ///< 現在の状態フラグ

        /**
         * @enum PlayerState
         * @brief プレイヤー動作状態ビットフラグ
         */
        enum PlayerState
        {
            NORMAL = 0b00000001,  ///< 待機中
            WALK = 0b00000010,  ///< 歩行中
            RUN = 0b00000100,  ///< 走行中
            ATTACK = 0b00001000,  ///< 攻撃中
            ZONE = 0b00010000,  ///< ゾーン中
            DASH = 0b00100000   ///< ダッシュ中
        };

        /**
         * @brief コンストラクタ
         * @param stage 所属ステージ
         */
        Player(const shared_ptr<Stage>& stage);

        /**
         * @brief コンストラクタ（位置・回転・スケール指定）
         * @param stage    所属ステージ
         * @param position 初期位置
         * @param rotation 初期回転
         * @param scale    初期スケール
         */
        Player(const shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& rotation,
            const Vec3& scale);

        ‾Player();

        /// @brief 初期化処理
        virtual void OnCreate() override;

        /// @brief 毎フレーム更新
        virtual void OnUpdate() override;

        /// @brief 描画処理
        virtual void OnDraw() override;

        /// @brief 死亡処理
        virtual void Dead() override;

        /**
         * @brief ダメージ処理
         * @param parry   パリィ受付結果
         * @param damage  ダメージ量
         * @param source  発生源オブジェクト（省略可）
         * @return ダメージ適用可否
         */
        virtual bool Damage(bool parry,float damage,const shared_ptr<GameObject> source = nullptr);

        /// @brief タイマー更新管理
        void IntervalManagement();

        /// @brief 入力状態取得
        Vec2 GetInputState() const;

        /**
         * @brief 移動ベクトル計算
         * @param rot [out] 回転角
         * @return 移動ベクトル
         */
        Vec3 GetMoveVector(float& rot);

        /**
         * @brief 通常移動
         * @param Speed 移動速度
         */
        void MovePlayer(const float Speed);

        /**
         * @brief ブースト移動
         * @param Speed 移動速度
         * @param Angle 移動方向
         */
        void BoostMove(const float Speed, const Vec3 Angle);

        /// @brief ゾーンモード発動
        void ZoneActivation();

        /// @brief デバッグ表示
        void Debug();

        /// @brief 正面ベクトル取得
        Vec3 GetForward();

        /// @brief 状態フラグ取得
        int GetStates();

        /// @brief エネルギー量取得
        float GetEnergy();

        /// @brief ダメージ量取得
        float GetDamage();

        /// @brief パリィ可能か
        bool GetParry();

        /**
         * @brief パリィ判定位置設定
         * @param position 位置
         */
        void SetParryPosition(const Vec3& position);

        /**
         * @brief ダメージ量設定
         * @param damage 新ダメージ量
         */
        void SetDamage(const float& damage);

        /**
         * @brief ゴール状態設定
         * @param goal 到達済みならtrue
         */
        void SetIsGaol(const bool& goal);

        /**
         * @brief 敵探索方向取得
         * @param angle 探索角度
         * @return 方向ベクトル
         */
        Vec3 SearchRange(float angle);

        /**
         * @brief チャージ量設定
         * @param charge 新チャージ量
         */
        void SetCharge(const float& charge);

        /// @brief 完璧パリィ入力受付
        void HandlePerfectParryInput();

        /**
         * @brief 対象へ向く回転ベクトル
         * @param object 自身位置
         * @param target 目標位置
         * @return 回転ベクトル
         */
        Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);

        /**
         * @brief ロックオン補正
         * @param rotate 回転ベクトル
         */
        void AimRock(Vec3 rotate);

        /**
         * @brief オブジェクト探索
         * @param group 対象グループ
         * @return 発見オブジェクト
         */
        shared_ptr<GameObject> ObjectSearch(const shared_ptr<GameObjectGroup>& group);

        /// @brief アニメーション更新
        void UpdateAnim();

        /**
         * @brief パリィ処理
         * @param damage       ダメージ量
         * @param ParrySecond  受付時間
         * @return 適用ダメージ量
         */
        float Parry(float damage, const float& ParrySecond);

        /// @brief アニメーション追加
        void AddAnimation();

        /// @brief アニメーション再生
        void PlayAnimation();

        /** @brief ダッシュ再生
		* @param elapsedTime 経過時間[s]
        */
        void HandleDash(const float& elapsedTime);

        /** @brief アタック再生
        * @param elapsedTime 経過時間[s]
        */
        void HandleAttack(const float& elapsedTime);

        /** @brief 通常状態の再生
        * @param elapsedTime 経過時間[s]
        */
        void HandleNormal(const float& elapsedTime);

        /// @brief 点滅演出
        void Blinking();

        /**
         * @brief アニメ切替補助
         * @param animname アニメ名
         * @param time     ブレンド時間[s]
         */
        const void SetAnim(wstring animname, float time = 0.0f)
        {
            auto draw = GetComponent<PNTBoneModelDraw>();
            if (draw->GetCurrentAnimation() != animname)
                if (draw->GetAnimeLoop())
                    draw->ChangeCurrentAnimation(animname, time);
                else if (draw->IsTargetAnimeEnd())
                    draw->ChangeCurrentAnimation(animname, time);
        }

        /**
         * @brief インターバルタイマー判定
         * @param TimerStart 判定開始フラグ
         * @param MaxTimer   目標時間[s]
         * @param frame      経過フレーム or 秒
         * @param Timer      累積時間
         * @param Return     到達時true返却
         * @return 到達有無
         */
        bool IntervalTimer(const bool& TimerStart,const float& MaxTimer,const float& frame,float& Timer,const bool& Return);

        /**
         * @brief 探索範囲長さ取得
         * @return m_SearchDistance
         */
        float GetLength()
        {
            return m_SearchDistance;
        }
    };

    /**
     * @class  HitSphere
     * @brief  プレイヤー攻撃用ヒット判定オブジェクト
     */
    class HitSphere : public Object
    {
    private:
        Vec3 m_HitPosition;            ///< 初期位置
        Vec3 m_HitRotation;            ///< 初期回転
        Vec3 m_HitScale;               ///< 初期スケール
        float m_FlyingTime;            ///< 飛行経過時間
        float m_TotalTime;             ///< 総経過時間
        float m_Speed;                 ///< 飛行速度
        float m_ZoneElapsedTime;       ///< ゾーン中経過時間
        float m_Length;                ///< 判定距離
        shared_ptr<EffectManager> m_Effect; ///< エフェクト管理
        Effekseer::Handle m_Handle;    ///< メインエフェクトハンドル
        Effekseer::Handle m_HitHandle; ///< ヒットエフェクトハンドル

        shared_ptr<GameObject> m_Player; ///< 発射元プレイヤー

        float f = 0;                    ///< 汎用カウンタ

    public:
        /**
         * @brief コンストラクタ
         * @param stage    所属ステージ
         * @param position 発射位置
         * @param forward  飛行方向
         * @param player   発射元
         * @param scale    モデルスケール
         * @param length   判定距離
         */
        HitSphere(const shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& forward,
            const shared_ptr<GameObject> player,
            const Vec3 scale,
            const float& length);

        ‾HitSphere();

        /// @brief 初期化処理
        virtual void OnCreate() override;

        /// @brief 毎フレーム更新
        virtual void OnUpdate() override;

        /**
         * @brief 衝突判定開始時
         * @param other 相手オブジェクト
         */
        void OnCollisionEnter(shared_ptr<GameObject>& other);
    };

} // namespace basecross