/*!
 @file ProductionCamera.h
 @brief 演出用のカメラを制御するクラス群
*/

#pragma once
#include "stdafx.h"

namespace basecross {

    /// @brief カメラのアニメーションや挙動を制御するゲームオブジェクト
    class ProductionCameraman : public GameObject {
        Vec3 m_startPos;           ///< カメラの開始位置を保持するベクトル
        Vec3 m_endPos;             ///< カメラの終了位置を保持するベクトル
        Vec3 m_atStartPos;         ///< 注視点の開始位置を保持するベクトル
        Vec3 m_atEndPos;           ///< 注視点の終了位置を保持するベクトル
        Vec3 m_atPos;              ///< 現在の注視点位置を保持するベクトル
        Vec3 m_eyePos;             ///< 現在のカメラ位置を保持するベクトル
        Vec3 m_secondEndPos;       ///< 二次終了位置を保持するベクトル
        Vec3 m_secondAtEndPos;     ///< 二次注視点終了位置を保持するベクトル

        float m_totalTime;         ///< アニメーション全体の再生時間（秒）
        float m_endcurrntTime;     ///< 二次アニメーションの再生時間（秒）
        float m_tempTotalTime;     ///< 一時的な合計時間を保持する変数
        float m_currntTime;        ///< 現在の経過時間を保持する変数

        bool m_switchToMainCamera; ///< 終了後にメインカメラへ切り替えるかどうかのフラグ
        bool m_finished;           ///< アニメーションが終了したかどうかのフラグ
        bool m_isReverse;          ///< アニメーションを逆再生するかどうかのフラグ

        unique_ptr<StateMachine<ProductionCameraman>> m_StateMachine;  ///< ステートマシン本体

    public:
        /// @brief 移動タイプを定義（回転 or 直線）
        enum MoveType {
            Linear = 0, ///< 直線移動
            Orbit = 1  ///< 円軌道移動
        };

        int m_moveType;            ///< 現在の移動タイプ（Linear または Orbit）

        /// @brief コンストラクタ
        /// @param StagePtr ステージへの共有ポインタ
        ProductionCameraman(const shared_ptr<Stage>& StagePtr);

        /// @brief デストラクタ
        ~ProductionCameraman() {}

        /// @brief 初期化処理
        virtual void OnCreate() override;

        /// @brief 毎フレームの更新処理
        virtual void OnUpdate() override;

        /// @brief カメラのオープニングアニメーションを開始する
        /// @param startPos           アニメーション開始時のカメラ位置
        /// @param endPos             アニメーション終了時のカメラ位置
        /// @param atStartPos         アニメーション開始時の注視点位置
        /// @param atEndPos           アニメーション終了時の注視点位置
        /// @param secondEndPos       2段階目のアニメーション終了時のカメラ位置
        /// @param secondAtEndPos     2段階目のアニメーション終了時の注視点位置
        /// @param totalTime          アニメーション全体の再生時間（秒）
        /// @param endtotalTime       二次アニメーションの再生時間（秒）
        /// @param switchToMainCamera 終了後にメインカメラへ切り替えるかどうか
        void StartOpeningAnimation(
            const Vec3& startPos,
            const Vec3& endPos,
            const Vec3& atStartPos,
            const Vec3& atEndPos,
            const Vec3& secondEndPos,
            const Vec3& secondAtEndPos,
            float totalTime,
            float endtotalTime,
            const bool& switchToMainCamera
        );

        /// @brief StateMachine への const 参照を取得する
        const unique_ptr<StateMachine<ProductionCameraman>>& GetStateMachine() {
            return m_StateMachine;
        }

        /// @brief 現在の注視点位置を取得する
        Vec3 GetAtPos() const { return m_atPos; }

        /// @brief 現在のカメラ位置を取得する
        Vec3 GetEyePos() const { return m_eyePos; }

        /// @brief アニメーションをリバース再生するか設定する
        void SetReverse(bool reverse) { m_isReverse = reverse; }

