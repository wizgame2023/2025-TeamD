/*!
@file BossEnemy.cpp
@brief ボス敵の実装
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	BossEnemy::BossEnemy(const shared_ptr<Stage>& stage) : BossEnemy(stage, Vec3(), Vec3(1.0f)) {}

	BossEnemy::BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage, position, scale), m_IsAppearance(false), m_ConditionTime(0.0f), m_ConditionDefeat(100)
	{
	}
	BossEnemy::~BossEnemy()
	{
	}
	void BossEnemy::OnCreate()
	{
		Enemy::OnCreate();
		m_HP = 3;
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			SetIntruder(player);
		}

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_CUBE");

		auto navi = AddComponent<Navigate>();
		navi->SetTargetPosition(m_Transform->GetPosition(), m_Intruder->GetPosition());

		m_currentState = make_unique<BossSearch>(GetThis<Enemy>());
		m_currentState->Enter();

		AddComponent<Gravity>();
		m_BossLine = m_Stage->AddGameObject<ForecastLine>(GetThis<Enemy>(), true);
	}
	void BossEnemy::OnUpdate()
	{
		Enemy::OnUpdate();
		SetDrawActive(m_IsAppearance);

		float elapsed = App::GetApp()->GetElapsedTime();
		if (!m_IsAppearance) {
			m_ConditionTime -= elapsed;
			if (m_ConditionTime < 0) {
				m_ConditionTime = 0;
				m_IsAppearance = true;
			}

			int defeatCount = ScoreManager::Instance()->GetEliminateEnemyCount();
			if (defeatCount >= m_ConditionDefeat) {
				m_IsAppearance = true;
			}
		}
		else {
			AsyncUpdate();
			Vec3 pos = GetPosition();
			m_currentState->Execute();
			auto navi = GetComponent<Navigate>();
			float elapsedTime = App::GetApp()->GetElapsedTime();
			Vec3 currntPosition = GetPosition();

			if (m_Intruder != nullptr) {
				if (m_IntruderAlert) {
					Vec3 halfPos = navi->GetAStarForword(currntPosition);

					if (halfPos == Vec3(1, 0, 0))  m_Transform->SetRotation(Vec3(0, 90, 0));
					if (halfPos == Vec3(-1, 0, 0)) m_Transform->SetRotation(Vec3(0, 270, 0));
					if (halfPos == Vec3(0, 0, 1))  m_Transform->SetRotation(Vec3(0, 0, 0));
					if (halfPos == Vec3(0, 0, -1)) m_Transform->SetRotation(Vec3(0, 180, 0));

					if (halfPos != Vec3(0))
					{
						currntPosition += halfPos * 3.0f * elapsedTime * m_ZoneElapsedTime;
						SetPosition(currntPosition);
					}

				}
				else {
					Vec3 taregtpoint = navi->AvoidBlock(GetPosition(), m_Intruder->GetPosition());
					if (taregtpoint == Vec3(1, 0, 0))  m_Transform->SetRotation(Vec3(0, 90, 0));
					else if (taregtpoint == Vec3(-1, 0, 0)) m_Transform->SetRotation(Vec3(0, 270, 0));
					else if (taregtpoint == Vec3(0, 0, 1))  m_Transform->SetRotation(Vec3(0, 0, 0));
					else if (taregtpoint == Vec3(0, 0, -1)) m_Transform->SetRotation(Vec3(0, 180, 0));

					if (taregtpoint != Vec3(0))
					{
						currntPosition += taregtpoint * 3.0f * elapsedTime * m_ZoneElapsedTime;
						SetPosition(currntPosition);
					}
				}
			}
			auto device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
			if (device.bConnected) {
				if (device.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) {

				}
				if (device.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) {
					m_Stage->AddGameObject<CrushAttack>(GetPosition() + m_Transform->GetForward(), Vec3(2.0f, 1.0f, 2.0f), 1, 0.5f, 5.0f);
				}
				if (device.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {

				}
				if (device.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT) {

				}
			}

			EndAsync();
		}
	}
	void BossEnemy::Dead()
	{
		Enemy::Dead();
	}

	Vec3 BossEnemy::GetPosition()
	{
		return m_Transform->GetPosition();;
	}

	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage) : BossEnemyLeg(stage, Vec3(), shared_ptr<Enemy>(), float()) {}

	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage, const Vec3& position, const shared_ptr<Enemy>& enemy, const float& direction) :
		Enemy(stage, position, Vec3(1.0f, 3.0f, 1.0f)),
		m_Enemy(enemy),
		m_Direction(direction)
	{
	}

	BossEnemyLeg::~BossEnemyLeg()
	{
	}

	void BossEnemyLeg::OnCreate()
	{
		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(m_Position.x + m_Direction, m_Position.y - 1.5f, m_Position.z);
		m_Transform->SetRotation(m_Rotation);
		m_Transform->SetScale(Vec3(1.0f, 3.0f, 1.0f));

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_CUBE");

	}
	void BossEnemyLeg::OnUpdate()
	{
	}
	void BossEnemyLeg::Dead()
	{
		Enemy::Dead();
	}
}
//end basecross