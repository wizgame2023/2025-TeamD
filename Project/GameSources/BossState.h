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
			EnemyState(enemy), m_OperatorIntarval(Timer(1.0f))
		{
		}

		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
	class BossWarning : public BossSearch {
		Vec3 m_StartDirection;
		Vec3 m_EndDirection;
	public:
		BossWarning(shared_ptr<BossEnemy>& enemy) :
			BossSearch(enemy) {
		}
	private:
		void Execute()override;
	};
	class BossHostility : public EnemyState<BossEnemy>
	{
		bool m_IntruderAlert;
		Timer m_CooldownTimer;
		Timer m_SideStepTimer;

		Vec3 m_LastInturderPosition;

		Vec3 m_NearDistance;
		float m_SideStepDirection;
	public:
		BossHostility(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_CooldownTimer(Timer(5.0f)), m_SideStepTimer(Timer(0.5f, false)), m_LastInturderPosition(Vec3()), m_NearDistance(0.2f), m_SideStepDirection(1.0f)
		{
		}

		virtual void Enter() override;
		virtual void Execute()override;
		virtual void Exit()override;
	};
	template<typename T>
	class AttackState : public EnemyState<BossEnemy> {

	protected:
		shared_ptr<T> m_Attack;
		Timer m_CooldownTimer;
		Timer m_ReadyTimer;
		float m_RotateTime;
		bool m_IsReady;
		bool m_IsFinish;

		Vec3 m_FinishedForward;
		Vec3 m_AttackPosition;

		bool LerpRotatePlayer(Vec3 direction) {
			float elapsed = App::GetApp()->GetElapsedTime();
			Vec3 result = m_FinishedForward + (direction - m_FinishedForward) * m_RotateTime;
			m_RotateTime = min(1.0f, m_RotateTime + elapsed);
			float rotateY = atan2f(result.x, result.z);
			m_Enemy->SetRotation(Vec3(0, rotateY, 0));

			return (result - direction).length() < 0.05f;
		}
	public:
		AttackState(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_CooldownTimer(Timer(false)), m_ReadyTimer(Timer(false)),
			m_IsReady(false), m_IsFinish(false), m_FinishedForward(Vec3()), m_RotateTime(0.0f),
			m_AttackPosition(Vec3()){}
	private:
		virtual void Enter() override {}
		virtual void Execute()override {}
		virtual void Exit()override {}
	};
	class BossCrush : public AttackState<CrushAttack> {
	public:
		BossCrush(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy) {
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
	class BossGun : public AttackState<Missile> {

	public:
		BossGun(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy) {
		}

	private:
		virtual void Enter() override;
		virtual void Execute()override;
		virtual void Exit()override;
	};
}
//end basecross
