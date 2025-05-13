
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	flyobject::flyobject(const shared_ptr<Stage>& stage) : Object(stage), m_ZoneElapsedTime(1.0f) {}
	flyobject::~flyobject() {}

	Vec3 flyobject::GetForward()
	{
		return m_Transform->GetForward();
	}

	void flyobject::flyPositison(shared_ptr<GameObject>& other)
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto gravity = GetComponent<Gravity>();
		Vec3 objPos = other->GetComponent<Transform>()->GetPosition();
		Vec3 flyPos = GetComponent<Transform>()->GetPosition();
		Vec3 spherePos = other->GetComponent<Transform>()->GetForward();
		Vec3 pos = objPos + -flyPos;
		pos.normalize();
		//flyPos += -pos * 5.0f * elapsedTime * m_ZoneElapsedTime;
		//float rote = atan2f(pos.x, pos.z);
		//SetRotation(Vec3(0.0f, rote, 0.0f));
		//SetPosition(flyPos);

		gravity->StartJump(Vec3(-pos.x + spherePos.x / 2, 5.0f, -pos.z + spherePos.z / 2));
	}

	void flyobject::OnCreate() {
		Object::OnCreate();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		auto objectGravity = AddComponent<Gravity>();

		//ï`âÊê›íË
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		//ptrDraw->SetTextureResource(L"GROUND");
	}

	void flyobject::OnUpdate() {
		ZoneSpeedSet();
	}

	void flyobject::ZoneSpeedSet()
	{
		auto gravity = GetComponent<Gravity>();
		auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();
		if ((state & Player::PlayerState::ZONE) == 0) {
			m_ZoneElapsedTime = 1.0f;
			gravity->SetZoneGravityVerocity(m_ZoneElapsedTime);
		}
		else {
			m_ZoneElapsedTime = 0.2f;
			gravity->SetZoneGravityVerocity(m_ZoneElapsedTime);
		}
	}

	void flyobject::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			flyPositison(other);
		}
		if (other->FindTag(L"Enemy"))
		{
			auto enemy = dynamic_pointer_cast<Character>(other);
			enemy->Damage(4.0f, false);
			m_Stage->RemoveGameObject<flyobject>(GetThis<flyobject>());
		}
		if (other->FindTag(L"Boss"))
		{
			auto bossEnemy = dynamic_pointer_cast<BossEnemy>(other);
			bossEnemy->Damage(5.0f, false);
			m_Stage->RemoveGameObject<flyobject>(GetThis<flyobject>());
		}
	}

}