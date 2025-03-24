/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class RootPointer;
	class Navigate : public GameObject {
		enum State {
			NONE,
			OPEN,
			CLOSE
		};

		struct Data
		{
		public:

			State m_State;
			float m_DistanceToStart;
			float m_DistanceToGoal;
			float m_TotalDistance;
			Vec3 m_ParentPosition;

			void SetDistance(float start, float goal)
			{
				m_DistanceToStart = start;
				m_DistanceToGoal = goal;
				m_TotalDistance = start + goal;
			}
		};

		vector<vector<Data>> cellData;		
		Vec3 m_TargetPosition;
		Vec3 m_HalfPosition = Vec3(0);
		deque<Vec3> path;

		shared_ptr<RootPointer> m_Points;
		vector<shared_ptr<RootPointer>> m_RootPointer;
		float m_MapWidth;
		float m_MapHeight;
		/*移動時用変数*/
		bool direChange = true; //移動方向を変更するタイミングか
		enum Dire
		{
			X, Z
		};

	public:
		Navigate(const shared_ptr<Stage>& stage, const shared_ptr<RootPointer>& points, const float& MapWidth, const float& MapHeight);
		~Navigate();

		virtual void OnCreate();

		void SetTargetPosition(Vec3 StartPos, Vec3 newTargetPosition);
		void AStarAlgorithm(Vec3 start, Vec3 goal);

		float Heuristic(Vec3 a, Vec3 b)
		{
			return std::abs(a.x - b.x) + std::abs(a.z - b.z); // XZ平面のみの距離
		}

		// Vec3同士が同じ位置にあるかをチェックする関数
		bool sSamePosition(Vec3 a, Vec3 b) {
			float tolerance = 0.1f;
			return (std::abs(a.x - b.x) < tolerance && std::abs(a.z - b.z) < tolerance);
		}
	};
}

//end basecross
