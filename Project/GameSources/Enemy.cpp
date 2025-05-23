/*!
@file Enemy.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"
#include "Enemy.h"

namespace basecross {
	Enemy::Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Character(stage, position, Vec3(), scale){
	}
	Enemy::~Enemy()
	{
	}
	void Enemy::OnCreate()
	{
		Character::OnCreate();
		InitHP(3);
		m_AlertTime = 5.0f;
		m_KnockBack = false;
		m_KnockBackTime = 0.5f;
		//CollisionSphereの設定
		auto ptrColl = AddComponent<CollisionCapsule>();
		//ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);


		auto& group = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		group->IntoGroup(GetThis<Enemy>());
		AddTag(L"Enemy");

	}

	void Enemy::OnUpdate()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();

		if (m_KnockBack)
		{
			m_KnockBackTime -= elapsedTime;
			if (m_KnockBackTime > 0.0f)
			{
				KnockBackTime();
			}
			else 
			{
				m_KnockBack = false;
				m_KnockBackTime = 0.5f;
			}
		}
		if (m_HP <= 0) {
			Dead();
		}
	}
	void Enemy::AsyncUpdate() {
		SearchRange();
	}

	Vec3 Enemy::GetDirectionToIntruder() {
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = m_Intruder->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		offset = offset.normalize();
		return offset;
	}
	float Enemy::GetDistanceToIntruder() {
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = m_Intruder->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		return offset.length();
	}

	Vec3 Enemy::GetDirectionToIntruderObject(shared_ptr<Object> obj)
	{
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = obj->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		offset = offset.normalize();
		return offset;
	}

	float Enemy::GetDistanceToIntruderObject(shared_ptr<Object> obj)
	{
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = obj->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		return offset.length();
	}


	void Enemy::SearchRange()
	{
		float searchDistance = 10.0f;
		Vec3 target = m_Intruder->GetComponent<Transform>()->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 forword = m_Transform->GetForword();
		Vec3 position = m_Transform->GetPosition();
		forword.normalize();
		if ((position - target).length() < searchDistance)
		{
			if (IsWithinDetectionRange(forword, GetDirectionToIntruder(), 45.0)) {
				//プレイヤーの方向をゆっくり向く
				m_IntruderAlert = true;
			}
			else {
				m_IntruderAlert = false;
			}
		}
		else {
			m_IntruderAlert = false;
		}

		if (m_IntruderAlert && GetDistanceToIntruder() < searchDistance) {
			RayCastHit hit;
			RayCast::HitTestVec(hit, Line(GetPosition(), GetDirectionToIntruder(), 10.0f), m_Stage->GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy" });
			if (hit.m_Object && !hit.m_Object->FindTag(L"Player")) {
				m_IntruderAlert = false;
			}
		}
	}

	void Enemy::IntervalEnemy(const Vec3& target)
	{
		Vec3 crrentPosition = GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		crrentPosition += target * elapsedTime * m_ZoneElapsedTime;
		SetPosition(crrentPosition);
	}

	Vec3 Enemy::GetPosition()
	{
		return m_Transform->GetPosition();
	}

	bool Enemy::GetIntruderAlert()
	{
		return m_IntruderAlert;
	}
	void Enemy::SetIntruderAlert(bool flag)
	{
		m_IntruderAlert = flag;
	}

	void Enemy::KnockBack()
	{
		m_KnockBack = true;
	}

	void Enemy::KnockBackTime()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 hitPos = m_Intruder->GetComponent<Transform>()->GetPosition();
		Vec3 pos = GetPosition();
		Vec3 vec = hitPos - pos;
		vec.normalize();
		pos += -vec * 5.0f * elapsedTime * m_ZoneElapsedTime;
		float rotate = atan2f(vec.x, vec.z);
		SetRotation(Vec3(0.0f, rotate, 0.0f));
		SetPosition(pos);
	}

	void Enemy::Dead() {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		m_KnockBackTime -= elapsedTime;
		if (m_KnockBackTime > 0.0f)
		{
			KnockBackTime();
		}
		else
		{
			ScoreManager::Instance()->AddEliminateEnemyCount();
			auto group = m_Stage->GetSharedObjectGroup(L"EnemyGroup");
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
			auto spawner = m_Stage->GetSharedGameObject<Spawner>(L"Spawner", false);
			if (spawner) {
				PostEvent(0.0f, GetThis<ObjectInterface>(), spawner, L"EnemyDead");
			}
			m_Stage->RemoveGameObject<Enemy>(GetThis<Enemy>());
		}

	}
	void Enemy::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			//Damage(m_Intruder->GetAttackDamage(), false);
			KnockBack();
			SoundManager::Instance().PlaySE(L"SE_HIT_ENEMY");
		}
		if (other->FindTag(L"Bullet"))
		{
			KnockBack();
		}
	}

	//--------------------------------------------------------------------------------------
	//	class LineObject : public GameObject; 
	//--------------------------------------------------------------------------------------
	LineObject::LineObject(const shared_ptr<Stage>& stage
	) :
		LineObject(stage, nullptr, nullptr)
	{
	}
	LineObject::LineObject(const shared_ptr<Stage>& stage,
		const shared_ptr<Character>& player,
		const shared_ptr<Character>& object
	) :
		GameObject(stage),
		m_MainObject(player),
		m_Object(object),
		m_Direc(0.0f),
		m_StartPos(Vec3(0.0f)),
		m_EndPos(Vec3(0.0f)),
		m_StartColor(Col4(1.0f)),
		m_EndColor(Col4(1.0f)),
		m_StartSize(Vec2()),
		m_EndSize(Vec2())
	{
	}
	void LineObject::OnCreate() {


		m_Vertices = {
			{m_StartPos, m_StartColor},
			{m_EndPos, m_EndColor}
		};

		m_Indices = {
			0,1
		};

		m_Draw = AddComponent<PCStaticDraw>();
		m_Draw->SetOriginalMeshUse(true);
		m_Draw->CreateOriginalMesh(m_Vertices, m_Indices);
		auto meshResoure = m_Draw->GetMeshResource();
		meshResoure->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	}
	void LineObject::OnUpdate() {
		auto player = m_MainObject.lock();
		if (!player) return;
		auto enemy = m_Object.lock();
		if (!enemy) return;

		SetLinePosition(player->GetComponent<Transform>()->GetPosition(), LinePos(Vec3(0.0f, 3.0f, 0.0f)));

	}
	Vec3 LineObject::LinePos(Vec3 pos) {
		auto enemy = m_Object.lock();
		if (enemy) {
			Vec3 enemyPos = enemy->GetComponent<Transform>()->GetPosition();
			Vec3 forward = enemy->GetComponent<Transform>()->GetForward();
			float face = atan2f(forward.z, forward.x);
			Vec3 linePos;
			linePos.x = (cosf(face) * pos.x) - (sinf(face) * pos.z);
			linePos.y = pos.y;
			linePos.z = (cosf(face) * pos.z) + (sinf(face) * pos.x);
			return enemyPos + linePos;
		}
	}

	void LineObject::VerticesUpdate() {
		m_Vertices = {
			{m_StartPos,m_StartColor},
			{m_EndPos,m_EndColor}
		};
		m_Draw->UpdateVertices(m_Vertices);
	}

	void LineObject::SetLinePosition(const Vec3& startPos, const Vec3& endPos) {
		m_StartPos = startPos;
		m_EndPos = endPos;
		m_Direc = endPos - startPos;
		m_Direc = m_Direc.normalize();

		VerticesUpdate();
	}

	void LineObject::SetLineColor(const Col4& startColor, const Col4& endColor) {
		m_StartColor = startColor;
		m_EndColor = endColor;

		VerticesUpdate();
	}

	Vec3 LineObject::GetStartPos() {
		return m_StartPos;
	}
	Vec3 LineObject::GetEndPos() {
		return m_EndPos;
	}
	Vec3 LineObject::GetDirec() {
		return m_Direc;
	}

}
//end basecross
