/*!
@file Enemy.cpp
@brief 敵など実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Enemy::Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Character(stage),
		m_Position(position),
		m_Rotation(Vec3()),
		m_Scale(scale){}

	Enemy::~Enemy()
	{
	}
	void Enemy::OnCreate()
	{
		m_HP = 3;
		//初期位置の設定
		m_Transform = AddComponent<Transform>();
		m_Transform->SetPosition(m_Position);
		m_Transform->SetRotation(m_Rotation);
		m_Transform->SetScale(m_Scale);

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//重力をつける
		auto ptrGra = AddComponent<Gravity>();


		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");
	}
	void Enemy::OnUpdate()
	{
		ZoneSpeedSet();
		if (m_HP <= 0)
		{
			GetStage()->RemoveGameObject<Enemy>(GetThis<Enemy>());
		}
	}
	Vec3 Enemy::GetDirectionToIntruder() {
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = m_Intruder->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		offset = offset.normalize();
		return offset;
	}

	void Enemy::ZoneSpeedSet()
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

	void Enemy::OnDraw()
	{
		Character::OnDraw();
	}
	void Enemy::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			m_HP -= 1;
		}
	}
}
//end basecross
