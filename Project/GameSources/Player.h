/*!
 * @file   Player.h
 * @brief  プレイヤーなど
 */

#pragma once

#include "stdafx.h"
#include "InputAndSettings.h"

namespace basecross {

    class Character;
    class TargetBoard;
    class Board;

    /**
     * @class Player
     * @brief Character を継承したプレイヤーキャラクタクラス
     */
    class Player : public Character
    {
    private:
        MovementSettings    m_MoveMent;         ///< 移動設定
        ZoneSettings        m_Zone;             ///< ゾーン設定
        ParrySettings       m_Parry;            ///< パリィ設定
        BoostSettings       m_Boost;            ///< ブースト設定
        AttackSettings      m_Attack;           ///< 攻撃設定
        DamageSettings      m_Damage;           ///< ダメージ設定
        ComboSettings       m_Combo;            ///< コンボ設定

        float m_TotalTime;                      ///< 全体タイマー
        bool  m_IsGoal;                         ///< ゴール到達フラグ
        bool  m_IsParryCounter;                 ///< パリィモード中フラグ
        bool  m_IsCharged;                      ///< チャージ完了フラグ

        Vec3  m_HitScale;                       ///< 被ヒット時スケール補正
        Vec3  m_EffectVec;                      ///< エフェクト位置補正
        std::shared_ptr<EffectManager> m_Effect;   ///< エフェクト管理
        Effekseer::Handle   m_Handle;           ///< メインエフェクトハンドル
        Effekseer::Handle   m_BrinkHandle;      ///< 点滅エフェクトハンドル
        Effekseer::Handle   m_ParryHandle;      ///< パリィエフェクトハンドル
        Effekseer::Handle   m_EarthQuakeHandle; ///< 地震エフェクトハンドル
        std::shared_ptr<TargetBoard>    m_TargetBoard; ///< 照準ボード
        std::shared_ptr<Board>          m_EnemyArrow;  ///< 最も近い敵の方向を示す矢印
        Vec3                            m_TargetObject;///< ロックオン対象オブジェクト

    public:
        int m_PlayerStateNum;   ///< 現在の状態ビットフラグ

        /**
         * @enum PlayerState
         * @brief プレイヤー動作状態ビットフラグ
         */
        enum PlayerState
        {
            NORMAL = 0b00000001, ///< 待機中
            WALK = 0b00000010, ///< 歩行中
            RUN = 0b00000100, ///< 走行中
            ATTACK = 0b00001000, ///< 攻撃中
            ZONE = 0b00010000, ///< ゾーン中
            DASH = 0b00100000, ///< ダッシュ中
            ATTACKCHARGE = 0b01000000  ///< ため攻撃中
        };

        /**
         * @brief コンストラクタ
         * @param stage 所属ステージ
         */
        Player(const std::shared_ptr<Stage>& stage);

        /**
         * @brief コンストラクタ（位置・回転・スケール指定）
         * @param stage    所属ステージ
         * @param position 初期位置
         * @param rotation 初期回転
         * @param scale    初期スケール
         */
        Player(const std::shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& rotation,
            const Vec3& scale);

        /**
         * @brief デストラクタ
         */
        ‾Player();

        /**
         * @brief 初期化処理
         */
        virtual void OnCreate() override;

        /**
         * @brief 毎フレーム更新処理
         */
        virtual void OnUpdate() override;

        /**
         * @brief 描画処理
         */
        virtual void OnDraw() override;

        /**
         * @brief 死亡処理
         */
        virtual void Dead() override;

        /**
         * @brief ダメージ処理
         * @param parry   パリィ受付結果（true: パリィ成功）
         * @param damage  受けるダメージ量
         * @param source  ダメージ発生源（省略可）
         * @return ダメージ適用可否（true: ダメージを受けた）
         */
        virtual bool Damage(bool parry, float damage, const std::shared_ptr<GameObject> source = nullptr);

        /**
         * @brief ピンチ状態イベントの処理
         */
        void HandlePinchEvent();

