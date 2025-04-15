/*!
@file MobState.h
@brief 敵のステート管理など
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Enemy;
	class Mob;

	template <typename T>
	class EnemyState {
	protected:
		shared_ptr<T> m_Enemy;
		shared_ptr<Stage> m_Stage;
		shared_ptr<Transform> m_Transform;
		shared_ptr<Character> m_Player;
		vector<Vec3> m_Path;
	public:
		EnemyState(shared_ptr<T>& enemy) :
			m_Enemy(enemy)
		{}
		virtual ~EnemyState() {}

		virtual void Enter();
		virtual void Execute() {}
		virtual void Exit() {}

	};

	class MobSearch : public EnemyState<Mob>
	{
		bool m_IntruderAlert;

	public:
		MobSearch(shared_ptr<Mob>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};

	class MobAlert : public EnemyState<Mob>
	{
		bool m_IntruderAlert;

	public:
		MobAlert(shared_ptr<Mob>& enemy) :
			EnemyState(enemy)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};

	class MobJoinAlert : public EnemyState<Mob>
	{
		bool m_IntruderAlert;

	public:
		MobJoinAlert(shared_ptr<Mob>& enemy) :
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
