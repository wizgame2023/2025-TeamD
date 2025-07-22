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
		BossStarting(shared_ptr<BossEnemy>& enemy) : EnemyState(enemy),m_SmokeHandle(0){}

		virtual void Enter() override;
		virtual void Execute()override;
		virtual void Exit()override;
	};
	class BossHostility : public EnemyState<BossEnemy>
	{
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

	enum class AState {
		Ready,
		Playing,
		Finished
	};

	template<typename T>
	class AttackState : public EnemyState<BossEnemy> {
	private:
		Timer m_StateTimer;

	protected:

		AState m_State;
		AState m_NextState;

		function<bool()> m_StateCheck;

		shared_ptr<T> m_Attack;
		float m_RotateTime;

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
		void SetState(AState state, float time) {
			m_NextState = state;
			m_StateTimer.SetTime(time, true);
		}
		void SetState(AState state, function<bool()> func) {
			m_NextState = state;
			m_StateCheck = func;
		}
		bool IsStandBy() {
			if (m_StateTimer.CheckTime()) {
				if (m_StateCheck != nullptr && m_StateCheck()) {
					return false;
				}
				else {
					return false;
				}
			}
			return true;
		}

		void Init() {
			m_State = AState::Ready;
			m_NextState = AState::Ready;

			m_AttackPosition = Vec3();
			m_FinishedForward = Vec3();
			m_RotateTime = 0.0f;

		}

	public:
		AttackState(shared_ptr<BossEnemy>& enemy) :
			EnemyState(enemy),
			m_FinishedForward(Vec3()), m_RotateTime(0.0f),
			m_AttackPosition(Vec3()),
			m_State(AState::Ready), m_NextState(AState::Ready), m_StateTimer(Timer(false)){
		}

		virtual void Attack() {}

		virtual void Enter() override {
			Init();
			EnemyState::Enter();
		}
		virtual void Execute()override {
			if (m_State != m_NextState && m_StateTimer.UpdateTimer()) {
				m_State = m_NextState;
			}
		}
		virtual void Exit()override {}


		void SetSkill(shared_ptr<T> skill) {
			m_Attack = skill;
		}
	};
	class BossCrush : public AttackState<CrushAttack> {
	public:
		BossCrush(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy) ,m_SmokeHandle(0){
		}

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
	class BossShakeOff : public AttackState<ShakeOffAttack> {
	public:
		BossShakeOff(shared_ptr<BossEnemy>& enemy) :
			AttackState(enemy),m_SmokeHandle(0) {
		}

	private:
		Effekseer::Handle m_SmokeHandle;

		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
}
//end basecross
