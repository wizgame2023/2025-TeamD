/*!
@file Enemy.h
@brief “G‚È‚Ç
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Enemy;
	class Tube;
	class ForecastLine;
	class HPBar;
	class RootPointer;
	class Mob : public Enemy
	{
	public:
		float m_BalletSpeed;
		float m_BalletRange;
		float m_MuzzleOffset;
		float m_BalletInterval;
		float m_KnockBackInterval;
		const float MAX_BALLET_INTERVAL;
		float m_ShotRandomInterval;
		bool m_IntervalStart;
		Vec3 m_Before = Vec3(0);


		shared_ptr<SharpFan> m_SearchFan;
		shared_ptr<HPBar> m_HpBar;
		shared_ptr<GameObject> m_NearPoint;
		vector<shared_ptr <GameObject>> m_PointData;

	public:
		Mob(const shared_ptr<Stage>& stage);
		Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		~Mob();
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void AsyncUpdate()override;

		virtual void Dead()override;
		void OnCollisionEnter(shared_ptr<GameObject>& other);
		Vec3 RootNaviGate();
		shared_ptr<Stage> GetStage();
		shared_ptr<Transform> GetTransfrom();
		float GetZoomTime() {
			return m_ZoneElapsedTime;
		}
		shared_ptr<GameObject> GetNearPoint()
		{
			return m_NearPoint;
		}
		vector<shared_ptr <GameObject>> GetPointData()
		{
			return m_PointData;
		}
		template <class NextState>
		void ChangeState() {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = make_unique<NextState>(GetThis<Mob>());
			m_currentState->Enter();
		}
	private:

	};
}
//end basecross
