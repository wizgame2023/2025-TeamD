/*!
@file Scene.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	OpeningCamera::OpeningCamera() :
		Camera()
	{
	}
	OpeningCamera::~OpeningCamera() {}


	void OpeningCamera::OnCreate() {
	}

	void OpeningCamera::OnUpdate() {
		Camera::OnUpdate();

		auto ptrOpeningCameraman = dynamic_pointer_cast<OpeningCameraman>(GetCameraObject());
		if (ptrOpeningCameraman) {
			auto pos = ptrOpeningCameraman->GetAtPos();
			auto eye = ptrOpeningCameraman->GetEyePos();
			SetEye(eye);
			SetAt(pos);
		}
	}

	OpeningCameraman::OpeningCameraman(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos, const Vec3& EndPos,
		const Vec3& AtStartPos, const Vec3& AtEndPos, const Vec3& AtPos, float& TotalTime,
		const Vec3& secondEndPos, const Vec3& secondAtEndPos) :
		GameObject(StagePtr),
		m_startPos(StartPos),
		m_endPos(EndPos),
		m_atStartPos(AtStartPos),
		m_atEndPos(AtEndPos),
		m_atPos(AtStartPos),
		m_totalTime(TotalTime),
		m_secondEndPos(secondEndPos),
		m_secondAtEndPos(secondAtEndPos)
	{
	}
	OpeningCameraman::~OpeningCameraman() {}

	//初期化
	void OpeningCameraman::OnCreate() {
		//初期位置などの設定
		auto ptr = GetComponent<Transform>();
		ptr->SetScale(0.25f, 0.25f, 0.25f);	//直径25センチの球体
		ptr->SetRotation(0.0f, 0.0f, 0.0f);
		ptr->SetPosition(m_startPos);
		//ステートマシンの構築
		m_StateMachine.reset(new StateMachine<OpeningCameraman>(GetThis<OpeningCameraman>()));
		//最初のステートをOpeningCameramanToGoalStateに設定
		m_StateMachine->ChangeState(OpeningCameramanToFirstState::Instance());

		//後半用の一時的な格納場所
		m_tempStartPos = m_startPos;
		m_tempEndPos = m_endPos;
		m_tempAtStartPos = m_atStartPos;
		m_tempAtEndPos = m_atEndPos;
		m_tempAtPos = m_atPos;
		m_tempTotalTime = m_totalTime;
	}

	//操作
	void OpeningCameraman::OnUpdate() {
		//ステートマシンのUpdateを行う
		//この中でステートの切り替えが行われる
		m_StateMachine->Update();
	}

	void OpeningCameraman::ToGoalEnterBehavior() { //後半部
		m_startPos; //カメラの最初の位置
		m_endPos; //カメラの最後の位置
		m_atStartPos; //最初に見てる方角
		m_atEndPos; //最後に見てる方角
		m_atPos;//カメラ最後の位置
		m_totalTime;
	}

	void OpeningCameraman::ToStartEnterBehavior() { //前半部
		m_startPos = m_tempEndPos; //カメラの最初の位置
		m_endPos = m_secondEndPos; //カメラの最後の位置
		m_atStartPos = m_tempAtEndPos; //最初に見てる方角
		m_atEndPos = m_secondAtEndPos; //最後に見てる方角
		m_atPos;//カメラ最後の位置
		m_totalTime = m_tempTotalTime;
	}

	bool OpeningCameraman::ExcuteBehavior(float totaltime) {
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		m_totalTime += ElapsedTime;
		if (m_totalTime > totaltime) {
			return true;
		}
		Easing<Vec3> easing;
		m_eyePos = easing.EaseInOut(EasingType::Cubic, m_startPos, m_endPos, m_totalTime, totaltime);
		m_atPos = easing.EaseInOut(EasingType::Cubic, m_atStartPos, m_atEndPos, m_totalTime, totaltime);
		auto ptrTrans = GetComponent<Transform>();
		ptrTrans->SetPosition(m_eyePos);
		return false;
	}
	void OpeningCameraman::EndStateEnterBehavior() {
		GetTypeStage<GameStage>()->ToMainCamera();
	}

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanToGoalState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	shared_ptr<OpeningCameramanToFirstState> OpeningCameramanToFirstState::Instance() {
		static shared_ptr<OpeningCameramanToFirstState> instance(new OpeningCameramanToFirstState);
		return instance;
	}
	void OpeningCameramanToFirstState::Enter(const shared_ptr<OpeningCameraman>& Obj) {
		Obj->ToGoalEnterBehavior();
	}
	void OpeningCameramanToFirstState::Execute(const shared_ptr<OpeningCameraman>& Obj) {
		if (Obj->ExcuteBehavior(4.0f)) {
			Obj->GetStateMachine()->ChangeState(OpeningCameramanToSecondState::Instance());
		}
	}
	void OpeningCameramanToFirstState::Exit(const shared_ptr<OpeningCameraman>& Obj) {
	}

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanToStartState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	shared_ptr<OpeningCameramanToSecondState> OpeningCameramanToSecondState::Instance() {
		static shared_ptr<OpeningCameramanToSecondState> instance(new OpeningCameramanToSecondState);
		return instance;
	}
	void OpeningCameramanToSecondState::Enter(const shared_ptr<OpeningCameraman>& Obj) {
		Obj->ToStartEnterBehavior();
	}
	void OpeningCameramanToSecondState::Execute(const shared_ptr<OpeningCameraman>& Obj) {
		if (Obj->ExcuteBehavior(2.0f)) {
			Obj->GetStateMachine()->ChangeState(OpeningCameramanEndState::Instance());
		}
	}
	void OpeningCameramanToSecondState::Exit(const shared_ptr<OpeningCameraman>& Obj) {
	}

	//--------------------------------------------------------------------------------------
	//	class OpeningCameramanEndState : public ObjState<OpeningCameraman>;
	//--------------------------------------------------------------------------------------
	shared_ptr<OpeningCameramanEndState> OpeningCameramanEndState::Instance() {
		static shared_ptr<OpeningCameramanEndState> instance(new OpeningCameramanEndState);
		return instance;
	}
	void OpeningCameramanEndState::Enter(const shared_ptr<OpeningCameraman>& Obj) {
		Obj->EndStateEnterBehavior();
	}
	void OpeningCameramanEndState::Execute(const shared_ptr<OpeningCameraman>& Obj) {
	}
	void OpeningCameramanEndState::Exit(const shared_ptr<OpeningCameraman>& Obj) {
	}


}
