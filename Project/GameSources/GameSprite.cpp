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
	NextWaveText::NextWaveText(const shared_ptr<Stage>& stage, Vec3 center, int currect, int max) :
		Object(stage,Vec3(0,0,0),Vec3(),Vec3(1)),m_CurrentWave(currect),m_MaxWave(max) {

	}
	void NextWaveText::OnCreate() {
		Object::OnCreate();
		m_Text = m_Stage->AddGameObject<Sprite>(L"NEXT_WAVE",Vec3(),Vec2(500,100));
		m_Text->GetComponent<Transform>()->SetParent(GetThis<NextWaveText>());
		m_BossText = m_Stage->AddGameObject<Sprite>(L"BOSS_APPEAR", Vec3(50,-100,0), Vec2(400, 80));
		m_BossText->GetComponent<Transform>()->SetParent(GetThis<NextWaveText>());
		m_BossText->SetDiffuse(Col4(0, 0, 0, 1));

		m_CurrentWaveSprite = m_Stage->AddGameObject<NumberSprite>(L"NUMBER", Vec3(275, 0, 0), Vec2(35, 70), 1);
		m_CurrentWaveSprite->UpdateNumber(m_CurrentWave);
		m_CurrentWaveSprite->GetComponent<Transform>()->SetParent(GetThis<NextWaveText>());
		m_CurrentWaveSprite->SetDiffuse(Col4(0, 0, 0, 1));
		m_MaxWaveSprite = m_Stage->AddGameObject<NumberSprite>(L"NUMBER", Vec3(375,0,0), Vec2(35, 70), 1);
		m_MaxWaveSprite->UpdateNumber(m_MaxWave);
		m_MaxWaveSprite->GetComponent<Transform>()->SetParent(GetThis<NextWaveText>());
		m_MaxWaveSprite->SetDiffuse(Col4(0, 0, 0, 1));

		SetPosition(Vec3(1000, 250, 0));
	}
	void NextWaveText::OnUpdate() {
		Vec3 position = GetPosition();
		float diff = abs(position.x - m_CenterPosition.x);
		float speed = diff * 4.5f;
		speed = max(speed, 320.0f);
		position.x -= speed * GetElpased();
		SetPosition(position);
		if (m_CurrentWave != m_MaxWave) {
			m_BossText->SetDrawActive(false);
		}
		if (position.x < -1000.0f) {
			m_Stage->RemoveGameObject<Sprite>(m_Text);
			m_Stage->RemoveGameObject<Sprite>(m_BossText);
			m_CurrentWaveSprite->Destroy();
			m_MaxWaveSprite->Destroy();
			m_Stage->RemoveGameObject<NextWaveText>(GetThis<NextWaveText>());
		}
	}
}
//end basecross
