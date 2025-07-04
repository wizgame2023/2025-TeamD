/*!
@file MobState.h
@brief 
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

	class BossStarting : public EnemyState<BossEnemy> {
		Effekseer::Handle m_SmokeHandle;
	public:
		BossStarting(shared_ptr<BossEnemy>& enemy) : EnemyState(enemy){}

		virtual void Enter() override;
		virtual void Execute()override;
		virtual void Exit()override;
	};
	class BossHostility : public EnemyState<BossEnemy>
	{
		bool m_IntruderAlert;
		Timer m_CooldownTimer;
		
	public:
		BossHostility(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_CooldownTimer(Timer(2.5f))
		{
		}

		virtual void Enter() override;
		virtual void Execute()override;
		virtual void Exit()override;

		static int m_MissileCount;
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
			m_AttackPosition(Vec3()) {
		}

		virtual void Ready(float time) {
			m_IsReady = true;
			m_ReadyTimer.SetTime(time, true);
		}
		virtual void Attack() {}

		virtual void Enter() override {}
		virtual void Execute()override {}
		virtual void Exit()override {}
	};
	class BossCrush : public AttackState<CrushAttack> {
	public:
		BossCrush(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy) {
		}

		virtual void Ready(float time)override;
	private:
		Effekseer::Handle m_SmokeHandle;

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
	class BossShakeOff : public AttackState<CrushAttack> {
	public:
		BossShakeOff(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy) {
		}

		virtual void Ready(float time)override;
	private:
		Effekseer::Handle m_SmokeHandle;

		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
}
//end basecross
