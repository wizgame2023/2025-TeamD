/*!
@file MobState.h
@brief “G‚ÌƒXƒe[ƒgŠÇ—‚È‚Ç
*/

#pragma once
#include "stdafx.h"
#include "MobState.h"
#include "BossEnemy.h"
#include "AttakCollision.h"
namespace basecross {
	class Enemy;
	class LineCube;
	class BossSearch : public EnemyState<BossEnemy>
	{
		bool m_IntruderAlert;
		vector<Vec3> m_Path;
		shared_ptr<LineCube> m_Line;
		float interval;
		float maxInterval;
	public:
		BossSearch(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),interval(0.0f),maxInterval(1.0f)
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
		float m_ChangeTime;
		float m_Cooldown;

		shared_ptr<CrushAttack> m_Cruch;
		shared_ptr<MachineGun> m_Gun;

		Vec3 m_LastInturderPosition;
	public:
		BossAttack(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_ChangeTime(1.0f),m_Cooldown(0.0f),m_LastInturderPosition(Vec3())
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};


}
//end basecross
