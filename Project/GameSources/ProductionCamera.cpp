/*!
@file Scene.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	ProductionCamera::ProductionCamera() :
		Camera()
	{
	}
	ProductionCamera::‾ProductionCamera() {}


	void ProductionCamera::OnCreate() {
	}

	void ProductionCamera::OnUpdate() {
		Camera::OnUpdate();

		auto ptrOpeningCameraman = dynamic_pointer_cast<ProductionCameraman>(GetCameraObject());
		if (ptrOpeningCameraman) {
			auto pos = ptrOpeningCameraman->GetAtPos();
			auto eye = ptrOpeningCameraman->GetEyePos();
			SetEye(eye);
			SetAt(pos);
		}
	}


	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanToGoalState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	shared_ptr<ProductionCameramanToFirstState> ProductionCameramanToFirstState::Instance(float& time, float& endtime) {
		static shared_ptr<ProductionCameramanToFirstState> instance(new ProductionCameramanToFirstState(time, endtime));
		instance->m_time = time; // 時間を保存  		
		instance->m_endTime = endtime; // 時間を保存  		
		return instance;
	}
	void ProductionCameramanToFirstState::Enter(const shared_ptr<ProductionCameraman>& Obj) {
		Obj->ToGoalEnterBehavior();
	}
	void ProductionCameramanToFirstState::Execute(const shared_ptr<ProductionCameraman>& Obj) {
		if (Obj->ExcuteBehavior(m_time)) {
			Obj->GetStateMachine()->ChangeState(ProductionCameramanEndState::Instance(m_endTime));
		}
	}
	void ProductionCameramanToFirstState::Exit(const shared_ptr<ProductionCameraman>& Obj) {
	}

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanEndState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	shared_ptr<ProductionCameramanEndState> ProductionCameramanEndState::Instance(float& time) { 
		static shared_ptr<ProductionCameramanEndState> instance(new ProductionCameramanEndState(time));
		instance->m_time = time; // 時間を保存  		
		return instance;
	}
	void ProductionCameramanEndState::Enter(const shared_ptr<ProductionCameraman>& Obj) {
	}
	void ProductionCameramanEndState::Execute(const shared_ptr<ProductionCameraman>& Obj) {
		if (Obj->ExcuteEndBehavior(m_time))
		{
			Obj->EndStateEnterBehavior();
		}
	}
	void ProductionCameramanEndState::Exit(const shared_ptr<ProductionCameraman>& Obj) {
	}


	ProductionCameraman::ProductionCameraman(const shared_ptr<Stage>& StagePtr): 
		GameObject(StagePtr),
		m_currntTime(0.0f),
		m_moveType(MoveType::Linear),
		m_tempTotalTime(0.0f),
		m_totalTime(0.0f),
		m_switchToMainCamera(true) // メインカメラに切り替えるかどうかのフラグ
	{}

	//初期化
	void ProductionCameraman::OnCreate() {
		//初期位置などの設定
		auto ptr = AddComponent<Transform>();
		ptr->SetScale(0.25f, 0.25f, 0.25f);	//直径25センチの球体
		ptr->SetRotation(0.0f, 0.0f, 0.0f);
		ptr->SetPosition(m_startPos);
		//ステートマシンの構築
		m_StateMachine.reset(new StateMachine<ProductionCameraman>(GetThis<ProductionCameraman>()));
	}

	//操作
	void ProductionCameraman::OnUpdate() {
		//ステートマシンのUpdateを行う
		//この中でステートの切り替えが行われる
		if (!m_finished)
		{
			m_StateMachine->Update();
		}
	}


	// カメラのオープニング演出を開始（位置や視線を引数で指定可能）
	void ProductionCameraman::StartOpeningAnimation(
		const Vec3& startPos,     // 補間開始時のカメラ位置
		const Vec3& endPos,       // 補間終了時のカメラ位置
		const Vec3& atStartPos,   // 補間開始時の視線ターゲット
		const Vec3& atEndPos,     // 補間終了時の視線ターゲット
		const Vec3& secondEndPos, // 後半フェーズ用のカメラ位置
		const Vec3& secondAtEndPos, // 後半フェーズ用の視線ターゲット
		float totalTime,           // 補間にかかる総時間
		float endtotalTime,           // 補間にかかる総時間
		const bool& switchToMainCamera// メインカメラに切り替えるかどうかのフラグ
	) {
		m_finished = false; 
		// 位置・視線の初期化
		m_startPos = startPos;
		m_endPos = endPos;
		m_atStartPos = atStartPos;
		m_atEndPos = atEndPos;
		m_secondEndPos = secondEndPos;
		m_secondAtEndPos = secondAtEndPos;
		m_totalTime = totalTime;
		m_endcurrntTime = 0.0f;
		m_switchToMainCamera = switchToMainCamera;
		m_currntTime = 0.0f;
		// ステートマシンを初期状態に変更
		m_StateMachine.reset(new StateMachine<ProductionCameraman>(GetThis<ProductionCameraman>()));
		m_StateMachine->ChangeState(ProductionCameramanToFirstState::Instance(totalTime, endtotalTime));

		// 初期視線と位置の設定
		m_eyePos = m_startPos;
		m_atPos = m_atStartPos;
	}

	void ProductionCameraman::ToGoalEnterBehavior() { //後半部
		m_startPos; //カメラの最初の位置
		m_endPos; //カメラの最後の位置
		m_atStartPos; //最初に見てる方角
		m_atEndPos; //最後に見てる方角
		m_atPos;//カメラ最後の位置
		m_totalTime;
	}

	// カメラ移動の処理（回転 or 直線）
	bool ProductionCameraman::ExcuteBehavior(float totaltime) {
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		m_currntTime += ElapsedTime;

		if (m_currntTime > totaltime) {
			return true;
		}

		if (m_moveType == MoveType::Orbit) {
			// 円軌道での移動
			float startDistance = (m_startPos - m_atStartPos).length();
			float endDistance = (m_endPos - m_atEndPos).length();
			float distance = Lerp::CalculateLerp(startDistance, endDistance, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);

			float initialAngle = atan2(m_startPos.z - m_atStartPos.z, m_startPos.x - m_atStartPos.x);
			float easedRotationRatio = Lerp::CalculateLerp(0.0f, 1.0f, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);
			float currentAngle = initialAngle + easedRotationRatio * XM_2PI;

			Vec3 target = Lerp::CalculateLerp(m_atStartPos, m_atEndPos, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);
			float interpHeight = Lerp::CalculateLerp(m_startPos.y, m_endPos.y, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);

			float newX = target.x + distance * cos(currentAngle);
			float newZ = target.z + distance * sin(currentAngle);
			m_eyePos = Vec3(newX, interpHeight, newZ);
			m_atPos = Lerp::CalculateLerp(m_atStartPos, m_atEndPos, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);
		}
		else {
			// 直線移動の処理
			m_eyePos = Lerp::CalculateLerp(m_startPos, m_endPos, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);
			m_atPos = Lerp::CalculateLerp(m_atStartPos, m_atEndPos, 0.0f, totaltime, m_currntTime, Lerp::rate::Cube);
		}

		auto ptrTrans = GetComponent<Transform>();
		ptrTrans->SetPosition(m_eyePos);

		return false;
	}
	bool ProductionCameraman::ExcuteEndBehavior(float totaltime) {
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		m_endcurrntTime += ElapsedTime;

		if (m_endcurrntTime > totaltime) {
			return true;
		}
		return false;
	}

	void ProductionCameraman::EndStateEnterBehavior() {
		m_finished = true; //演出が終了したことを通知
		// 演出終了後に MainCamera に切り替える設定なら変更を実行
		if (m_switchToMainCamera) {
			GetTypeStage<GameStage>()->ToMainCamera();
		}
	}

}
