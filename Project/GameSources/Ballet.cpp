/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Ballet.h"

namespace basecross {

	Bullet::Bullet(const shared_ptr<Stage>& stage, Vec3 position, float speed, Vec3 direction, float range) :
		Object(stage,position,Vec3(0),Vec3(0.1f)), m_Position(position), m_Speed(speed), m_Direction(direction), m_EffectiveRange(range),
		m_ZoneElapsedTime(1.0f), m_EndPosition(Vec3(0)), m_LineEndPosition(Vec3()), m_LineLength(5.0f), 
		m_bulletPally(false)
	{
	}
	Bullet::‾Bullet() {}

	void Bullet::OnCreate()
	{
		Object::OnCreate();
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetAfterCollision(AfterCollision::None);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticModelDraw>();
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.5f), //(.1f, .1f, .1f),
			Vec3(0, 0.0f, 0),
			Vec3(0, 0, 0),
			Vec3(0.0f, -0.5f, 0.0f)
		);
		ptrDraw->SetMeshResource(L"BULLET");
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::AlphaBlend);
		ptrDraw->SetOwnShadowActive(true);

		AddTag(L"Bullet");

		auto& group = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		group->IntoGroup(GetThis<Bullet>());

		m_Line = GetStage()->AddGameObject<BulletLine>(0.02f, Col4(1, 1, 0, 1));
	}

	void Bullet::OnUpdate() {
		float elapsed = GetGameElapsed();
		Vec3 position = m_Transform->GetPosition();
		Vec3 moveAmount = Vec3();
		Vec3 rot = GetForward();
		float rotate = atan2f(-m_Direction.x, -m_Direction.z);
		m_Transform->SetRotation(Vec3(0, rotate, 0));
		ZoneSpeedSet();
		moveAmount += m_Speed * m_Direction * elapsed * m_ZoneElapsedTime;

		if ((m_Position - position).length() > m_EffectiveRange) {
			GetStage()->RemoveGameObject<LineCube>(m_Line);
			Delete();
		}
		else {
			position += moveAmount;
			m_Transform->SetPosition(position);
			if ((m_Position - position).length() < m_LineLength) {
				m_Line->SetLine(Line(position, m_Position));
			}
			else {
				Vec3 direction = m_Position - position;
				direction = direction.normalize();
				m_Line->SetLine(Line(position,position + direction * m_LineLength));
			}
		}
	}

	void Bullet::OnCollisionEnter(shared_ptr<GameObject>& other) {
		Difficulty difficulty = GameManager::Instance()->GetDifficulty();

		if (other->FindTag(L"Player"))
		{
			auto player = dynamic_pointer_cast<Player>(other);
			m_bulletPally = player->Damage(false, 2.0f);
			if (m_bulletPally)
			{
				player->SetParryPosition(GetPosition());
				m_Direction = -m_Direction;
			}
			else {
				GetStage()->RemoveGameObject<LineCube>(m_Line);
				Delete();
			}
		}
		else if (other->FindTag(L"Citizen"))
		{
			GetStage()->RemoveGameObject<LineCube>(m_Line);
			auto citizen = dynamic_pointer_cast<Character>(other);
			citizen->Damage(1.0f, false);
			Delete();
		}
		else if (other->FindTag(L"Enemy")) 
		{
			GetStage()->RemoveGameObject<LineCube>(m_Line);
			auto enemy = dynamic_pointer_cast<Enemy>(other);
			enemy->Damage(1.0f + ((float)difficulty * 1.5f),  false);
			Delete();
		}
		else if (other->FindTag(L"Object"))
		{
			GetStage()->RemoveGameObject<LineCube>(m_Line);
			Delete();
		}
	}

	void Bullet::Delete()
	{
		auto group = m_Stage->GetSharedObjectGroup(L"BulletGroup");
		auto& groupVec = group->GetGroupVectors();
		for (int i = 0; i < groupVec.size(); i++) {
			auto obj = groupVec[i].lock();
			if (obj != nullptr) {
				if (obj == GetThis<GameObject>()) {
					groupVec.erase(groupVec.begin() + i);
					break;
				}
			}
		}
		m_Stage->RemoveGameObject<Bullet>(GetThis<Bullet>());

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
