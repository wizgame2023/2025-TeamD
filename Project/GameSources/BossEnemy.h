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
	public:

		BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		~BossEnemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void Dead();

		Vec3 GetPosition();

	};

	class BossEnemyLeg : public Enemy
	{
		shared_ptr<Enemy> m_Enemy;
		float m_Direction;
	public:
		BossEnemyLeg(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale, const shared_ptr<Enemy>& enemy ,const float& direction);
		~BossEnemyLeg();
		virtual void OnCreate();
		virtual void OnUpdate();
	};
}
//end basecross
