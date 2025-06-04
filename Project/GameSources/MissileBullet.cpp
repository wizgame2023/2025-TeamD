/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Ballet.h"

namespace basecross {
	MissileBullet::MissileBullet(const shared_ptr<Stage>& stage,Vec3 position,Vec3 direction, shared_ptr<Transform>& target,float speed, float explodeSize) :
		Object(stage,position,Vec3(),Vec3(0.2f)),
		m_TargetPosition(Vec3(0)), m_Target(target), m_ExplodeSize(explodeSize), m_IsTarget(false), m_Speed(speed),
		m_LaunchPosition(position),m_Direction(direction){}

	void MissileBullet::OnCreate() {
		Object::OnCreate();
		m_Direction = m_Direction.normalize();
		float launchTime = (1.0f / m_Speed);
		Vec3 beginTargetPosition = GetPosition() + (m_Direction * m_Speed) * launchTime;
		
		m_TargetPosition = m_Target->GetPosition();
		
		float targetTime = (m_TargetPosition - beginTargetPosition).length() / m_Speed;
		//float time = (1.0f / m_Speed) + ()
		m_AreaEffect = m_Stage->AddGameObject<AreaOfEffect>(m_TargetPosition, m_ExplodeSize / 2.0f, 36, launchTime + targetTime);

		auto draw = AddComponent<PNTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_CUBE");
		draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));
		auto col = AddComponent<CollisionObb>();
		col->AddExcludeCollisionTag(L"Attack");
		auto& effect = m_Stage->GetCreateEffect();
		effect->PlayEffect(m_EffectHandle,L"boost", GetPosition(), 0.0f);
		effect->SetScale(m_EffectHandle,Vec3(0.2f));
	}

	void MissileBullet::OnUpdate() {

		

		Vec3 position = GetPosition();
		Vec3 move = Vec3();
		move = m_Direction * m_Speed * GetGameElapsed();

		position += move;
		if (!m_IsTarget && (m_LaunchPosition - position).length() > 1.0f) {
			m_IsTarget = true;
			m_Direction = m_TargetPosition - position;
			m_Direction = m_Direction.normalize();
		}
		if ((m_TargetPosition - position).length() < 0.1f) {
			m_Direction = Vec3(0, -1, 0);
		}
		if (GameManager::Instance()->GetDifficulty() == Difficulty::Hard && (m_TargetPosition - position).length() > 8.0f) {
			m_TargetPosition = m_Target->GetPosition();
			m_AreaEffect->SetPosition(m_TargetPosition);

			m_Direction = m_TargetPosition - position;
			m_Direction = m_Direction.normalize();
		}
		SetPosition(position);

		
		auto& effect = m_Stage->GetCreateEffect();
		Vec3 c = cross(Vec3(0, 0, 1),m_Direction);
		float angle = acosf(dot(m_Direction,Vec3(0, 0, 1)) / (m_Direction.length() * Vec3(0, 0, 1).length()));
		effect->SetRotation(m_EffectHandle,c, angle);
		effect->SetLocation(m_EffectHandle, position);
		effect->SetEffectSpeed(m_EffectHandle,GameManager::Instance()->GetTimeRate());
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
