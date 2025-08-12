/*!
 * @file   Player.h
 * @brief  プレイヤーなど
 */

#pragma once

#include "stdafx.h"

namespace basecross {

    class Character;
    class TargetBoard;
    class Board;

    struct InputState {
        // ボタン
        bool attackPressed = false;  // 攻撃ボタン（A／左クリック）を押した瞬間
        bool attackHeld = false;  // 攻撃ボタンをホールド中
        bool attackReleased = false;  // 攻撃ボタンを離した瞬間

        bool dashPressed = false;  // ダッシュボタン（X／右クリック）を押した瞬間
        bool dashHeld = false;  // ダッシュボタンをホールド中

        bool zonePressed = false;  // ゾーン発動ボタン（B／スペース）を押した瞬間
        bool parryPressed = false;  // パリィカウンター用ボタン（同じくA／左クリック）

        // 移動軸
        float moveX = 0.0f;
        float moveY = 0.0f;
    };


    class InputReader {
    public:
        static InputReader& Get() {
            static InputReader inst;
            return inst;
        }

        InputState Read() {
            InputState in;
            auto& dev = App::GetApp()->GetInputDevice();
            auto& ks = dev.GetKeyState();
            auto& cv = dev.GetControlerVec()[0];

            // ─── 移動軸 ─────────────────────────────────
            if (cv.bConnected) {
                in.moveX = cv.fThumbLX;
                in.moveY = cv.fThumbLY;
            }
            if (ks.m_bPushKeyTbl['A']) in.moveX = -1.0f;
            if (ks.m_bPushKeyTbl['D']) in.moveX = 1.0f;
            if (ks.m_bPushKeyTbl['W']) in.moveY = 1.0f;
            if (ks.m_bPushKeyTbl['S']) in.moveY = -1.0f;
            // ─── 攻撃ボタン ───────────────────────────────
            in.attackPressed = (cv.wPressedButtons & XINPUT_GAMEPAD_A)
                || ks.m_bPressedKeyTbl[VK_LBUTTON];
            in.attackHeld = (cv.wButtons & XINPUT_GAMEPAD_A)
                || ks.m_bPushKeyTbl[VK_LBUTTON];
            in.attackReleased = (cv.wReleasedButtons & XINPUT_GAMEPAD_A)
                || ks.m_bUpKeyTbl[VK_LBUTTON];

            // ─── ダッシュボタン ───────────────────────────
            in.dashPressed = (cv.wPressedButtons & XINPUT_GAMEPAD_X)
                || ks.m_bPressedKeyTbl[VK_RBUTTON];
            in.dashHeld = (cv.wButtons & XINPUT_GAMEPAD_X)
                || ks.m_bPushKeyTbl[VK_RBUTTON];

            // ─── ゾーン発動ボタン ─────────────────────────
            in.zonePressed = (cv.wPressedButtons & XINPUT_GAMEPAD_B)
                || ks.m_bPressedKeyTbl[VK_SPACE];

            // ─── カウンターパリィ用（同Aボタン） ───────────
            in.parryPressed = in.attackPressed;

            return in;
        }
    };

    // Movement（移動・回転関連）
    struct MovementSettings {
        float MoveSpeed;           ///< 移動速度
        float CurrentYaw;          ///< 現在の回転角（ラジアン）
        float RotationSpeed;       ///< 補間の速さ（大きいほど瞬時、低いほどゆっくり）
        float SearchDistance;      ///< 敵探索距離
        Vec3  BoostAngle;          ///< ブースト移動方向ベクトル
    };

    // Zone／Energy（ゾーンチャージ関連）の設定をまとめた構造体
    struct ZoneSettings {
        float EnergyCharge;       ///< 現在のエネルギーチャージ量（0.0〜1.0）
        float ZoneTime;           ///< ゾーン状態の累積時間
        float ZoneAnimProgress;   ///< ゾーン発動アニメーションの進行度（秒数）
        float ZoneAnim;         ///< ゾーン演出進行度
    };

    // Parry（パリィ関連）
    struct ParrySettings
    {
        float JudgeTime;          ///< パリィ受付時間
        float DamageInterval;     ///< パリィ後無敵インターバル
        float PerfectWindow;      ///< 完璧パリィ判定ウィンドウ
        bool  IsJudgeActive;      ///< パリィ受付中フラグ
        bool  IsInDamageInterval; ///< 無敵インターバル中フラグ
        bool  IsPerfectParry;     ///< 完璧パリィ成功フラグ
        bool  HasCountered;       ///< カウンターヒット済フラグ
    };

    // Boost（ブースト関連）
    struct BoostSettings
    {
        float Duration;      ///< ブースト継続時間
        float CounterTime;   ///< カウンターブースト継続時間
        float Cooldown;      ///< ブースト再使用インターバル
    };

    // Attack（攻撃関連）
    struct AttackSettings
    {
        float AnimTime;        ///< 攻撃アニメーション再生時間
        float ChargeInterval;  ///< チャージ攻撃待機時間
		float ChargeTime;      ///< チャージ攻撃時間
        float Interval;        ///< 通常攻撃インターバル
        float MaxInterval;     ///< 攻撃最大インターバル
        std::wstring AnimName; ///< アニメーション名
    };

