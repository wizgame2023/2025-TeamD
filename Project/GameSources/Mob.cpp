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
		m_HpBar = m_Stage->AddGameObject<HPBar>(GetThis<Mob>(),Vec3(0,1,0));
		m_HpBar->SetMaxHp(3);
		m_HpBar->SetCurrentHp(m_HP);
		//m_HpFrame = m_Stage->AddGameObject<Board>(L"HP_FRAME", Vec3(1, 1, 5), Vec3(1.0f, 0.1f, 1.0f), true);
	}
	void Mob::OnUpdate()
	{
		AsyncUpdate();
		Enemy::OnUpdate();
		
		/*if (m_IsEndAsyncUpdate) {
			auto updateThread = thread(&Mob::AsyncUpdate, GetThis<Mob>());
			updateThread.detach();
		}*/
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
		m_HpBar->SetCurrentHp(m_HP);
		//float raminingHp = (float)m_HP / 3.0f;
		//m_HpBar->GetComponent<Transform>()->SetScale(Vec3(raminingHp, 0.09f, 1));

		//m_HpBar->GetComponent<Transform>()->SetPosition(GetPosition() + Vec3(0, 1, 0));
		
		//AsyncUpdate();
	}
	void Mob::AsyncUpdate()
	{
		StartAsync();
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
			}


			/*if(GetDistanceToIntruder() < )*/
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
