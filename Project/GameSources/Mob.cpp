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
		m_BalletRange(10.0f)
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


		m_SearchFan = m_Stage->AddGameObject<SharpFan>(L"SEARCH_RANGE", 36, 90.0f, 10.0f);
		
	}
	void Mob::OnUpdate()
	{
		Enemy::OnUpdate();
		AsyncUpdate();
		if (m_IsEndAsyncUpdate) {
			auto updateThread = thread(&Mob::AsyncUpdate, GetThis<Mob>());
			updateThread.detach();
		}
		float elapsed = App::GetApp()->GetElapsedTime();
		m_BalletInterval -= elapsed * m_ZoneElapsedTime;
		if (m_BalletInterval < 0) {
			m_BalletInterval = 0;
			m_ShotRandomInterval -= elapsed * m_ZoneElapsedTime;
			if (m_ShotRandomInterval <= 0) {
				m_ShotRandomInterval = 0;
			}
		}
		m_SearchFan->SetForward(m_Transform->GetForword().normalize());
		m_SearchFan->SetPosition(GetPosition());
		//AsyncUpdate();
	}
	void Mob::AsyncUpdate()
	{
		StartAsync();

		Enemy::AsyncUpdate();
		m_currentState->Execute();

		if (m_Intruder != nullptr) {
			if (Enemy::m_IntruderAlert)
			{
				m_Line->SetDrawActive(true);
				if (m_BalletInterval <= MAX_BALLET_INTERVAL * 0.2f) {
					m_Line->SetDrawActive(true);
				}
				else {
					m_Line->SetDrawActive(false);
				}
			}
			else {

				m_Line->SetDrawActive(false);
			}


			/*if(GetDistanceToIntruder() < )*/
		}

		EndAsync();
	}
	void Mob::Dead() {
		m_Stage->RemoveGameObject<SharpFan>(m_SearchFan);
		Enemy::Dead();
	}
	void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		Enemy::OnCollisionEnter(other);
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
