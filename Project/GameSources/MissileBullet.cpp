/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Ballet.h"

namespace basecross {
	MissileBullet::MissileBullet(const shared_ptr<Stage>& stage,Vec3 target,float speed, float explodeSize) :
		Object(stage),m_Target(target),m_ExplodeSize(explodeSize),m_IsTarget(true),m_Speed(speed){}

	void MissileBullet::OnCreate() {
		Object::OnCreate();

		m_AreaEffect = m_Stage->AddGameObject<AreaOfEffect>(m_Target, m_ExplodeSize, 36, 0);
	}

	void MissileBullet::OnUpdate() {
		if (m_IsTarget) {
			Vec3 position = GetPosition();
			Vec3 direction = position - m_Target;
			direction = direction.normalize();

			position += direction * m_Speed * GetElpased();

			SetPosition(position);
		}
		else {

		}
	}

	void MissileBullet::OnCollisionEnter(shared_ptr<GameObject>& Other) {
		auto explode = m_Stage->AddGameObject<CrushAttack>(Vec3(m_ExplodeSize), AttackDate(nullptr, 10.0f, 0.0f, 0.1f, 0.0f, 0.0f), 3.0f);
		explode->Play(GetPosition());
	}
}
//end basecross
