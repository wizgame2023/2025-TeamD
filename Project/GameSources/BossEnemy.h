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
	class BossEnemy;

	struct Skill{
		shared_ptr<EnemyState<BossEnemy>> m_Skill;
		float m_Prob;
	};

	
	class BossEnemy : public Enemy
	{
		friend class BossHostility;
		friend class BossCrush;
		friend class BossGun;
		friend class BossShakeOff;


		bool m_IsAppearance;
		float m_ConditionTime;
		int m_ConditionDefeat;
		float m_MotionRate;

		shared_ptr<EnemyState<BossEnemy>> m_currentState;  
		shared_ptr<EnemyState<BossEnemy>> m_nextState;


		shared_ptr<CrushAttack> m_Cruch;
		shared_ptr<MachineGun> m_Gun;
		shared_ptr<Missile> m_Missile;

		shared_ptr<EffectManager> m_Effect;

		float m_Stun;
		bool m_IsStun;
		Timer m_HealStun;
		bool m_DeadEffect;
		Timer m_ComboTimer;
		int m_ComboCount;
		bool m_IsGround;
		float m_AroundPlayerTime;

		Vec3 m_StartPosition;

		Timer m_InvincibleTimer;

		wstring m_CurrentAnimationKey;

		Effekseer::Handle m_EffectHandle;
		Effekseer::Handle m_EffectBombHandle;
		Effekseer::Handle m_SmokeHandle;

		vector<Skill> m_Skills;

		void AddSkill(const shared_ptr<EnemyState<BossEnemy>>& skill, float prob) {
			m_Skills.push_back({ skill,prob });
		}
	public:
		BossEnemy(const shared_ptr<Stage>& stage);
		BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		‾BossEnemy();
		virtual void OnCreate();
		virtual void OnAfterCreate()override;
		virtual void OnUpdate();
		virtual void OnSpawn();
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
		float GetStun() const { return m_Stun; }
		
		void SetCondition(float time, int defeatCount) {
			m_IsAppearance = false;
			m_ConditionDefeat = defeatCount;
			m_ConditionTime = time;
		}

		template <class NextState>
		void ChangeState() {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = make_shared<NextState>(GetThis<BossEnemy>());
			m_currentState->Enter();
		}

		void ChangeState(int i) {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = m_Skills[i].m_Skill;
			m_currentState->Enter();
		}
		float GetMotionRate() {
			return m_MotionRate;
		}
	};
}
//end basecross