    // Damage／Invincible（被ダメージ関連）
    struct DamageSettings
    {
        float InvincibleDuration;///< 無敵時間
        float BlinkingInterval;  ///< 点滅サイクル
        float Damage;            ///< ダメージ量
        bool  IsInvincible;      ///< 無敵中フラグ
    };

    // Combo（連続パリィコンボ関連）
    struct ComboSettings
    {
        bool  IsActive;           ///< コンボ猶予中フラグ
        float Timer;              ///< 猶予タイマー残り時間
        float WindowAfterPerfect; ///< 完璧後次猶予時間
        float ChargeTime;         ///< チャージ蓄積時間
    };


    /**
     * @class  Player
     * @brief  Character を継承したプレイヤーキャラクラス
     */
    class Player : public Character
    {
    private:
        MovementSettings m_MoveMent;
        ZoneSettings m_Zone;
        ParrySettings m_Parry;
        BoostSettings m_Boost;
        AttackSettings m_Attack;
        DamageSettings m_Damage;
		ComboSettings m_Combo;

        float m_TotalTime;                 ///< 全体タイマー
        bool  m_IsGoal;                    ///< ゴール到達フラグ
        bool  m_IsParryCounter;            ///< パリィモード中フラグ
        bool  m_IsCharged;                 ///< チャージフラグ

        Vec3 m_HitScale;                   ///< 被ヒット時スケール補正
        Vec3 m_EffectVec;                  ///< エフェクト位置補正
        shared_ptr<EffectManager> m_Effect;///< エフェクト管理
        Effekseer::Handle m_Handle;        ///< メインエフェクトハンドル
        Effekseer::Handle m_BrinkHandle;   ///< 点滅エフェクトハンドル
        Effekseer::Handle m_ParryHandle;   ///< パリィエフェクトハンドル
        Effekseer::Handle m_EarthQuakeHandle; //// 地震エフェクトハンドル
        shared_ptr<TargetBoard> m_TargetBoard;///< 照準ボード
        shared_ptr<Board> m_EnemyArrow;       ///最も近い敵の方向を示す
		Vec3 m_TargetObject;                  ///< ロックオン対象オブジェクト

    public:
        int m_PlayerStateNum;              ///< 現在の状態フラグ

        /**
         * @enum PlayerState
         * @brief プレイヤー動作状態ビットフラグ
         */
        enum PlayerState
        {
            NORMAL = 0b00000001,  ///< 待機中
            WALK =   0b00000010,  ///< 歩行中
            RUN =    0b00000100,  ///< 走行中
            ATTACK = 0b00001000,  ///< 攻撃中
            ZONE =   0b00010000,  ///< ゾーン中
            DASH =   0b00100000,   ///< ダッシュ中
            ATTACKCHARGE = 0b01000000   ///< ため攻撃中
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
        void BoostMove(const float Speed, const Vec3 Angle, const float time = 0.0f);

        /// @brief ゾーンモード発動
        void ZoneActivation();

        /// @brief デバッグ表示
        void Debug();


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

        void DrawArrow();
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

        /** @brief 溜めアタック再生
        * @param elapsedTime 経過時間[s]
        */
        void HandleAttackCharge(const float& elapsedTime);

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
        const void SetAnim(wstring animname, const bool force = false)
        {
            auto draw = GetComponent<PNTBoneModelDraw>();
            if (draw->GetCurrentAnimation() != animname)
            if (draw->GetAnimeLoop() || force)   draw->ChangeCurrentAnimation(animname, 0.0f);
            else if (draw->IsTargetAnimeEnd())  draw->ChangeCurrentAnimation(animname, 0.0f);
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
            return m_MoveMent.SearchDistance;
        }

        /// @brief 正面ベクトル取得
        Vec3 GetForward() {
            return m_Transform->GetForward();
        }
        /**
         * @brief パリィ判定位置設定
         * @param position 位置
         */
        void SetParryPosition(const Vec3& position) {
            m_EffectVec = position;
        }

        /**
         * @brief ダメージ量設定
         * @param damage 新ダメージ量
         */
        void SetDamage(const float& damage) {
            m_Damage.Damage = damage;
        }

        /**
         * @brief ゴール状態設定
         * @param goal 到達済みならtrue
         */
        void SetIsGaol(const bool& goal) {
            m_IsGoal = goal;
        }

        /// @brief 状態フラグ取得
        int GetStates() {
            return m_PlayerStateNum;
        }

        /// @brief エネルギー量取得
        float GetEnergy() {
            return m_Zone.EnergyCharge;
        }

        /// @brief ダメージ量取得
        float GetDamage() {
            return m_Damage.Damage;
        }

        /// @brief パリィ可能か
        bool GetParry() {
            return m_Parry.IsJudgeActive;
        }

        /// @brief 攻撃可能か判定
        bool IsAttack() {
            return IntervalTimer(true, 0.2f, 0.0f, m_Damage.InvincibleDuration, false) && !(m_PlayerStateNum & PlayerState::ATTACK);
        }

        /// @brief ダッシュ可能か判定
        bool IsDash() {
            return IntervalTimer(true, 0.5f, 0.0f, m_Boost.Cooldown, false) && !(m_PlayerStateNum & PlayerState::DASH);
        }

    };
} // namespace basecross