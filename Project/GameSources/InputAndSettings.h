/**
 * @file InputAndSettings.h
 * @brief 入力状態、リーダークラス、および各種ゲームプレイ設定を定義
 */

#pragma once


#include "stdafx.h"

namespace basecross {
    /**
 * @struct InputState
 * @brief 各フレームの入力ボタン状態と移動軸値を保持する構造体
 */
    struct InputState
    {
        /// 攻撃ボタンを押した瞬間（Aボタン／左クリック）
        bool attackPressed = false;

        /// 攻撃ボタンをホールド中（Aボタン／左クリック押下中）
        bool attackHeld = false;

        /// 攻撃ボタンを離した瞬間（Aボタン／左クリック離し）
        bool attackReleased = false;

        /// ダッシュボタンを押した瞬間（Xボタン／右クリック）
        bool dashPressed = false;

        /// ダッシュボタンをホールド中（Xボタン／右クリック押下中）
        bool dashHeld = false;

        /// ゾーン発動ボタンを押した瞬間（Bボタン／スペース）
        bool zonePressed = false;

        /// パリィカウンターボタン（攻撃ボタンと同義）
        bool parryPressed = false;

        /// 水平移動入力値（-1.0: 左, +1.0: 右）
        float moveX = 0.0f;

        /// 垂直移動入力値（-1.0: 下, +1.0: 上）
        float moveY = 0.0f;
    };

    /**
     * @class InputReader
     * @brief シングルトンとして入力デバイスをポーリングし、InputStateを生成するクラス
     */
    class InputReader
    {
    public:
        /**
         * @brief InputReaderのシングルトンインスタンスを取得
         * @return InputReaderの参照
         */
        static InputReader& Get()
        {
            static InputReader inst;
            return inst;
        }

        /**
         * @brief 各フレームのキー・コントローラ状態を読み取り、InputStateを返す
         * @return 現在の入力状態を格納したInputState
         */
        InputState Read()
        {
            InputState in;
            auto& dev = App::GetApp()->GetInputDevice();
            auto& ks = dev.GetKeyState();
            auto& cv = dev.GetControlerVec()[0];

            // ─── 移動軸 ─────────────────────────────
            if (cv.bConnected)
            {
                in.moveX = cv.fThumbLX;
                in.moveY = cv.fThumbLY;
            }
            if (ks.m_bPushKeyTbl['A']) in.moveX = -1.0f;
            if (ks.m_bPushKeyTbl['D']) in.moveX = 1.0f;
            if (ks.m_bPushKeyTbl['W']) in.moveY = 1.0f;
            if (ks.m_bPushKeyTbl['S']) in.moveY = -1.0f;

            // ─── 攻撃ボタン (A / 左クリック) ───────────
            in.attackPressed = (cv.wPressedButtons & XINPUT_GAMEPAD_A)
                || ks.m_bPressedKeyTbl[VK_LBUTTON];
            in.attackHeld = (cv.wButtons & XINPUT_GAMEPAD_A)
                || ks.m_bPushKeyTbl[VK_LBUTTON];
            in.attackReleased = (cv.wReleasedButtons & XINPUT_GAMEPAD_A)
                || ks.m_bUpKeyTbl[VK_LBUTTON];

            // ─── ダッシュボタン (X / 右クリック) ───────
            in.dashPressed = (cv.wPressedButtons & XINPUT_GAMEPAD_X)
                || ks.m_bPressedKeyTbl[VK_RBUTTON];
            in.dashHeld = (cv.wButtons & XINPUT_GAMEPAD_X)
                || ks.m_bPushKeyTbl[VK_RBUTTON];

            // ─── ゾーン発動ボタン (B / スペース) ────────
            in.zonePressed = (cv.wPressedButtons & XINPUT_GAMEPAD_B)
                || ks.m_bPressedKeyTbl[VK_SPACE];

            // ─── パリィ用入力 (攻撃ボタンと同じ) ─────────
            in.parryPressed = in.attackPressed;

            return in;
        }

