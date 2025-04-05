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

		shared_ptr<ForecastLine> m_BossLine;
		shared_ptr<ForecastLine> m_FLine;

	public:
		BossEnemy(const shared_ptr<Stage>& stage);
		BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		~BossEnemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void Dead();

		void SetCondition(float time, int defeatCount) {
			m_IsAppearance = false;
			m_ConditionDefeat = defeatCount;
			m_ConditionTime = time;
		}
		Vec3 GetPosition();

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
