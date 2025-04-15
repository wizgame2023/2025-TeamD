/*!
@file Enemy.cpp
@brief “G‚È‚ÇŽÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void BossSearch::Enter()
	{
		EnemyState::Enter();
		m_Line = m_Stage->AddGameObject<LineCube>(0.02f, Col4(0, 1, 0, 1));
	}
	void BossSearch::Execute()
	{
		auto navigate = m_Enemy->GetComponent<Navigate>(false);
		float elapsed = App::GetApp()->GetElapsedTime();
		if (navigate) {
			if (m_Path.size() == 0) {
				m_Line->SetDrawActive(false);
				if (interval <= 0.0f) {
					float rndOparation = Util::RandZeroToOne() * 100.0f;
					if (rndOparation < 40) {
						auto group = m_Stage->GetSharedObjectGroup(L"PointerGroup");
						auto pointers = group->GetGroupVector();
						int rnd = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
						auto pointer = pointers[rnd].lock();
						if (pointer != nullptr) {
							m_Path = navigate->FindPathWithWaypoints2(navigate->GetNearPinter(m_Enemy->GetPosition()), pointer->GetComponent<Transform>()->GetPosition());
						}
					}
					else if (rndOparation < 60) {

					}
					else {
						Vec3 target;
						do {
							target = Vec3(Util::RandZeroToOne() * 10.0f, m_Enemy->GetPosition().y, Util::RandZeroToOne() * 10.0f);
						} while (RayCast::HitTestVec(RayCastHit(), Line(m_Enemy->GetPosition(), target), m_Enemy->GetStage()->GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy" }));
						m_Path.push_back(target);
					}
					interval = maxInterval;
				}
				else {
					interval -= elapsed;
				}
			}
			else {
				m_Line->SetDrawActive(true);
				Vec3 pos = m_Enemy->GetPosition();
				m_Path[0].y = pos.y;
				Vec3 direction = m_Path[0] - pos;
				m_Line->SetLine(Line(pos, m_Path[0]));
				if (direction.length() < 0.1f) {
					m_Path.erase(m_Path.begin());
				}
				else {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Transform->SetRotation(Vec3(0, rotate, 0));
					pos += direction * 1.0f * elapsed;
				}
				m_Enemy->SetPosition(pos);
			}

		}
		m_Enemy->SearchRange();
		if (m_Enemy->GetIntruderAlert()) {
			m_Enemy->ChangeState<BossAttack>();
		}
	}
	void BossSearch::Exit()
	{
		m_Stage->RemoveGameObject<LineCube>(m_Line);
	}
	void BossAttack::Enter()
	{
		EnemyState::Enter();
		m_Cruch = m_Stage->AddGameObject<CrushAttack>(Vec3(0.5f, 0.1f, 0.5f), AttackDate(3.0f,0.5f,0.25f,3.0f,1.0f),3.0f);
		m_Gun = m_Stage->AddGameObject<MachineGun>(m_Enemy->m_Intruder, AttackDate(1.0f, 5.0f, 2.0f, 10.0f, 2.0f), 20.0f);
	}
	void BossAttack::Execute()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * m_Enemy->m_ZoneElapsedTime;
		auto navi = m_Enemy->GetComponent<Navigate>();
		m_ChangeTime -= elapsedTime;
		Vec3 position = m_Enemy->GetPosition();
		Vec3 intruderPosition = m_Enemy->m_Intruder->GetPosition();
		if (RayCast::HitTestVec(RayCastHit(), Line(position, intruderPosition), m_Enemy->GetStage()->GetGameObjectVec(), { L"Bullet",L"Line",L"Enemy" })) {
			m_Enemy->SetIntruderAlert(false);
			if (m_LastInturderPosition != Vec3()) {
				Vec3 direction = m_LastInturderPosition - position;
				if (direction.length() >= 0.1f) {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Enemy->SetRotation(Vec3(0, rotate, 0));
					position += direction * 1.0f * elapsedTime;
				}
				else {
					m_Enemy->SearchRange();
					if (!m_Enemy->GetIntruderAlert()) {
						m_Enemy->ChangeState<BossSearch>();
					}
				}
				m_Enemy->SetPosition(position);
			}
		}
		else {
			m_Enemy->SetIntruderAlert(true);

			m_LastInturderPosition = intruderPosition;
			Vec3 direction = intruderPosition - position;
			if (m_Cooldown > 0) {
				m_Cooldown -= elapsedTime;
			}
			else {
				float distance = direction.length();
				if (distance > 2.0f && distance < m_Gun->GetRange() && m_Gun->GetCooldown() == 0) {
					m_Gun->Play(position + Vec3(0, 0.1f, 0.0f));
					m_Cooldown = m_Gun->GetCharaCooldown();
				}
				else if (distance < m_Cruch->GetRange() && m_Cruch->GetCooldown() == 0) {
					m_Cruch->Play(m_Enemy->GetPosition() + direction.normalize() * 0.25f);
					m_Cooldown = m_Cruch->GetCharaCooldown();
				}
				else {
					direction = direction.normalize();
					float rotate = atan2f(direction.x, direction.z);
					m_Enemy->SetRotation(Vec3(0, rotate, 0));
					position += direction * 1.0f * elapsedTime;
					m_Enemy->SetPosition(position);
				}
			}

		}
	}
	void BossAttack::Exit()
	{
	}

}

//end basecross