    private:
        InputReader() = default;
        ‾InputReader() = default;
        InputReader(const InputReader&) = delete;
        InputReader& operator=(const InputReader&) = delete;
    };

    /**
     * @struct MovementSettings
     * @brief キャラクターの移動速度や回転設定をまとめた構造体
     */
    struct MovementSettings
    {
        /// 移動速度（ユニット／秒）
        float MoveSpeed;

        /// 現在のヨー回転角（ラジアン）
        float CurrentYaw;

        /// 回転補間の速さ（値が大きいほど瞬時、小さいほどゆっくり）
        float RotationSpeed;

        /// 敵を探索する最大距離
        float SearchDistance;

        /// ブースト時の移動方向ベクトル
        Vec3  BoostAngle;
    };

    /**
     * @struct ZoneSettings
     * @brief ゾーンチャージと発動時間を管理する構造体
     */
    struct ZoneSettings
    {
        /// エネルギーチャージ量（0.0〜1.0）
        float EnergyCharge;

        /// ゾーン状態に入ってからの累積時間
        float ZoneTime;

        /// ゾーン発動アニメーションの進行度（秒）
        float ZoneAnimProgress;

        /// ゾーン演出の進行度
        float ZoneAnim;
    };

    /**
     * @struct ParrySettings
     * @brief パリィ判定とカウンターに関する設定と状態フラグ
     */
    struct ParrySettings
    {
        /// パリィ受付時間（秒）
        float JudgeTime;

        /// パリィ後の無敵インターバル時間（秒）
        float DamageInterval;

        /// 完璧パリィ判定ウィンドウ（秒）
        float PerfectWindow;

        /// パリィ受付中かどうか
        bool  IsJudgeActive;

        /// 無敵インターバル中かどうか
        bool  IsInDamageInterval;

        /// 完璧パリィに成功したかどうか
        bool  IsPerfectParry;

        /// カウンターヒット済みかどうか
        bool  HasCountered;
    };

    /**
     * @struct BoostSettings
     * @brief ブースト移動とカウンターブーストの設定
     */
    struct BoostSettings
    {
        /// 通常ブースト継続時間（秒）
        float Duration;

        /// カウンターブースト継続時間（秒）
        float CounterTime;

        /// ブースト再使用までのクールダウン時間（秒）
        float Cooldown;

        /// ブースト使用可能フラグ
        bool  IsReady;
    };

    /**
     * @struct AttackSettings
     * @brief 通常攻撃およびチャージ攻撃の挙動設定
     */
    struct AttackSettings
    {
        /// 攻撃アニメーション再生時間（秒）
        float AnimTime;

        /// チャージ攻撃待機時間（秒）
        float ChargeInterval;

        /// チャージ攻撃持続時間（秒）
        float ChargeTime;

        /// 通常攻撃間のインターバル（秒）
        float Interval;

        /// 攻撃コンボ維持の最大インターバル（秒）
        float MaxInterval;

        /// チャージ攻撃中フラグ
        bool  IsCharging;

        /// 攻撃中フラグ
        bool  IsAttack;

        /// 使用するアニメーション名
        std::wstring AnimName;
    };

    /**
     * @struct DamageSettings
     * @brief 被ダメージ時の無敵時間や点滅エフェクト設定
     */
    struct DamageSettings
    {
        /// 被ダメ時の無敵持続時間（秒）
        float InvincibleDuration;

        /// 無敵時の点滅周期（秒）
        float BlinkingInterval;

        /// ダメージ量
        float Damage;

        /// 無敵中フラグ
        bool  IsInvincible;
    };

    /**
     * @struct ComboSettings
     * @brief パリィコンボのタイミング管理と状態
     */
    struct ComboSettings
    {
        /// コンボ猶予時間が有効かどうか
        bool  IsActive;

        /// 残り猶予時間（秒）
        float Timer;

        /// 完璧パリィ後に追加される猶予時間（秒）
        float WindowAfterPerfect;

        /// コンボのチャージ蓄積時間（秒）
        float ChargeTime;
    };


}