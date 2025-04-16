/*!
@file MobState.h
@brief “G‚ÌƒXƒe[ƒgŠÇ—‚È‚Ç
*/

#pragma once
#include "stdafx.h"
#include "MobState.h"
#include "BossEnemy.h"
#include "AttakCollision.h"
#include "Timer.h"
namespace basecross {
	class Enemy;
	class LineCube;

	class BossSearch : public EnemyState<BossEnemy>
	{
		bool m_IntruderAlert;
		vector<Vec3> m_Path;
		shared_ptr<LineCube> m_Line;
		Timer m_OperatorIntarval;
	public:
		BossSearch(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),m_OperatorIntarval(Timer())
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
	class BossAttack : public EnemyState<BossEnemy>
	{
		bool m_IntruderAlert;
		Timer m_CooldownTimer;
		Timer m_SideStepTimer;

		Vec3 m_LastInturderPosition;

		Vec3 m_NearDistance;
		float m_SideStepDirection;
	public:
		BossAttack(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_CooldownTimer(Timer(false)),m_SideStepTimer(Timer()), m_LastInturderPosition(Vec3()), m_NearDistance(0.2f), m_SideStepDirection(1.0f)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};


}
//end basecross