        /**
         * @brief パリィ試行
         * @param incoming 受けている攻撃の威力
         * @param source   攻撃元オブジェクト
         * @return パリィ成功可否
         */
        bool TryParry(float incoming, const std::shared_ptr<GameObject>& source);

        /**
         * @brief 完璧パリィ開始処理
         * @param source パリィ対象オブジェクト
         * @param dir    パリィ方向
         */
        void HandlePerfectParry(const std::shared_ptr<GameObject>& source, const Vec3& dir);

        /**
         * @brief ダメージ適用とエフェクト再生
         * @param amount     ダメージ量
         * @param playEffect エフェクト再生フラグ
         */
        void ApplyDamage(float amount, bool playEffect);

        /**
         * @brief 通常ヒット時の処理
         * @param damage ヒット時ダメージ
         */
        void HandleNormalHit(float damage);

        /**
         * @brief 各種インターバルタイマーの管理
         */
        void IntervalManagement();

        /**
         * @brief 移動ベクトルの算出
         * @param rot [out] 方向回転角
         * @return ワールド空間での移動ベクトル
         */
        Vec3 GetMoveVector(float& rot);

        /**
         * @brief 通常移動処理
         * @param Speed 移動速度
         */
        void MovePlayer(const float Speed);

        /**
         * @brief ブースト移動処理
         * @param Speed 移動速度
         * @param Angle 移動方向ベクトル
         * @param time  ブースト時間（省略可）
         */
        void BoostMove(const float Speed, const Vec3 Angle, const float time = 0.0f);

        /**
         * @brief ゾーンモード発動処理
         */
        void ZoneActivation();

        /**
         * @brief デバッグ描画
         */
        void Debug();

        /**
         * @brief 敵探索方向ベクトル取得
         * @param angle 探索角度（度単位）
         * @return 探索方向の単位ベクトル
         */
        Vec3 SearchRange(float angle);

        /**
         * @brief チャージ量設定
         * @param charge 新しいチャージ量
         */
        void SetCharge(const float& charge);

        /**
         * @brief 完璧パリィ入力受付
         */
        void HandlePerfectParryInput();

        /**
         * @brief 対象へ向く回転ベクトル取得
         * @param object 自身の位置
         * @param target 目標の位置
         * @return 向きベクトル
         */
        Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);

        /**
         * @brief ロックオン補正処理
         * @param rotate 補正後の回転ベクトル
         */
        void AimRock(Vec3 rotate);

        /**
         * @brief 敵方向矢印の描画
         */
        void DrawArrow();

        /**
         * @brief グループ内オブジェクト探索
         * @param group 探索対象のオブジェクトグループ
         * @return 発見したオブジェクト（なければ nullptr）
         */
        std::shared_ptr<GameObject> ObjectSearch(const std::shared_ptr<GameObjectGroup>& group);

        /**
         * @brief アニメーション更新
         */
        void UpdateAnim();

        /**
         * @brief パリィ処理
         * @param damage      本来のダメージ量
         * @param ParrySecond パリィ受付時間
         * @return 実際に適用されるダメージ量
         */
        float Parry(float damage, const float& ParrySecond);

        /**
         * @brief アニメーションリソース追加
         */
        void AddAnimation();

        /**
		 * @brief Payerの状態遷移のディスパッチ
         */
        void DispatchStateTransition();

        /**
         * @brief ダッシュ再生処理
         * @param elapsedTime 経過時間 [秒]
         */
        void HandleDash(const float& elapsedTime);

        /**
         * @brief 攻撃再生処理
         * @param elapsedTime 経過時間 [秒]
         */
        void HandleAttack(const float& elapsedTime);

        /**
         * @brief ため攻撃再生処理
         * @param elapsedTime 経過時間 [秒]
         */
        void HandleAttackCharge(const float& elapsedTime);

        /**
         * @brief 通常状態再生処理
         * @param elapsedTime 経過時間 [秒]
         */
        void HandleNormal(const float& elapsedTime);

