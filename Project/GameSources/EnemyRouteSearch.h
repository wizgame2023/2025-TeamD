/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Navigate : public GameObject {
		enum State {
			NONE,
			OPEN,
			CLOSE
		};

		class Data
		{
		public:

			State m_State;
			float m_DistanceToStart;
			float m_DistanceToGoal;
			float m_TotalDistance;
			Vec3 m_ParentPosition;
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
		};

		Data data;
		vector<shared_ptr<RootPointer>> m_Points;
		Vec3 m_TargetPosition;
		Vec3 m_HalfPosition = Vec3(0);
	public:
		Navigate(const shared_ptr<Stage>& stage, vector<shared_ptr<RootPointer>> points);
		~Navigate();

		virtual void OnCreate();
		virtual void OnUpdate();
		void SetTargetPosition(Vec3 newTargetPosition);
		void AStarAlgorithm();
		void OpenCell(Vec3 index);
		bool UpdateDistaince(Vec3 index);

	};
}

//end basecross
