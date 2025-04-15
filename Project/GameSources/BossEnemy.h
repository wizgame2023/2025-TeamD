/*!
@file BossEnemy.h
@brief ƒ{ƒX‚È‚Ç
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	template <typename> class EnemyState;

	class Enemy;
	class BossEnemy : public Enemy
	{
		bool m_IsAppearance;
		float m_ConditionTime;
		int m_ConditionDefeat;

		unique_ptr<EnemyState<BossEnemy>> m_currentState;  
		unique_ptr<EnemyState<BossEnemy>> m_nextState;   

		//shared_ptr<CrushAttack> m_Cruch;
		//shared_ptr<MachineGun> m_Gun;

		friend BossAttack;
	public:
		BossEnemy(const shared_ptr<Stage>& stage);
		BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		~BossEnemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void Dead();
		virtual void Damage(float damage, const bool& isSound = true)override;

		void SetCondition(float time, int defeatCount) {
			m_IsAppearance = false;
			m_ConditionDefeat = defeatCount;
			m_ConditionTime = time;
		}
		Vec3 GetPosition();
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		template <class NextState>
		void ChangeState() {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = make_unique<NextState>(GetThis<BossEnemy>());
			m_currentState->Enter();
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
