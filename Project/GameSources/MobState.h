/*!
@file MobState.h
@brief 敵のステート管理など
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Enemy;
	class Mob;

	class EnemyState {
	protected:
		shared_ptr<Enemy> m_Enemy;
		shared_ptr<Stage> m_Stage;
		shared_ptr<Transform> m_BoneTransform;
		shared_ptr<Character> m_Player;
	public:
		EnemyState(shared_ptr<Enemy>& enemy) :
			m_Enemy(enemy)
		{}
		virtual ~EnemyState() {}

		virtual void Enter();
		virtual void Execute() {}
		virtual void Exit() {}

	};

	class MobSearch : public EnemyState
	{
		bool m_IntruderAlert;
	public:
		MobSearch(shared_ptr<Enemy>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};

	class MobAlert : public EnemyState
	{
		bool m_IntruderAlert;

	public:
		MobAlert(shared_ptr<Enemy>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};

	class BossSearch : public EnemyState
	{
		bool m_IntruderAlert;

	public:
		BossSearch(shared_ptr<Enemy>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};
	class BossAttack : public EnemyState
	{
		bool m_IntruderAlert;

	public:
		BossAttack(shared_ptr<Enemy>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};


}
//end basecross
