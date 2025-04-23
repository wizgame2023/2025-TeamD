/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	BossAppearText::BossAppearText(const shared_ptr<Stage>& stage, Vec3 position, Vec3 size) : 
		Object(stage,position,Vec3(),size),m_ExitTime(Timer(3.5f)){ }

	void BossAppearText::OnCreate() {
		Object::OnCreate();
		Vec3 scale = GetScale();
		m_Text = m_Stage->AddGameObject<Sprite>(L"BOSS_APPEAR", GetPosition(),Vec2(scale.x,scale.y));
		m_Text->AddComponent<SpriteFlash>(1.0f);
		m_Text->SetDiffuse(Col4(0, 0, 0, 1));
	}
	void BossAppearText::OnUpdate() {
		if (m_ExitTime.UpdateTimer()) {
			m_Stage->RemoveGameObject<Sprite>(m_Text);
		}
	}

}
//end basecross
