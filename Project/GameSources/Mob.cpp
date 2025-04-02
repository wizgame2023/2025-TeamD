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
		m_BalletInterval(0.5f), MAX_BALLET_INTERVAL(0.5f), m_ShotRandomInterval(0.0f),
		m_BalletSpeed(20.0f), m_MuzzleOffset(0.1f),
		m_BalletRange(10.0f), m_IntervalStart(false),
		m_KnockBackInterval(2.0f)
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
		m_currentState = make_unique<MobSearch>(GetThis<Mob>());
		m_currentState->Enter();

		////ƒfƒoƒbƒN—p
		//auto line = GetStage()->AddGameObject<LineObject>(m_Intruder, GetThis<Character>());
		//line->SetLineColor(Col4(1.0f, 0.0f, 0.0f, 1.0f), Col4(0.0f, 0.0f, 1.0f, 1.0f));

		auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
		auto pointers = pointerGroup->GetGroupVector();
		if (pointers.size() != 0) {
			auto navi = AddComponent<Navigate>();

			for (auto& point : pointers)
			{
				auto shObj = point.lock();
				Vec3 vec1 = shObj->GetComponent<Transform>()->GetPosition();
				m_PointData.push_back(shObj);
				if (m_NearPoint == nullptr)
				{
					m_NearPoint = shObj;
				}
				Vec3 vec0 = m_NearPoint->GetComponent<Transform>()->GetPosition();
				if ((vec1 - m_Position).length() < (vec0 - m_Position).length())
				{
					m_NearPoint = shObj;
				}
			}
			Vec3 pos = m_NearPoint->GetComponent<Transform>()->GetPosition();

			navi->SetTargetPosition(GetPosition(), pos);
		}

		m_SearchFan = m_Stage->AddGameObject<SharpFan>(L"SEARCH_RANGE", 36, 90.0f, 10.0f);

		m_HpBar = m_Stage->AddGameObject<HPBar>(GetThis<Mob>(), Vec3(0, GetScale().y / 2.0f, 0));
		m_HpBar->SetMaxHp(3);
		m_HpBar->SetCurrentHp(m_HP);
		//m_HpFrame = m_Stage->AddGameObject<Board>(L"HP_FRAME", Vec3(1, 1, 5), Vec3(1.0f, 0.1f, 1.0f), true);
	}
	void Mob::OnUpdate()
	{
		//m_HpBar->SetColor(Col4(1, 1, 1, 1));
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
		m_SearchFan->SetForward(m_Transform->GetForword().normalize());
		m_SearchFan->SetPosition(GetPosition());
		m_HpBar->SetCurrentHp(m_HP);
	}
	void Mob::AsyncUpdate()
	{
		StartAsync();

		Vec3 none = Vec3(0);
		float elapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 currntPosition = m_Transform->GetPosition();

		m_currentState->Execute();
		Enemy::AsyncUpdate();


		if (m_Intruder != nullptr) {
			if (Enemy::m_IntruderAlert)
			{
				if (m_BalletInterval <= MAX_BALLET_INTERVAL * 0.2f) {
					m_Line->SetDrawActive(true);
				}
				else {
					m_Line->SetDrawActive(false);
				}
			}
			else {

				m_Line->SetDrawActive(false);
				auto navi = GetComponent<Navigate>(false);
				if (navi) {
					Vec3 halfPos = navi->GetAStarForword(currntPosition);
					if (halfPos == Vec3(1, 0, 0))  SetRotation(Vec3(0, 90, 0));
					if (halfPos == Vec3(-1, 0, 0)) SetRotation(Vec3(0, 270, 0));
					if (halfPos == Vec3(0, 0, 1))  SetRotation(Vec3(0, 0, 0));
					if (halfPos == Vec3(0, 0, -1)) SetRotation(Vec3(0, 180, 0));


					if (halfPos != Vec3(0))
					{
						currntPosition += halfPos * 6.0f * elapsedTime * m_ZoneElapsedTime;
						SetPosition(currntPosition);
					}
					else {

						Vec3 before = m_NearPoint->GetComponent<Transform>()->GetPosition();
						SetPosition(before);
						Vec3 pos = RootNaviGate();
						navi->SetTargetPosition(GetPosition(), pos);
					}
				}
			}
		}
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
		auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
		auto pointers = pointerGroup->GetGroupVector();
		for (auto& point : pointers)
		{
			auto shObj = point.lock();
			Vec3 vec1 = shObj->GetComponent<Transform>()->GetPosition();
			auto shPtr = dynamic_pointer_cast<RootPointer>(m_NearPoint);
			wstring number = shPtr->GetPointerNumber();
			if (number == L"0_")
			{
				m_NearPoint = m_PointData[2];
				break;
			}
			else if (number == L"1_") {
				m_NearPoint = m_PointData[3];
				break;
			}
			else if (number == L"2_") {
				m_NearPoint = m_PointData[0];
				break;
			}
			else if (number == L"3_") {
				m_NearPoint = m_PointData[1];
				break;
			}
		}
		Vec3 pos = m_NearPoint->GetComponent<Transform>()->GetPosition();
		return pos;
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
