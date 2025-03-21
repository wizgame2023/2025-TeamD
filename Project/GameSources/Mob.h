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

	class Mob : public Enemy
	{
	public:
		float m_BalletSpeed;
		float m_BalletRange;
		float m_MuzzleOffset;
		float m_BalletInterval;
		const float MAX_BALLET_INTERVAL;
		float m_ShotRandomInterval;
		shared_ptr<Tube> m_SearchEffect;
		shared_ptr<ForecastLine> m_Line;
		shared_ptr<ForecastLine> m_fLine;
		void DrawSearchRange();
		Mob(const shared_ptr<Stage>& stage);
		Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		~Mob();
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void AsyncUpdate()override;

		virtual void Dead()override;
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		shared_ptr<Stage> GetStage();
		shared_ptr<Transform> GetTransfrom();
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
