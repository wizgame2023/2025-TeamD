/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void Attack::OnCreate() {
		Object::OnCreate();
		SetDrawActive(false);
		LoopEffect();
	}
	void Attack::OnUpdate() {
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
		auto character = static_pointer_cast<Player>(player);
		if (character) {
			character->Damage(true,m_Date.m_Damage);
			Stop();
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
	
	void Missile::OnUpdate() {
		Attack::OnUpdate();
		if (!GetDrawActive()) return;

		if (m_MissileCount > 0 && m_MissileTimer.UpdateTimer()) {
			SoundManager::Instance().PlaySE(L"SE_MISSILE");
			Vec3 position = GetPosition();
			position += m_MuzzlePositions[m_MuzzleIndex];
			m_Stage->AddGameObject<MissileBullet>(position, Vec3(0.0f, 1.0f, 0.0f), m_Target->GetPosition(), 10.0f, 2.0f);
			m_MissileCount--;
			m_MuzzleIndex++;
			if (m_MuzzlePositions.size() <= m_MuzzleIndex) {
				m_MuzzleIndex = 0;
			}
			m_MissileTimer.Reset();
		}
	}
	void Missile::Play(Vec3 position) {
		Attack::Play(position);

		m_MissileCount = m_MissileMaxCount;
		//m_MissileTimer.Reset();
	}
}
//end basecross
