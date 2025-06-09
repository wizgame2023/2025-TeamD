/*!
@file BossEnemy.h
@brief
*/

#pragma once
#include "stdafx.h"
#include "AttakCollision.h"
#include "Timer.h"
namespace basecross {

	template <typename> class EnemyState;
	class Enemy;
	class BossEnemy : public Enemy
	{
		friend class BossHostility;
		friend class BossCrush;
		friend class BossGun;


		bool m_IsAppearance;
		float m_ConditionTime;
		int m_ConditionDefeat;
		float m_MotionRate;

		unique_ptr<EnemyState<BossEnemy>> m_currentState;  
		unique_ptr<EnemyState<BossEnemy>> m_nextState;   


		shared_ptr<CrushAttack> m_Cruch;
		shared_ptr<MachineGun> m_Gun;
		shared_ptr<Missile> m_Missile;

		shared_ptr<EffectManeger> m_Effect;

		float m_Stun;
		bool m_IsStun;
		Timer m_ComboTimer;
		int m_ComboCount;

		Vec3 m_StartPosition;

		Timer m_DamageEffectTime;

		wstring m_CurrentAnimationKey;

		Effekseer::Handle m_EffectHandle;
	public:
		BossEnemy(const shared_ptr<Stage>& stage);
		BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		~BossEnemy();
		virtual void OnCreate();
		virtual void OnAfterCreate()override;
		virtual void OnUpdate();
		virtual void OnCollisionEnter(shared_ptr<GameObject>& other)override;
		virtual void Dead();
		virtual void Damage(float damage, const bool& isSound = true)override;
		virtual void Move(const Vec3& direction,const bool& isGameSpeed = true)override;

		void RegisterAttack();

		void AddAnimation();
		void SetAnimation(const wstring& key, const bool& isChange = false);
		bool GetAnimationFinish();
		wstring GetCurrentAnimationKey();

		void AddStun(float stun);
		
		void SetCondition(float time, int defeatCount) {
			m_IsAppearance = false;
			m_ConditionDefeat = defeatCount;
			m_ConditionTime = time;
		}

		template <class NextState>
		void ChangeState() {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = make_unique<NextState>(GetThis<BossEnemy>());
			m_currentState->Enter();
		}

		float GetMotionRate() {
			return m_MotionRate;
		}
	};

	class BossEnemyLeg : public Enemy
	{
		shared_ptr<Enemy> m_Enemy;
		float m_Direction;
	public:
		BossEnemyLeg(const shared_ptr<Stage>& stage);
		BossEnemyLeg(const shared_ptr<Stage>& stage, const Vec3& position,  const shared_ptr<Enemy>& enemy ,const float& direction);
		~BossEnemyLeg();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void Dead();

	};


}
//end basecross
