/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void HPBar::OnCreate() {
		m_Transform = GetComponent<Transform>();

		m_HpBar = GetStage()->AddGameObject<Board>(L"HP_BAR", Vec3(0, 0, 0), Vec3(1.0f, 0.09f, 1.0f), true);
	}
	void HPBar::OnUpdate() {
		m_Transform->SetPosition(m_Target->GetPosition() + m_Offset);
		float remainingHp = m_CurrentHp / m_MaxHp;
		auto barTransform = m_HpBar->GetComponent<Transform>();
		barTransform->SetScale(Vec3(remainingHp, 0.09f, 1.0f));

		Vec3 forward = barTransform->GetForword();
		forward.y = 0.0f;
		forward = forward.normalize();

		Vec3 crossVec = cross(forward, Vec3(0, 1, 0));
		crossVec = crossVec.normalize();
		barTransform->SetPosition(m_Transform->GetPosition() + crossVec * 0.5f * (1.0f - remainingHp));
	}
	void HPBar::Destroy() {
		GetStage()->RemoveGameObject<Board>(m_HpBar);
		GetStage()->RemoveGameObject<HPBar>(GetThis<HPBar>());
	}
}
//end basecross