        /// @brief ゴール進入時の挙動を実行する
        void ToGoalEnterBehavior();

        /// @brief 通常動作を実行する
        /// @param totaltime 動作再生時間（秒）
        /// @param isReverse リバース再生フラグ
        /// @return 実行に成功した場合は true
        bool ExcuteBehavior(float totaltime, bool isReverse);

        /// @brief 2段階目の動作を実行する
        /// @param totaltime 動作再生時間（秒）
        /// @return 実行に成功した場合は true
        bool ExcuteEndBehavior(float totaltime);

        /// @brief 終了ステート進入時の挙動
        void EndStateEnterBehavior();

        /// @brief 移動タイプを指定された値に設定する
        /// @param type 0 は Linear、1 は Orbit
        void SetMoveType(const MoveType type) { m_moveType = type; }

        /// @brief アニメーション終了フラグを取得する
        bool GetEndState() const { return m_finished; }

        /// @brief 経過時間をリセットする
        void ResetTime() { m_currntTime = 0.0f; }
    };

    /// @brief ProductionCameraman のフェーズ1（開始→First）を管理するステート
    class ProductionCameramanToFirstState : public ObjState<ProductionCameraman> {
        float m_time;      ///< 経過時間参照
        float m_endTime;   ///< 終了時間参照
        bool  m_return;    ///< リターン動作フラグ

        ProductionCameramanToFirstState(float& time, float& endtime, bool toReturn)
            : m_time(time), m_endTime(endtime), m_return(toReturn) {
        }

    public:
        /// @brief インスタンス取得
        static shared_ptr<ProductionCameramanToFirstState> Instance(
            float& time, float& endtime, bool toReturn);

        /// @brief ステート開始時に呼ばれる処理
        void Enter(const shared_ptr<ProductionCameraman>& Obj) override;

        /// @brief 毎フレーム呼ばれるステート処理
        void Execute(const shared_ptr<ProductionCameraman>& Obj) override;

        /// @brief ステート終了時に呼ばれる処理
        void Exit(const shared_ptr<ProductionCameraman>& Obj) override;
    };

    /// @brief ProductionCameraman のフェーズ2（Return）を管理するステート
    class ProductionCameramanToReturnState : public ObjState<ProductionCameraman> {
        float m_time;    ///< 経過時間参照
        float m_endTime; ///< 終了時間参照

        ProductionCameramanToReturnState(float& time, float& endtime)
            : m_time(time), m_endTime(endtime) {
        }

    public:
        /// @brief インスタンス取得
        static shared_ptr<ProductionCameramanToReturnState> Instance(
            float& time, float& endtime);

        void Enter(const shared_ptr<ProductionCameraman>& Obj) override;
        void Execute(const shared_ptr<ProductionCameraman>& Obj) override;
        void Exit(const shared_ptr<ProductionCameraman>& Obj) override;
    };

    /// @brief ProductionCameraman の終了ステートを管理するクラス
    class ProductionCameramanEndState : public ObjState<ProductionCameraman> {
        float& m_time; ///< 経過時間参照

        ProductionCameramanEndState(float& time) : m_time(time) {}

    public:
        /// @brief インスタンス取得
        static shared_ptr<ProductionCameramanEndState> Instance(float& time);

        void Enter(const shared_ptr<ProductionCameraman>& Obj) override;
        void Execute(const shared_ptr<ProductionCameraman>& Obj) override;
        void Exit(const shared_ptr<ProductionCameraman>& Obj) override;
    };

    /// @brief シーン用カメラの生成と更新処理を行うクラス
    class ProductionCamera : public Camera {
    public:
        /// @brief コンストラクタ
        ProductionCamera();

        /// @brief デストラクタ
        virtual ~ProductionCamera();

        /// @brief 初期化処理
        void OnCreate() override;

        /// @brief 毎フレームの更新処理
        void OnUpdate() override;
    };

}  // namespace basecross