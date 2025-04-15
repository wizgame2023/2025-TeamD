/*!
@file BossEnemy.h
@brief É{ÉXÇ»Ç«
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Enemy;
	class BossEnemy : public Enemy
	{
		bool m_IsAppearance;
		float m_ConditionTime;
		int m_ConditionDefeat;
		
		//shared_ptr<CrushAttack> m_Cruch;
		//shared_ptr<MachineGun> m_Gun;

		//friend BossAttack;
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
