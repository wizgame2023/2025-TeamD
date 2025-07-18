/*!
 * @file   PlayerAttack.h
 * @brief  プレイヤーなど
 */

#pragma once

#include "stdafx.h"

namespace basecross {
    /**
     * @class  BaseHitObject
     * @brief  全てのヒット判定オブジェクトの基底クラス
     *
     * このクラスは共通のメンバ変数や初期化・更新・衝突処理を
     * 提供し、派生クラスで処理をカスタマイズできるようにしている。
     */
    class BaseHitObject : public Object {
    protected:
        Vec3                        m_Position;    /**< ワールド空間での座標 */
        Vec3                        m_Scale;       /**< 当たり判定オブジェクトのスケール */
        shared_ptr<GameObject>     m_Player;      /**< 発射元のプレイヤーオブジェクト */
        shared_ptr<EffectManager>  m_Effect;      /**< エフェクト管理クラスへの参照 */
        Effekseer::Handle          m_MainHandle = -1; /**< メインエフェクトのハンドル */
        Effekseer::Handle          m_HitHandle = -1; /**< ヒット時エフェクトのハンドル */

        /**
         * @brief 敵にヒットしたときに呼び出される純粋仮想メソッド
         * @param enemy  触れた敵キャラクターへの参照
         *
         * ダメージ判定やチャージ付与など個別の挙動は
         * 派生クラスで実装すること。
         */
        virtual void ApplyHit(shared_ptr<Character> enemy) = 0;

    public:
        /**
         * @brief コンストラクタ
         * @param stage   所属ステージ
         * @param position 初期位置
         * @param scale    当たり判定のスケール
         * @param player   発射元プレイヤー
         */
        BaseHitObject(
            const shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& scale,
            const shared_ptr<GameObject>& player
        );

        /** @brief デストラクタ */
        virtual ‾BaseHitObject();

        /** @brief 生成時に一度だけ呼ばれる初期化処理 */
        virtual void OnCreate() override;

        /** @brief 毎フレーム呼び出される更新処理 */
        virtual void OnUpdate() override;

        /**
         * @brief 衝突開始時に呼ばれる処理
         * @param other  衝突相手のゲームオブジェクト
         */
        virtual void OnCollisionEnter(shared_ptr<GameObject>& other) override;
    };

    /**
     * @class  HitSphere
     * @brief  飛翔型のヒット判定オブジェクト
     *
     * プレイヤーの前方へ飛ばし、一定時間または敵衝突まで移動し続ける。
     */
    class HitSphere : public BaseHitObject {
        Vec3  m_Direction;   /**< 飛翔方向（単位ベクトル） */
        float m_Speed;       /**< 飛翔速度 */
        float m_FlyingTime;  /**< 最大飛行時間 */
        float m_Elapsed = 0.0f; /**< 経過時間 */

    protected:
        /**
         * @brief ヒット時のダメージ処理を実装
         * @param enemy  被ヒットキャラクター
         */
        void ApplyHit(shared_ptr<Character> enemy) override;

    public:
        /**
         * @brief コンストラクタ
         * @param stage     所属ステージ
         * @param position  初期位置
         * @param forward   発射方向（ワールド空間）
         * @param player    発射元プレイヤー
         * @param scale     判定サイズ
         * @param length    飛行可能距離
         */
        HitSphere(
            const shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& forward,
            const shared_ptr<GameObject>& player,
            const Vec3& scale,
            float                    length
        );

        ‾HitSphere() override = default;

        /** @brief エフェクトやパーティクルの生成処理 */
        virtual void OnCreate() override;

        /** @brief 速度に基づく移動＆寿命管理 */
        virtual void OnUpdate() override;
    };

    /**
     * @class  ChargeHitSphere
     * @brief  チャージ攻撃用ヒット判定オブジェクト
     *
     * チャージ時間経過でエフェクトやダメージ倍率が変動する。
     */
    class ChargeHitSphere : public BaseHitObject {
        float m_TotalTime;  /**< チャージ完了までの必要時間 */
        float m_ChargeRate; /**< 現在のチャージ進行度（0〜1） */

    protected:
        /**
         * @brief チャージ率に応じたダメージ／効果を適用
         * @param enemy  被ヒットキャラクター
         */
        void ApplyHit(shared_ptr<Character> enemy) override;

    public:
        /**
         * @brief コンストラクタ
         * @param stage      所属ステージ
         * @param position   初期位置
         * @param scale      判定サイズ
         * @param player     発射元プレイヤー
         * @param chargeTime チャージに要する時間
         */
        ChargeHitSphere(
            const shared_ptr<Stage>& stage,
            const Vec3& position,
            const Vec3& scale,
            const shared_ptr<GameObject>& player,
            float                    chargeTime
        );

        ‾ChargeHitSphere() override = default;

        /** @brief チャージエフェクトの生成 */
        virtual void OnCreate() override;

        /** @brief 経過時間でチャージ率を更新 */
        virtual void OnUpdate() override;
    };

    /**
     * @class  CounterHitSphere
     * @brief  カウンター攻撃用のヒット判定オブジェクト
     *
     * プレイヤーに追従しつつ一定時間後に拡大し、
     * 衝突時に相対位置に応じた攻撃を行う。
     */
    class CounterHitSphere : public BaseHitObject {
        float m_AttachDuration; /**< プレイヤーに追従する総時間 */
        float m_Elapsed = 0.0f; /**< 追従経過時間 */
        Vec3  m_LocalOffset;   /**< プレイヤー基準の相対位置 */
        float m_ChargeRate;    /**< 時間経過時の拡大速度 */

    protected:
        /**
         * @brief カウンター成立時のダメージ処理
         * @param enemy  被ヒットキャラクター
         */
        void ApplyHit(shared_ptr<Character> enemy) override;

    public:
        /**
         * @brief コンストラクタ
         * @param stage        所属ステージ
         * @param player       発射元プレイヤー（追従対象）
         * @param offset       プレイヤーからの相対オフセット
         * @param attachTime   追従持続時間
         * @param chargeRate   拡大率の係数
         */
        CounterHitSphere(
            const shared_ptr<Stage>& stage,
            const shared_ptr<GameObject>& player,
            const Vec3& offset,
            float attachTime,
            float chargeRate = 1.0f
        );

        ‾CounterHitSphere() override = default;

        /** @brief 追従エフェクトの生成と初期設定 */
        virtual void OnCreate() override;

        /** @brief 追従ロジックと拡大処理 */
        virtual void OnUpdate() override;
    };

}