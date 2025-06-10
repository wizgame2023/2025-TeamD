/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void LineCube::OnCreate() {
		m_Transform = GetComponent<Transform>();

		m_Draw = AddComponent<BcPNTStaticDraw>();
		m_Draw->SetMeshResource(L"DEFAULT_CUBE");
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 0.1f));
		SetAlphaActive(true);

		AddTag(L"Line");
	}
	void LineCube::OnUpdate() {
		if (m_Line.GetLength() == 0) {
			SetDrawActive(false);
			return;
		}
		SetDrawActive(true);
		float rad = atan2f(-m_Line.GetDirection().z, m_Line.GetDirection().x);
		//m_Transform->SetRotation(Vec3(0, rad, 0));

		m_Transform->SetScale(Vec3(m_Line.GetLength(), m_LineSize, m_LineSize));

		m_Transform->SetPosition(m_Line.m_Start + m_Line.GetDirection() / 2.0f);

		auto rotateMatrix = (Mat4x4)XMMatrixLookAtLH(m_Line.m_Start, m_Line.m_End, Vec3(0, 1, 0));
		rotateMatrix = inverse(rotateMatrix);
		Quat qt = rotateMatrix.quatInMatrix();
		qt = Quat(0, sin(XMConvertToRadians(45)), 0, cos(XMConvertToRadians(45))) * qt;
		m_Transform->SetQuaternion(qt);
	}
	void LineCube::Destroy() {
		GetStage()->RemoveGameObject<LineCube>(GetThis<LineCube>());
	}
	void BulletLine::OnCreate() {
		m_Transform = GetComponent<Transform>();
		m_Draw = AddComponent<BcPNTStaticDraw>();
		m_Draw->SetMeshResource(L"DEFAULT_CONE");
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDiffuse(m_Color);
		SetAlphaActive(true);

		AddTag(L"Line");
	}
	void BulletLine::OnUpdate() {
		LineCube::OnUpdate();
	}
}
//end basecross
