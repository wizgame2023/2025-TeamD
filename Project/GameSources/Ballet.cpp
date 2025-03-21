/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	Bullet::Bullet(const shared_ptr<Stage>& stage, Vec3 position, float speed, Vec3 direction, float range) :
		GameObject(stage), m_Position(position), m_Speed(speed), m_Direction(direction), m_EffectiveRange(range),
		m_ZoneElapsedTime(1.0f), m_EndPosition(Vec3(0)), m_LineEndPosition(Vec3())
	{
	}
	Bullet::~Bullet() {}

	void Bullet::OnCreate()
	{
		//初期位置の設定
		m_Transform = AddComponent<Transform>();
		m_Transform->SetPosition(m_Position);
		m_Transform->SetRotation(Vec3(0));
		m_Transform->SetScale(Vec3(0.1f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		AddTag(L"Bullet");

		auto& group = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		group->IntoGroup(GetThis<Bullet>());
	}

	void Bullet::OnUpdate() {
		float elapsed = App::GetApp()->GetElapsedTime();
		Vec3 position = m_Transform->GetPosition();
		Vec3 moveAmount = Vec3();

		ZoneSpeedSet();
		moveAmount += m_Speed * m_Direction * elapsed * m_ZoneElapsedTime;

		if ((m_Position - position).length() > m_EffectiveRange) {
			GetStage()->RemoveGameObject<Bullet>(GetThis<Bullet>());
		}
		else {
			position += moveAmount;
			m_Transform->SetPosition(position);
		}
	}

	void Bullet::OnCollisionEnter(shared_ptr<GameObject>& other) {
		if (other->FindTag(L"HitJudge") || other->FindTag(L"Player") || other->FindTag(L"Object")) {
			GetStage()->RemoveGameObject<Bullet>(GetThis<Bullet>());
		}
	}
	void Bullet::ZoneSpeedSet()
	{
		auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();
		if ((state & Player::PlayerState::ZONE) == 0) {
			m_ZoneElapsedTime = 1.0f;
		}
		else {
			m_ZoneElapsedTime = 0.2f;
		}
	}
}
//end basecross
