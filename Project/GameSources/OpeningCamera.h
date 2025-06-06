/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
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
		float m_tempTotalTime;                    // 一時的な合計時間を保持する変数
		float m_currntTime;

		bool m_switchToMainCamera; // メインカメラに切り替えるかどうかのフラグ

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
		ProductionCameraman::~ProductionCameraman() {}

		// 初期化
		virtual void OnCreate() override;

		// 操作
		virtual void OnUpdate() override;

		void StartOpeningAnimation(
			const Vec3& startPos,     // 補間開始時のカメラ位置
			const Vec3& endPos,       // 補間終了時のカメラ位置
			const Vec3& atStartPos,   // 補間開始時の視線ターゲット
			const Vec3& atEndPos,     // 補間終了時の視線ターゲット
			const Vec3& secondEndPos, // 後半フェーズ用のカメラ位置
			const Vec3& secondAtEndPos, // 後半フェーズ用の視線ターゲット
			float totalTime,           // 補間にかかる総時間
			const bool& switchToMainCamera// メインカメラに切り替えるかどうかのフラグ
		);
		// アクセサ
		const unique_ptr<StateMachine<ProductionCameraman>>& GetStateMachine() {
			return m_StateMachine;
		}

		Vec3 GetAtPos() const {
			return m_atPos;
		}

		Vec3 GetEyePos() const {
			return m_eyePos;
		}

		void ToGoalEnterBehavior();

		// 行動を実行する
		bool ExcuteBehavior(float totaltime);

		// 終了状態エンタービヘイビア
		void EndStateEnterBehavior();

		void SetMoveType(const int type)
		{
			if (type == 0) {
				m_moveType = MoveType::Linear; // 直線移動
			}
			else if (type == 1) {
				m_moveType = MoveType::Orbit; // 円軌道移動
			}
		}
	};

    class ProductionCameramanToFirstState : public ObjState<ProductionCameraman>  
    {  
        float& m_time; // 時間を参照する変数  

        // コンストラクタでメンバー変数を初期化する  
        ProductionCameramanToFirstState(float& time) : m_time(time) {} 

    public:  
        static shared_ptr<ProductionCameramanToFirstState> Instance(float& time);  
        virtual void Enter(const shared_ptr<ProductionCameraman>& Obj) override;  
        virtual void Execute(const shared_ptr<ProductionCameraman>& Obj) override;  
        virtual void Exit(const shared_ptr<ProductionCameraman>& Obj) override;  
    };

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanEndState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	class ProductionCameramanEndState : public ObjState<ProductionCameraman>
	{
		ProductionCameramanEndState() {}
	public:
		static shared_ptr<ProductionCameramanEndState> Instance();
		virtual void Enter(const shared_ptr<ProductionCameraman>& Obj)override;
		virtual void Execute(const shared_ptr<ProductionCameraman>& Obj)override;
		virtual void Exit(const shared_ptr<ProductionCameraman>& Obj)override;
	};


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
		virtual ~ProductionCamera();
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
