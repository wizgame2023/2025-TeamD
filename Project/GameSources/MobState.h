/*!
@file MobState.h
@brief 敵のステート管理など
*/

#pragma once
#include "stdafx.h"
#include "Mob.h"
#include <Timer.h>

namespace basecross {
	class Enemy;
	//class Mob;

	template <typename T>
	class EnemyState {
	protected:
		shared_ptr<T> m_Enemy;
		shared_ptr<Stage> m_Stage;
		shared_ptr<Transform> m_Transform;
		shared_ptr<Character> m_Player;
	public:
		EnemyState(shared_ptr<T>& enemy) :
			m_Enemy(enemy)
		{}
		virtual ~EnemyState() {}

		virtual void Enter()
		{

			m_Stage = m_Enemy->GetStage();
			m_Transform = m_Enemy->GetComponent<Transform>();
			m_Player = m_Enemy->m_Intruder;
			
		}

		virtual void Execute() {};
		virtual void Exit() {}

	};

	class MobSearch : public EnemyState<Mob>
	{
		bool m_IntruderAlert;

		vector<Vec3> m_Path;

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
		float m_Interval;
		float m_BulletRelord;
		int m_CurrentBullet;
		int m_BulletRemain;

	public:
		MobAlert(shared_ptr<Mob>& enemy) :
			EnemyState(enemy),
			m_Interval(0.2f),
			m_CurrentBullet(5),
			m_BulletRelord(3.0f)
		{
		}

	private:
		void Enter() override;
		void Execute()override;
		void Exit()override;
	};

	//class MobJoinAlert : public EnemyState<Mob>
	//{
	//	bool m_IntruderAlert;
	//	float m_AlertTime;
	//	vector<Vec3> m_Path;

	//public:
	//	MobJoinAlert(shared_ptr<Mob>& enemy) :
	//		EnemyState(enemy)
	//	{
	//	}

	//private:
	//	void Enter() override;
	//	void Execute()override;
	//	void Exit()override;
	//};
}
//end basecross
