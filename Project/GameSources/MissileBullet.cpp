/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Ballet.h"

namespace basecross {
	MissileBullet::MissileBullet(const shared_ptr<Stage>& stage,Vec3 position,Vec3 direction,Vec3 target,float speed, float explodeSize) :
		Object(stage,position,Vec3(),Vec3(0.2f)),
		m_Target(target),m_ExplodeSize(explodeSize),m_IsTarget(false),m_Speed(speed),
		m_LaunchPosition(position),m_Direction(direction){}

	void MissileBullet::OnCreate() {
		Object::OnCreate();
		m_Direction = m_Direction.normalize();
		float launchTime = (1.0f / m_Speed);
		Vec3 beginTargetPosition = GetPosition() + (m_Direction * m_Speed) * launchTime;
		float targetTime = (m_Target - beginTargetPosition).length() / m_Speed;
		//float time = (1.0f / m_Speed) + ()
		m_AreaEffect = m_Stage->AddGameObject<AreaOfEffect>(m_Target, m_ExplodeSize / 2.0f, 36, launchTime + targetTime);

		auto draw = AddComponent<PNTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_CUBE");
		draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));
		auto col = AddComponent<CollisionObb>();
		
		auto& effect = m_Stage->GetCreateEffect();
		effect->PlayEffect(m_EffectHandle,L"boost", GetPosition(), 0.0f);
		effect->SetScale(m_EffectHandle,Vec3(0.2f));
	}

	void MissileBullet::OnUpdate() {
		Vec3 position = GetPosition();
		Vec3 move = Vec3();
		move = m_Direction * m_Speed * GetElpased();

		position += move;
		if (!m_IsTarget && (m_LaunchPosition - position).length() > 1.0f) {
			m_IsTarget = true;
			m_Direction = m_Target - position;
			m_Direction = m_Direction.normalize();
		}
		if ((m_Target - position).length() < 0.1f) {
			m_Direction = Vec3(0, -1, 0);
		}
		SetPosition(position);

		
		auto& effect = m_Stage->GetCreateEffect();
		Vec3 c = cross(Vec3(0, 0, 1),m_Direction);
		float angle = acosf(dot(m_Direction,Vec3(0, 0, 1)) / (m_Direction.length() * Vec3(0, 0, 1).length()));
		effect->SetRotation(m_EffectHandle,c, angle);
		effect->SetLocation(m_EffectHandle, position);
	}
	void MissileBullet::OnCollisionEnter(shared_ptr<GameObject>& Other) {
		SoundManager::Instance().PlaySE(L"SE_EXPLODE");
		auto explode = m_Stage->AddGameObject<CrushAttack>(Vec3(m_ExplodeSize), AttackDate(nullptr, 2.5f, 0.0f, 0.1f, 0.0f, 0.0f), 3.0f);
		explode->Play(GetPosition());
		auto& effect = m_Stage->GetCreateEffect();
		effect->StopEffect(m_EffectHandle);
		effect->PlayEffect(m_EffectHandle,L"MissileFlash", GetPosition() , 0.0f);
		effect->SetScale(m_EffectHandle,Vec3(0.25f));
		effect->SetRotation(m_EffectHandle,Vec3(1.0f, 0.0f, 0.0f),XMConvertToRadians(90.0f));
		m_Stage->RemoveGameObject<MissileBullet>(GetThis<MissileBullet>());
	}
}
