/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void TargetBoard::OnCreate() {
		m_Board = GetStage()->AddGameObject<Board>(L"TARGET", Vec3(), Vec3(1.0f));
	}
	void TargetBoard::OnUpdate() {
		if (m_Target == nullptr || !m_Target->GetDrawActive()) {
			m_Board->SetDrawActive(false);
			return;
		}
		else {
			m_Board->SetDrawActive(true);
		}
		Vec3 targetSize = m_Target->GetComponent<Transform>()->GetScale();
		Vec3 targetPosition = m_Target->GetComponent<Transform>()->GetPosition() + Vec3(0, 0.15f, 0);
		Vec3 eye = OnGetDrawCamera()->GetEye();
		Vec3 direction = eye - targetPosition;
		m_Board->GetComponent<Transform>()->SetPosition(targetPosition + direction * 0.1f);
		m_Board->GetComponent<Transform>()->SetScale(targetSize);
	}
}
//end basecross
