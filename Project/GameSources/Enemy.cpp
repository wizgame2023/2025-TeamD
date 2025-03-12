/*!
@file Enemy.cpp
@brief 敵など実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Enemy.h"

namespace basecross {
	Enemy::Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
		Character(stage),
		m_Position(position),
		m_Rotation(rotation),
		m_Scale(scale)
	{
	}
	Enemy::~Enemy()
	{
	}
	void Enemy::OnCreate()
	{
		m_HP = 3;
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(m_Position);
		ptr->SetRotation(m_Rotation);
		ptr->SetScale(m_Scale);

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
		if (m_HP <= 0)
		{
			SetDrawActive(false);
			SetUpdateActive(false);
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
