/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	/// @brief ProductionCameraman 
	/// クラスは、カメラのアニメーションや挙動を制御するためのゲームオブジェクトです。
	/// カメラの位置や注視点の管理、アニメーションの開始、状態遷移の管理などを行います。
	class ProductionCameraman : public GameObject {
		Vec3 m_startPos;                          // カメラの開始位置を保持するベクトル
		Vec3 m_endPos;                            // カメラの終了位置を保持するベクトル
		Vec3 m_atStartPos;                        // 注視点の開始位置を保持するベクトル
		Vec3 m_atEndPos;                          // 注視点の終了位置を保持するベクトル
		Vec3 m_atPos;                             // 現在の注視点位置を保持するベクトル
		Vec3 m_eyePos;                            // 現在のカメラ位置を保持するベクトル
		Vec3 m_secondEndPos;                      // 二次終了位置を保持するベクトル
		Vec3 m_secondAtEndPos;                    // 二次注視点終了位置を保持するベクトル

		float m_totalTime;                        // 合計時間を保持する変数
		float m_endcurrntTime;                        // 合計時間を保持する変数
		float m_tempTotalTime;                    // 一時的な合計時間を保持する変数
		float m_currntTime;

		bool m_switchToMainCamera; // メインカメラに切り替えるかどうかのフラグ
		bool m_finished;        // アニメーションが終了したかどうかのフラグ

		// ステートマシン
		unique_ptr< StateMachine<ProductionCameraman> >  m_StateMachine;

	public:
		// 移動タイプを定義（回転 or 直線）
		enum MoveType {
			Linear = 0, // 直線移動
			Orbit = 1   // 円軌道（現在の動き）
		};

		// OpeningCameraman クラスのメンバ変数
		int m_moveType;

		// 構築と破棄
		ProductionCameraman::ProductionCameraman(const shared_ptr<Stage>& StagePtr);
		ProductionCameraman::‾ProductionCameraman() {}

		// 初期化
		virtual void OnCreate() override;

		// 操作
		virtual void OnUpdate() override;

		/// @brief カメラのアニメーションを開始する。
		/// @param startPos アニメーション開始時のカメラ位置。
		/// @param endPos アニメーション終了時のカメラ位置。
		/// @param atStartPos アニメーション開始時の注視点位置。
		/// @param atEndPos アニメーション終了時の注視点位置。
		/// @param secondEndPos 2段階目のアニメーション終了時のカメラ位置。
		/// @param secondAtEndPos 2段階目のアニメーション終了時の注視点位置。
		/// @param totalTime アニメーション全体の再生時間（秒単位）。
		/// @param switchToMainCamera アニメーション終了後にメインカメラへ切り替えるかどうか。
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

		/// @brief ProductionCameraman用のStateMachineへの参照を取得します。
		/// @return ProductionCameraman型のStateMachineへのconst unique_ptr参照。
		const unique_ptr<StateMachine<ProductionCameraman>>& GetStateMachine() {
			return m_StateMachine;
		}

		Vec3 GetAtPos() const {
			return m_atPos;
		}

		Vec3 GetEyePos() const {
			return m_eyePos;
		}

		/// @brief ゴール進入時の挙動を実行します。
		void ToGoalEnterBehavior();

		/// @brief 指定された合計時間に基づいて動作を実行します。
		/// @param totaltime 動作を実行するための合計時間（秒単位）。
		/// @return 動作が正常に実行された場合は true、失敗した場合は false を返します。
		bool ExcuteBehavior(float totaltime);
		bool ExcuteEndBehavior(float totaltime);
		// 終了状態エンタービヘイビア
		void EndStateEnterBehavior();

		/// @brief 移動タイプを指定された値に設定します。
		/// @param type 設定する移動タイプを表す整数値。0は直線移動、1は円軌道移動を指定します。
		void SetMoveType(const int type)
		{
			if (type == 0) {
				m_moveType = MoveType::Linear; // 直線移動
			}
			else if (type == 1) {
				m_moveType = MoveType::Orbit; // 円軌道移動
			}
		}

		bool GetEndState() const {
			return m_finished;
		}
	};
    /// @brief ProductionCameraman オブジェクトの最初の状態を管理するステートクラスです。
    class ProductionCameramanToFirstState : public ObjState<ProductionCameraman>  
    {  
		float m_time; // 時間を参照する変数  
		float m_endTime; // 時間を参照する変数  

        // コンストラクタでメンバー変数を初期化する  
        ProductionCameramanToFirstState(float& time, float& endtime) : m_time(time), m_endTime(endtime) {}

    public:  
        static shared_ptr<ProductionCameramanToFirstState> Instance(float& time, float& endtime);
        virtual void Enter(const shared_ptr<ProductionCameraman>& Obj) override;  
        virtual void Execute(const shared_ptr<ProductionCameraman>& Obj) override;  
        virtual void Exit(const shared_ptr<ProductionCameraman>& Obj) override;  
    };

	/// @brief ProductionCameramanEndState クラスは、ProductionCameraman オブジェクトの終了状態を管理します。
	class ProductionCameramanEndState : public ObjState<ProductionCameraman>
	{
		float& m_time; // 時間を参照する変数  

		ProductionCameramanEndState(float& time) : m_time(time) {}
	public:
		static shared_ptr<ProductionCameramanEndState> Instance(float& time);
		virtual void Enter(const shared_ptr<ProductionCameraman>& Obj)override;
		virtual void Execute(const shared_ptr<ProductionCameraman>& Obj)override;
		virtual void Exit(const shared_ptr<ProductionCameraman>& Obj)override;
	};

	/// @brief ProductionCamera クラスは Camera クラスを継承し、カメラの生成や更新処理を提供します。
	class ProductionCamera : public Camera {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		ProductionCamera();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ‾ProductionCamera();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};

	
}
