/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void Attack::OnCreate() {
		Object::OnCreate();
		SetUpdateActive(false);
		SetDrawActive(false);
		LoopEffect();
	}
	void Attack::OnUpdate() {
		float elapsed = App::GetApp()->GetElapsedTime();
		m_IsFinish = false;
		if (GetDrawActive()) {
			if (m_Date.m_ExitTimer.UpdateTimer()) {
				Stop();
			}
		}
		else {
			m_Date.m_CooldownTimer.UpdateTimer();
		}
	}
	//void AttackCollisionObb<T>::OnCreate() {
	//	Attack::OnCreate();
	//	m_Collision = AddComponent<CollisionObb>();
	//	m_Collision->SetDrawActive(true);
	//	m_Collision->SetAfterCollision(AfterCollision::None);
	//	m_Transform->SetScale(m_Size);
	//	AddTag(L"BossAttack");
	//}
	//void AttackCollisionCircle::OnCreate() {
	//	Attack::OnCreate();
	//	m_Collision = AddComponent<CollisionSphere>();
	//	m_Collision->SetDrawActive(true);
	//	m_Collision->SetAfterCollision(AfterCollision::None);
	//	m_Transform->SetScale(Vec3(m_Size));
	//	AddTag(L"BossAttack");
	//}

	void CrushAttack::ContactPlayer(shared_ptr<GameObject>& player) {
		Vec3 position = m_Transform->GetPosition();
		Vec3 playerPosition = player->GetComponent<Transform>()->GetPosition();

		Vec3 direction = playerPosition - position;
		direction = direction.normalize();
		direction.y = 0.5f;

		auto gravity = player->GetComponent<Gravity>(false);
		if (gravity != nullptr) {
			gravity->StartJump(direction * m_BlowForce);
		}
		auto character = static_pointer_cast<Character>(player);
		if (character) {
			character->Damage(m_Date.m_Damage);
		}
	}
	void CrushAttack::OnCollisionEnter(shared_ptr<GameObject>& Other) {
		Attack::OnCollisionEnter(Other);
		if (Other->FindTag(L"HitJudge")) {
			Other->OnCollisionEnter(GetThis<GameObject>());
			if (m_Date.m_ExitTimer.GetTime() < m_Date.m_ExitTimer.GetMaxTime() / 2.0f) {
				if (m_Date.m_Owner != nullptr) {
					auto boss = static_pointer_cast<BossEnemy>(m_Date.m_Owner);
					boss->AddStun(0.5f);
					Vec3 direction = GetPosition() - Other->GetComponent<Transform>()->GetPosition();
					direction = direction.normalize();
					boss->GetComponent<Gravity>()->StartJump(direction + Vec3(0.0f, 2.0f, 0.0f));
					Stop();
				}
			}
		}
		if (Other->FindTag(L"Player"))
		{
			auto player = dynamic_pointer_cast<Player>(Other);
			player->Damage(false, 4.0f);
			GetStage()->RemoveGameObject<CrushAttack>(GetThis<CrushAttack>());
		}
	}
	void MachineGun::OnUpdate() {
		Attack::OnUpdate();
		if (!GetDrawActive()) return;
		float elapsed = App::GetApp()->GetElapsedTime();
		if (m_ShotInterval < 0) {
			m_ShotInterval = m_Date.m_ExitTimer.GetMaxTime() / m_LaunchNum;
			Vec3 position = m_Transform->GetPosition();
			Vec3 direction = m_Target->GetComponent<Transform>()->GetPosition() - position;
			m_Stage->AddGameObject<Bullet>(position + direction * 0.1f, 2.0f, direction, m_Date.m_Range);
		}
		else {
			m_ShotInterval -= elapsed;
		}
	}
	void Missile::ContactStage(shared_ptr<GameObject>& object) {
		Vec3 scale = m_Transform->GetScale();
		m_Transform->SetScale(scale * 2.0f);

		m_Date.m_ExitTimer.SetTime(0.5f, true);
	}
	void Missile::OnUpdate() {
		Attack::OnUpdate();

	}
}
//end basecross
