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
		position.x -= speed * GetGameElapsed();
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


	HpSprite::HpSprite(const shared_ptr<Stage>& stage, shared_ptr<Character>& owner, Vec3 position, Vec3 size,Col4 color) :
		Object(stage, position, Vec3(), size),m_Owner(owner), m_Color(color){

	}
	void HpSprite::OnCreate() {
		Object::OnCreate();
		Vec3 position = GetPosition();
		Vec3 size = GetScale();

		Vec3 leftEdgePosition = position - Vec3(0.0f, size.y / 2.0f, 0);
		Vec3 rightEdgePosition = position + Vec3(size.x, -size.y / 2.0f, 0);

		m_HpBar = m_Stage->AddGameObject<Sprite>(L"HP_BAR2D", GetPosition(), Vec2(size.x, size.y));
		m_HpBar->SetDiffuse(m_Color);
		m_HpBar->SetDrawLayer(1);
		m_HpBarBackGround = m_Stage->AddGameObject<Sprite>(L"HP_BAR2D", GetPosition(), Vec2(size.x, size.y));
		m_HpBarBackGround->SetDiffuse(Col4(0, 0, 0, 1));
		m_HpBarBackGround->SetDrawLayer(0);

		auto edge = m_Stage->AddGameObject<Sprite>(L"HP_BAR_EDGE", leftEdgePosition, Vec2(size.y / 2.0f, size.y),true);
		edge->SetDiffuse(m_Color);
		edge->SetDrawLayer(1);
		m_Edge.push_back(edge);

		edge = m_Stage->AddGameObject<Sprite>(L"HP_BAR_EDGE", rightEdgePosition, Vec2(size.y / 2.0f, size.y),true);
		edge->SetDiffuse(m_Color);
		edge->SetDrawLayer(1);
		m_Edge.push_back(edge);

		edge = m_Stage->AddGameObject<Sprite>(L"HP_BAR_EDGE", leftEdgePosition, Vec2(size.y / 2.0f, size.y), true);
		edge->SetDiffuse(Col4(0, 0, 0, 1));
		edge->SetDrawLayer(0);
		m_Edge.push_back(edge);

		edge = m_Stage->AddGameObject<Sprite>(L"HP_BAR_EDGE", rightEdgePosition, Vec2(size.y / 2.0f, size.y), true);
		edge->SetDiffuse(Col4(0, 0, 0, 1));
		edge->SetDrawLayer(0);
		m_Edge.push_back(edge);
	}
	void HpSprite::OnUpdate() {
		if (m_Owner == nullptr) return;
		float currentHp = m_Owner->GetHP();
		float maxHp = m_Owner->GetMaxHP();

		float rate = currentHp / maxHp;
		rate = max(0.0f, rate);
		rate = min(1.0f, rate);

		Vec3 size = GetScale();
		Vec3 leftPosition = GetPosition();

		size.x *= rate;

		m_HpBar->UpdateSize(Vec2(size.x, size.y));

		Vec3 rightEdgePosition = leftPosition + Vec3(size.x, 0, 0);
		m_Edge[0]->SetPos(leftPosition - Vec3(0,size.y / 2.0f,0));
		m_Edge[1]->SetPos(rightEdgePosition - Vec3(0, size.y / 2.0f, 0));


		bool isActive = GetDrawActive();
		if (currentHp <= 0 || !m_Owner->GetDrawActive()) {
			isActive = false;
		}
		m_HpBar->SetDrawActive(isActive);
		m_HpBarBackGround->SetDrawActive(isActive);

		for (auto& edge : m_Edge) {
			edge->SetDrawActive(isActive);
		}
		for (auto& item : m_Items) {
			item->SetDrawActive(isActive);
		}
	}

	shared_ptr<Sprite> HpSprite::AddSprite(const wstring& key, Vec3 offset, Vec2 size) {
		auto sprite = m_Stage->AddGameObject<Sprite>(key, GetPosition() + offset, size);
		m_Items.push_back(sprite);

		return sprite;
	}
}
//end basecross
