/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
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
	}
	void Mob::OnUpdate()
	{
		Enemy::OnUpdate();
		float elapsed = App::GetApp()->GetElapsedTime();

		if (m_Intruder != nullptr) {
			m_BalletInterval -= elapsed * m_ZoneElapsedTime;
			if (m_BalletInterval <= 0) {
				Vec3 direction = GetDirectionToIntruder();
				GetStage()->AddGameObject<Ballet>(m_Transform->GetPosition() + direction * m_MuzzleOffset,m_BalletSpeed,direction, m_BalletRange);
				m_BalletInterval = MAX_BALLET_INTERVAL;
			}
		}
	}
	void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		Enemy::OnCollisionEnter(other);
	}
}
//end basecross
