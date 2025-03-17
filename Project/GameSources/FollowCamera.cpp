/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	FollowCamera::FollowCamera() :
		Camera(),
		m_Direction(Vec3(0)),
		m_Eye(Vec3(0)),
		m_Position(Vec3(0)),
		m_Angle(0),
		m_RotateSpeed(XMConvertToRadians(180))
	{
	}

	void FollowCamera::OnUpdate() {
		//コントローラの取得
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		float elapsed = App::GetApp()->GetElapsedTime();
		if (cntlVec.bConnected) {
			float stickX = cntlVec.fThumbRX;
			m_Angle -= m_RotateSpeed * elapsed * stickX;
		}


		//方向
		m_Direction = Vec3(cos(m_Angle),0.0f,sin(m_Angle));
		//位置
		m_Position = m_PlayerTransform->GetPosition();
		m_Eye = m_Position + m_Direction * 5.0f;
		m_Eye.y = m_Position.y + 2.0f;
		//自分の位置
		SetEye(m_Eye);
		//見ているところ
		SetAt(m_PlayerTransform->GetPosition());
		LogCamera();

	}

	void FollowCamera::LogCamera() {

		auto scene = App::GetApp()->GetScene<Scene>();

		wstringstream Debug(L"\n");
		Debug << L"\nForward.x : " << m_Direction.x
			<< L"\nForward.y : " << m_Direction.y
			<< L"\nForward.z : " << m_Direction.z << endl;

		scene->SetDebugString(Debug.str());

	}

}