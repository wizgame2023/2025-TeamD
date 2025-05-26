/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Player;
	class OpeningCameraman;


	class OpeningCamera : public Camera {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		OpeningCamera();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~OpeningCamera();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 更新処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};

	class OpeningCameramanToFirstState : public ObjState<OpeningCameraman>
	{
		OpeningCameramanToFirstState() {}
	public:
		static shared_ptr<OpeningCameramanToFirstState> Instance();
		virtual void Enter(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Execute(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Exit(const shared_ptr<OpeningCameraman>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanToStartState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	class OpeningCameramanToSecondState : public ObjState<OpeningCameraman>
	{
		OpeningCameramanToSecondState() {}
	public:
		static shared_ptr<OpeningCameramanToSecondState> Instance();
		virtual void Enter(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Execute(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Exit(const shared_ptr<OpeningCameraman>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanEndState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	class OpeningCameramanEndState : public ObjState<OpeningCameraman>
	{
		OpeningCameramanEndState() {}
	public:
		static shared_ptr<OpeningCameramanEndState> Instance();
		virtual void Enter(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Execute(const shared_ptr<OpeningCameraman>& Obj)override;
		virtual void Exit(const shared_ptr<OpeningCameraman>& Obj)override;
	};

	class OpeningCameraman : public GameObject {
		Vec3 m_startPos;                          // カメラの開始位置を保持するベクトル
		Vec3 m_endPos;                            // カメラの終了位置を保持するベクトル
		Vec3 m_atStartPos;                        // 注視点の開始位置を保持するベクトル
		Vec3 m_atEndPos;                          // 注視点の終了位置を保持するベクトル
		Vec3 m_atPos;                             // 現在の注視点位置を保持するベクトル
		Vec3 m_eyePos;                            // 現在のカメラ位置を保持するベクトル
		float m_totalTime;                        // 合計時間を保持する変数
		Vec3 m_secondEndPos;                      // 二次終了位置を保持するベクトル
		Vec3 m_secondAtEndPos;                    // 二次注視点終了位置を保持するベクトル

		Vec3 m_tempStartPos;                      // 一時的な開始位置を保持するベクトル
		Vec3 m_tempEndPos;                        // 一時的な終了位置を保持するベクトル
		Vec3 m_tempAtStartPos;                    // 一時的な注視点開始位置を保持するベクトル
		Vec3 m_tempAtEndPos;                      // 一時的な注視点終了位置を保持するベクトル
		Vec3 m_tempAtPos;                         // 一時的な注視点位置を保持するベクトル
		float m_tempTotalTime;                    // 一時的な合計時間を保持する変数

		// ステートマシン
		unique_ptr< StateMachine<OpeningCameraman> >  m_StateMachine;
	public:
		// 構築と破棄
		OpeningCameraman(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos, const Vec3& EndPos,
			const Vec3& AtStartPos, const Vec3& AtEndPos, const Vec3& AtPos, float& TotalTime,
			const Vec3& secondEndPos, const Vec3& secondAtEndPos);
		virtual ~OpeningCameraman();

		// 初期化
		virtual void OnCreate() override;

		// 操作
		virtual void OnUpdate() override;

		// アクセサ
		const unique_ptr<StateMachine<OpeningCameraman>>& GetStateMachine() {
			return m_StateMachine;
		}

		Vec3 GetAtPos() const {
			return m_atPos;
		}

		Vec3 GetEyePos() const {
			return m_eyePos;
		}

		// ゴールエンタービヘイビア
		void ToGoalEnterBehavior();

		// スタートエンタービヘイビア
		void ToStartEnterBehavior();

		// 行動を実行する
		bool ExcuteBehavior(float totaltime);

		// 終了状態エンタービヘイビア
		void EndStateEnterBehavior();
	};
}
