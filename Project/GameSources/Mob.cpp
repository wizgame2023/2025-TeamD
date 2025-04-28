/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Mob::Mob(const shared_ptr<Stage>& stage) : Mob(stage, Vec3(), Vec3(1.0f)) {}

	Mob::Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage, position, scale),
		m_BalletInterval(0.5f), MAX_BALLET_INTERVAL(0.5f), m_ShotRandomInterval(0.5f),
		m_BalletSpeed(5.0f), m_MuzzleOffset(0.05f),
		m_BalletRange(10.0f), m_IntervalStart(false),
		m_KnockBackInterval(2.0f),
		m_NearPoint(nullptr),
		m_BulletRemain(10)
	{
	}

	Mob::~Mob() {}
	void Mob::OnCreate()
	{
		Enemy::OnCreate();
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			SetIntruder(player);
		}
		auto draw = GetComponent<BcPNTStaticDraw>();
		draw->SetDiffuse(Col4(1, 0, 0, 1));
		auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
		auto pointers = pointerGroup->GetGroupVector();
		auto navi = AddComponent<Navigate>();

		if (pointers.size() != 0) {

			for (auto& point : pointers)
			{
				auto shObj = point.lock();
				m_PointData.push_back(shObj);
			}
			int rnd = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
			m_NearPoint = m_PointData[rnd];
		}

		//m_SearchFan = m_Stage->AddGameObject<SharpFan>(L"SEARCH_RANGE", 36, 90.0f, 10.0f);

		m_HpBar = m_Stage->AddGameObject<HPBar>(GetThis<Mob>(), Vec3(0, GetScale().y / 2.0f, 0));
		m_HpBar->SetMaxHp(3);
		m_HpBar->SetCurrentHp(m_HP);
		//m_HpFrame = m_Stage->AddGameObject<Board>(L"HP_FRAME", Vec3(1, 1, 5), Vec3(1.0f, 0.1f, 1.0f), true);

		m_currentState = make_unique<MobSearch>(GetThis<Mob>());
		m_currentState->Enter();
	}
	void Mob::OnUpdate()
	{
		if (GetUpdateActive())
		{
			AsyncUpdate();
			Enemy::OnUpdate();
			auto draw = GetComponent<BcPNTStaticDraw>();

			/*if (m_IsEndAsyncUpdate) {
				auto updateThread = thread(&Mob::AsyncUpdate, GetThis<Mob>());
				updateThread.detach();
			}*/
			float elapsed = App::GetApp()->GetElapsedTime();
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
			//m_SearchFan->SetForward(m_Transform->GetForword().normalize());
			//m_SearchFan->SetPosition(GetPosition());
			m_HpBar->SetCurrentHp(m_HP);
		}
	}
	void Mob::AsyncUpdate()
	{
		StartAsync();
		Vec3 none = Vec3(0);
		float elapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 currntPosition = m_Transform->GetPosition();
			m_currentState->Execute();
		Enemy::AsyncUpdate();

		EndAsync();
	}
	void Mob::Dead() {
		m_Stage->RemoveGameObject<SharpFan>(m_SearchFan);
		m_HpBar->Destroy();
		Enemy::Dead();
	}

	void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			m_IntervalStart = false;
		}
		Enemy::OnCollisionEnter(other);
	}

	Vec3 Mob::RootNaviGate()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
		auto pointers = pointerGroup->GetGroupVector();
		auto LegionGroup = GetStage()->GetSharedObjectGroup(L"Legion");
		auto Legions = LegionGroup->GetGroupVector();
		auto navi = GetComponent<Navigate>();
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
}
//end basecross
