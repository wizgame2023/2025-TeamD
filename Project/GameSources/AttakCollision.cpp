/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void AttackCollision::OnCreate() {
		m_Collision = AddComponent<CollisionObb>();
		m_Collision->SetDrawActive(true);
		m_Collision->SetAfterCollision(AfterCollision::None);

		m_BoneTransform = GetComponent<Transform>();
		m_BoneTransform->SetPosition(m_StartPosition);
		m_BoneTransform->SetScale(m_Size);

		LoopEffect();
	}
	void AttackCollision::OnUpdate() {
		float elapsed = App::GetApp()->GetElapsedTime();
		m_ExistenceTime -= elapsed;
		if (m_ExistenceTime <= 0) {
			m_ExistenceTime = 0;

			GetStage()->RemoveGameObject<AttackCollision>(GetThis<AttackCollision>());
		}
	}

	void CrushAttack::ContactPlayer(shared_ptr<GameObject>& player) {
		Vec3 position = m_BoneTransform->GetPosition();
		Vec3 playerPosition = player->GetComponent<Transform>()->GetPosition();

		Vec3 direction = playerPosition - position;
		direction = direction.normalize();
		direction.y = 0.5f;

		auto gravity = player->GetComponent<Gravity>(false);
		if (gravity != nullptr) {
			gravity->StartJump(direction * m_BlowForce);
		}
	}

	void Missile::ContactStage(shared_ptr<GameObject>& object) {
		Vec3 scale = m_BoneTransform->GetScale();
		m_BoneTransform->SetScale(scale * 2.0f);

		m_ExistenceTime = 0.5f;
	}
	void Missile::OnUpdate() {
		AttackCollision::OnUpdate();

	}
}
//end basecross
