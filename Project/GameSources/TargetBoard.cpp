/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void TargetBoard::OnCreate() {
		m_Board = GetStage()->AddGameObject<Board>(L"TARGET",Vec3(),Vec3(0.5f));
	}
	void TargetBoard::OnUpdate() {
		if (m_Target == nullptr) {
			SetDrawActive(false);
			return;
		}
		else {
			SetDrawActive(true);
		}

		Vec3 targetPosition = m_Target->GetComponent<Transform>()->GetPosition();
		Vec3 playerPosition = m_Player->GetComponent<Transform>()->GetPosition();

		Vec3 direction = playerPosition - targetPosition;

		m_Target->GetComponent<Transform>()->SetPosition(targetPosition + direction * 0.1f);
	}
}
//end basecross
