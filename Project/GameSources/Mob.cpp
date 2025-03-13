/*!
@file Enemy.cpp
@brief 敵など実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Mob::Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage,position,scale),m_BalletInterval(0.5f),MAX_BALLET_INTERVAL(0.5f),m_BalletSpeed(10.0f),m_MuzzleOffset(1.5f),m_BalletRange(10.0f)
	{
	}
	Mob::~Mob()
	{
	}
	void Mob::OnCreate()
	{
		Enemy::OnCreate();
		////デバック用
		//auto line = GetStage()->AddGameObject<LineObject>(m_Intruder, GetThis<Character>());
		//line->SetLineColor(Col4(1.0f, 0.0f, 0.0f, 1.0f), Col4(0.0f, 0.0f, 1.0f, 1.0f));

		DrawSearchRange();
	}
	void Mob::OnUpdate()
	{
		Enemy::OnUpdate();
		float elapsed = App::GetApp()->GetElapsedTime();

		if (m_Intruder != nullptr) {
			if (m_IntruderAlert)
			{
				m_BalletInterval -= elapsed * m_ZoneElapsedTime;
				if (m_BalletInterval <= 0) {
					Vec3 direction = GetDirectionToIntruder();
					GetStage()->AddGameObject<Ballet>(m_Transform->GetPosition() + direction * m_MuzzleOffset,m_BalletSpeed,direction, m_BalletRange);
					m_BalletInterval = MAX_BALLET_INTERVAL;
				}
			}

		}
	}
	void Mob::Dead() {
		GetStage()->RemoveGameObject<Tube>(m_SearchEffect);
		Enemy::Dead();
	}
	void Mob::DrawSearchRange() {
		m_SearchEffect = GetStage()->AddGameObject<Tube>(L"SEARCH_RANGE");
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
