/*!
@file Enemy.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Mob::Mob(const shared_ptr<Stage>& stage) : Mob(stage, Vec3(), Vec3(1.0f)) {}

	Mob::Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage, position, scale),
		m_BalletInterval(0.5f), MAX_BALLET_INTERVAL(1.0f), m_ShotRandomInterval(1.0f),
		m_BalletSpeed(50.0f), m_MuzzleOffset(0.5f),
		m_BalletRange(10.0f), m_IntervalStart(false),
		m_KnockBackInterval(2.0f),
		m_NearPoint(nullptr),
		m_BulletRemain(10),
		m_Update(true)
	{
	}

	Mob::‾Mob() {}
	void Mob::OnCreate()
	{
		Enemy::OnCreate();
		m_kariState = kariState::hakai;
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			SetIntruder(player);
		}

		/*auto draw = GetComponent<BcPNTStaticDraw>();
		draw->SetDiffuse(Col4(1, 0, 0, 1));*/
		/*ptrColl->AddExcludeCollisionTag(L"Mob");*/

		//auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		//ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.25f), //(.1f, .1f, .1f),
			Vec3(0.0f, 90.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -0.75f, 0.0f)
		);
		ptrDraw->SetMeshResource(L"MOB");
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::AlphaBlend);
		ptrDraw->SetOwnShadowActive(true);

		auto ptrGra = AddComponent<Gravity>();
		auto shadowPtr = AddComponent<Shadowmap>();
		shadowPtr->SetMeshResource(L"MOB");
		shadowPtr->SetMeshToTransformMatrix(meshMat);
		AddAnimation();

		m_currentState = make_unique<MobSearch>(GetThis<Mob>());
		m_currentState->Enter();

	}
	void Mob::OnAfterCreate() {
		m_HpBar = m_Stage->AddGameObject<HPBar>(GetThis<Mob>(), Vec3(GetScale().x * 0.25f, GetScale().y * 1.5f, 0));
		m_HpBar->SetMaxHp(3);
		m_HpBar->SetCurrentHp(m_HP);
	}
	void Mob::OnUpdate()
	{
		Enemy::OnUpdate();
		if (m_Update)
		{
			float elapsed = GetGameElapsed();
			AsyncUpdate();
			auto draw = GetComponent<BcPNTBoneModelDraw>();
			draw->UpdateAnimation(elapsed);
			/*if (m_IsEndAsyncUpdate) {
				auto updateThread = thread(&Mob::AsyncUpdate, GetThis<Mob>());
				updateThread.detach();
			}*/
			if (m_IntervalStart == true)
			{
				draw->SetDiffuse(Col4(1, 0, 0, 1));

				m_BalletInterval -= elapsed * m_ZoneElapsedTime;
			}
			else {
				m_KnockBackInterval -= elapsed * m_ZoneElapsedTime;
				draw->SetDiffuse(Col4(1, 1, 1, 1));

				if (m_KnockBackInterval < 0)
				{
					m_BalletInterval = 0.5f;
					m_KnockBackInterval = 2.0f;
					m_IntervalStart = true;

				}
			}
			if (m_BalletInterval < 0) {
				m_BalletInterval = 0;
				m_ShotRandomInterval -= elapsed * m_ZoneElapsedTime;
				if (m_ShotRandomInterval <= 0) {
					m_ShotRandomInterval = 0;
				}
			}
			m_HpBar->SetCurrentHp(m_HP);
		}
		else {
			float elapsed = GetGameElapsed();;
			auto draw = GetComponent<BcPNTBoneModelDraw>();
			draw->UpdateAnimation(elapsed);
		}
	}
	void Mob::AsyncUpdate()
	{
		StartAsync();
		Vec3 none = Vec3(0);
		float elapsedTime = GetGameElapsed();
		Vec3 currntPosition = m_Transform->GetPosition();
		m_currentState->Execute();
		Enemy::AsyncUpdate();

		EndAsync();
	}
	void Mob::Dead() {
		m_Update = false;
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		float elapsedTime = GetGameElapsed();
		m_Stage->RemoveGameObject<SharpFan>(m_SearchFan);
		m_HpBar->Destroy();

		m_KnockBackTime -= elapsedTime;
		if (m_KnockBackTime > 0.0f)
		{
			SetAnim(L"Down", 0.0f, true);
			KnockBackTime();
		}
		else if(draw->IsTargetAnimeEnd())
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

	void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if ((other->FindTag(L"Bullet") || other->FindTag(L"HitJudge")) && m_IntervalStart)
		{
			SetAnim(L"Damage", 0.0f, true);
			Enemy::OnCollisionEnter(other);
			m_IntervalStart = false;
		}
	}

	void Mob::AddAnimation()
	{
		auto ptrDraw = GetComponent<BcPNTBoneModelDraw>();
		auto anim_fps = 60.0f;
		ptrDraw->AddAnimation(L"Walk", 20, 170, true, anim_fps * 1.75f);
		ptrDraw->AddAnimation(L"SetUp", 288, 72, false, anim_fps);
		ptrDraw->AddAnimation(L"Set", 318, 30, true, anim_fps);
		ptrDraw->AddAnimation(L"SetDown", 361, 103, false, anim_fps);
		ptrDraw->AddAnimation(L"Down", 557, 93, false, anim_fps);
		ptrDraw->AddAnimation(L"Damage", 557, 25, false, anim_fps);
		ptrDraw->AddAnimation(L"Reload", 661, 103, false, anim_fps);
	}

	Vec3 Mob::RootNaviGate()
	{
		float elapsedTime = GetGameElapsed();
		auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
		auto pointers = pointerGroup->GetGroupVector();
		//auto LegionGroup = GetStage()->GetSharedObjectGroup(L"Legion");
		//auto Legions = LegionGroup->GetGroupVector();
		shared_ptr<RootPointer> memoryPoint;
		shared_ptr<GameObject> memoryRndPoint;
		vector<int> memoryNum;

		for (auto point : pointers)
		{
			auto shObj = point.lock();
			int rnd = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
			memoryRndPoint = m_PointData[rnd];
			if (shObj = memoryRndPoint)
			{
				continue;
			}
			m_NearPoint = m_PointData[rnd];
		}
		m_BeforPoint = m_NearPoint;
		return m_NearPoint->GetComponent<Transform>()->GetPosition();
	}

	shared_ptr<Stage> Mob::GetStage()
	{
		return m_Stage;
	}

	shared_ptr<Transform> Mob::GetTransfrom()
	{
		return m_Transform;
	}
	void Mob::AlartMove(shared_ptr<Object> obj)
	{
		Vec3 target = obj->GetComponent<Transform>()->GetPosition();
		Vec3 position = m_Transform->GetPosition();
		Vec3 rot = target - position;
		rot.normalize();
		float rotate = atan2f(rot.x, rot.z);
		SetRotation(Vec3(0, rotate, 0));
	}
}
