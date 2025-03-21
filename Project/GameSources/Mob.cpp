/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Mob::Mob(const shared_ptr<Stage>& stage) : Mob(stage,Vec3(),Vec3(1.0f)){}

	Mob::Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage,position,scale),
		m_BalletInterval(0.5f),MAX_BALLET_INTERVAL(0.5f),m_ShotRandomInterval(0.0f),
		m_BalletSpeed(20.0f),m_MuzzleOffset(1.5f),
		m_BalletRange(10.0f)
	{}

	Mob::~Mob(){}
	void Mob::OnCreate()
	{
		Enemy::OnCreate();
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			SetIntruder(player);
		}

		////ƒfƒoƒbƒN—p
		//auto line = GetStage()->AddGameObject<LineObject>(m_Intruder, GetThis<Character>());
		//line->SetLineColor(Col4(1.0f, 0.0f, 0.0f, 1.0f), Col4(0.0f, 0.0f, 1.0f, 1.0f));

		DrawSearchRange();
		m_Line = m_Stage->AddGameObject<ForecastLine>(GetThis<Mob>());
		m_fLine = m_Stage->AddGameObject<ForecastLine>(GetThis<Mob>());
	}
	void Mob::OnUpdate()
	{
		float elapsed = App::GetApp()->GetElapsedTime();
		m_BalletInterval -= elapsed * m_ZoneElapsedTime;
		if (m_BalletInterval < 0) {
			m_BalletInterval = 0;
			m_ShotRandomInterval -= elapsed * m_ZoneElapsedTime;
			if (m_ShotRandomInterval <= 0) {
				m_ShotRandomInterval = 0;
			}
		}
		AsyncUpdate();
		/*if (m_IsEndAsyncUpdate) {
			auto updateThread = thread(&Mob::AsyncUpdate);
			updateThread.join();
		}*/
		//if (m_Intruder != nullptr) {
		//	m_Line->SetLine(GetDirectionToIntruder(), m_Transform->GetPosition(), 10.0f);
		//	if (m_IntruderAlert)
		//	{
		//		if (m_BalletInterval <= 0) {
		//			Vec3 direction = GetDirectionToIntruder();
		//			auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * m_MuzzleOffset,m_BalletSpeed,direction, m_BalletRange);
		//			m_BalletInterval = MAX_BALLET_INTERVAL;
		//			
		//			m_Line->SetBallet(ballet);
		//			m_Line = m_Stage->AddGameObject<ForecastLine>(GetThis<Mob>());
		//		}
		//	}
		//	if (m_BalletInterval <= MAX_BALLET_INTERVAL * 0.2f) {
		//		m_Line->SetDrawActive(true);
		//	}
		//	else {
		//		m_Line->SetDrawActive(false);
		//	}
		//	m_SearchEffect->SetUpdateActive(false);
		//	m_SearchEffect->SetDrawActive(false);
		//	/*if(GetDistanceToIntruder() < )*/
		//}
	}
	void Mob::AsyncUpdate()
	{
		double angle = 45.0;
		StartAsync();
		Enemy::OnUpdate();

		if (m_Intruder != nullptr) {
			m_Line->SetLine(GetDirectionToIntruder(), m_Transform->GetPosition(), 10.0f);
			m_fLine->SetLine(m_Transform->GetForward(), m_Transform->GetPosition(), 2.0f);
			if (Enemy::m_IntruderAlert)
			{
				m_Line->SetDrawActive(true);
				if (m_BalletInterval <= 0 && m_ShotRandomInterval <= 0) {
					Vec3 direction = GetDirectionToIntruder();

					auto ballet = m_Stage->AddGameObject<Bullet>(m_Transform->GetPosition() + direction * m_MuzzleOffset, m_BalletSpeed, direction, m_BalletRange);
					m_BalletInterval = MAX_BALLET_INTERVAL;

					m_Line->SetBallet(ballet);
					m_Line = m_Stage->AddGameObject<ForecastLine>(GetThis<Mob>());

					m_ShotRandomInterval = Util::RandZeroToOne() * (MAX_BALLET_INTERVAL * 0.5f);
				}
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
		m_Stage->RemoveGameObject<Tube>(m_SearchEffect);
		m_Line->Destroy();
		Enemy::Dead();
	}
	void Mob::DrawSearchRange() {
		m_SearchEffect = m_Stage->AddGameObject<Tube>(L"SEARCH_RANGE");
		m_SearchEffect->SetHeight(0.0f);
		m_SearchEffect->SetTopRadius(0.0f);
		m_SearchEffect->SetBottomRadius(10.0f);
		m_SearchEffect->SetTopColor(1.0f, 0.0f, 0.0f, 1.0f);
		m_SearchEffect->SetBottomColor(1.0f, 0.0f, 0.0f, 1.0f);
		m_SearchEffect->SetLoops({ 5.0f,1.0f });
		m_SearchEffect->SetScrollPerSecond({ 0.0f,1.0f });
		m_SearchEffect->SetTracking(GetThis<Mob>());
		m_SearchEffect->SetTrackingDiff(Vec3(0.0f, -0.4f, 0.0f));
		auto effectTrans = m_SearchEffect->GetComponent<Transform>();
		effectTrans->SetPosition(m_Transform->GetPosition());
		effectTrans->SetScale(Vec3(1.0f));
	}
	void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		Enemy::OnCollisionEnter(other);
	}
}
//end basecross
