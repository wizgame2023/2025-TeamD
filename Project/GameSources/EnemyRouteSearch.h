/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	struct RouteSearch
	{
		enum State {
			NONE,
			OPEN,
			CLOSE
		};

		class Data
		{
			State m_State;
			float m_DistanceToStart;
			float m_DistanceToGoal;
			float m_TotalDistance;
			Vec3 m_ParentPosition;
		public:
			Data() {
				m_State = State::NONE;
				m_DistanceToStart = 0;
				m_DistanceToGoal = 0;
				m_TotalDistance = 0;
				m_ParentPosition = Vec3(0);
			}
			~Data() {};

			void SetDistance(float start, float goal)
			{
				m_DistanceToStart = start;
				m_DistanceToGoal = goal;
				m_TotalDistance = start + goal;
			}

		public:
			shared_ptr<RouteSearch> m_Route;
			Vec3 m_TargetPosition;
			vector<Vec3> points = {};
			Vec3 m_HalfPosition = Vec3(0);
		};
	};
}

//end basecross