        /**
         * @brief 内部タイマー更新
         * @param dt フレーム時間差 [秒]
         */
        void UpdateTimers(const float& dt);

        /**
         * @brief ダッシュトライアル
         * @param forward 前方ベクトル
         */
        void TryDash(const Vec3& forward);

        /**
         * @brief チャージホールド処理
         * @param dt 経過時間 [秒]
         */
        void HandleChargeHold(float dt);

        /**
         * @brief フルチャージ攻撃実行
         */
        void ExecuteFullChargeAttack();

        /**
         * @brief 通常攻撃実行
         */
        void ExecuteNormalAttack();

        /**
         * @brief 点滅エフェクト演出
         */
        void Blinking();

        /**
         * @brief 汎用インターバルタイマー
         * @param TimerStart 開始フラグ
         * @param MaxTimer   目標時間 [秒]
         * @param frame      経過フレーム or 秒
         * @param Timer      累積時間
         * @param Return     到達時 true を返すか
         * @return 到達有無（true 到達済み）
         */
        bool IntervalTimer(const bool& TimerStart,
            const float& MaxTimer,
            const float& frame,
            float& Timer,
            const bool& Return);


        /**
        * @brief アニメ切替補助
        * @param animname アニメ名
        * @param time     ブレンド時間[s]
        */
        const void SetAnim(wstring animname, const bool force = false)
        {
            auto draw = GetComponent<PNTBoneModelDraw>();
            if (draw->GetCurrentAnimation() != animname)
                if (draw->GetAnimeLoop() || force)   draw->ChangeCurrentAnimation(animname, 0.0f);
                else if (draw->IsTargetAnimeEnd())  draw->ChangeCurrentAnimation(animname, 0.0f);
        }

        /**
         * @brief 探索範囲の長さ取得
         * @return m_MoveMent.SearchDistance
         */
        float GetLength() { return m_MoveMent.SearchDistance; }

        /**
         * @brief 正面ベクトル取得
         * @return 自身の正面ベクトル
         */
        Vec3 GetForward() { return m_Transform->GetForward(); }

        /**
         * @brief パリィ判定位置設定
         * @param position パリィ位置
         */
        void SetParryPosition(const Vec3& position) { m_EffectVec = position; }

        /**
         * @brief ダメージ量設定
         * @param damage 新しいダメージ量
         */
        void SetDamage(const float& damage) { m_Damage.Damage = damage; }

        /**
         * @brief ゴール到達フラグ設定
         * @param goal true: ゴール済み
         */
        void SetIsGaol(const bool& goal) { m_IsGoal = goal; }

        /**
         * @brief 現在の状態ビットフラグ取得
         * @return m_PlayerStateNum
         */
        int GetStates() { return m_PlayerStateNum; }

        /**
         * @brief 現在のエネルギー量取得
         * @return m_Zone.EnergyCharge
         */
        float GetEnergy() { return m_Zone.EnergyCharge; }

        /**
         * @brief 現在のダメージ量取得
         * @return m_Damage.Damage
         */
        float GetDamage() { return m_Damage.Damage; }

        /**
         * @brief パリィ可能状態か判定
         * @return m_Parry.IsJudgeActive
         */
        bool GetParry() { return m_Parry.IsJudgeActive; }

        /**
         * @brief 攻撃可能か判定
         * @return true: 攻撃可能
         */
        bool IsAttack()
        {
            return IntervalTimer(true, 0.2f, 0.0f, m_Damage.InvincibleDuration, false)
                && !(m_PlayerStateNum & PlayerState::ATTACK);
        }

        /**
         * @brief ダッシュ可能か判定
         * @return true: ダッシュ可能
         */
        bool IsDash()
        {
            return IntervalTimer(true, 0.5f, 0.0f, m_Boost.Cooldown, false)
                && !(m_PlayerStateNum & PlayerState::DASH);
        }
    };
} // namespace basecross