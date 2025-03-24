/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyRouteSearch.h"

namespace basecross {
	Navigate::Navigate(const shared_ptr<Stage>& stage, const shared_ptr<RootPointer>& points, const float& MapWidth, const float& MapHeight) :
		GameObject(stage),
		m_Points(points),
		m_MapWidth(MapWidth),
		m_MapHeight(MapHeight)
	{
		cellData.resize(MapHeight, vector<Data>(MapWidth));
	}
	Navigate::~Navigate()
	{
	}
	void Navigate::OnCreate()
	{
		Dire dire = Z; //縦横どっちに移動するか(true : X軸 false : Y軸)
		m_RootPointer = m_Points->m_RootPointer;
	}

	void Navigate::SetTargetPosition(Vec3 StartPos,Vec3 newTargetPosition)
	{
		m_TargetPosition = newTargetPosition;
		AStarAlgorithm(StartPos, newTargetPosition);
	}

	void Navigate::AStarAlgorithm(Vec3 start, Vec3 goal)
	{
		for (float y = 0; y < m_MapHeight; ++y) {
			for (float x = 0; x < m_MapWidth; ++x) {
				cellData[y][x] = Data();
			}
		}

		cellData[start.z][start.x].m_State = State::CLOSE;
		std::vector<Vec3> OpenSet;
		OpenSet.push_back(start);

		cellData[start.z][start.x].m_State = State::OPEN;
		cellData[start.z][start.x].m_DistanceToStart = 0.0f;
		cellData[start.z][start.x].m_DistanceToGoal = Heuristic(start, goal);
		cellData[start.z][start.x].m_TotalDistance = cellData[start.z][start.x].m_DistanceToGoal;

		while (!OpenSet.empty())
		{
			auto& current = *std::min_element(OpenSet.begin(), OpenSet.end(),
				[&](const Vec3& a, const Vec3& b) {
				int aX = static_cast<int>(a.x);
				int aZ = static_cast<int>(a.z);
				int bX = static_cast<int>(b.x);
				int bZ = static_cast<int>(b.z);
				return cellData[bZ][aX].m_TotalDistance < cellData[bZ][bX].m_TotalDistance;
				});
		

			OpenSet.erase(std::remove(OpenSet.begin(), OpenSet.end(), current), OpenSet.end());
			int currentX = static_cast<int>(current.x);
			int currentZ = static_cast<int>(current.z);

			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dz = -1; dz <= 1; dz++)
				{
					if (dx == 0 && dz == 0) continue;
					if (dx != 0 && dz != 0) continue;

					int neighborX = currentX + dx;
					int neighborZ = currentZ + dz;
					Vec3 neighborNode = Vec3(neighborX, 0.0f, neighborZ );

					// マップの範囲内か確認
					if (neighborX >= 0 && neighborX < m_MapWidth && neighborZ >= 0 && neighborZ < m_MapHeight)
					{
						float newDistanceToStart = cellData[currentZ][currentX].m_DistanceToStart + 1.0f;
						float newDistanceToGoal = Heuristic(neighborNode, goal);
						float newTotalDistance = newDistanceToStart + newDistanceToGoal;

						// まだ探索していないか、またはより良いパスが見つかった場合
						if (cellData[neighborZ][neighborX].m_State == State::NONE || newTotalDistance < cellData[neighborZ][neighborX].m_TotalDistance) {
							OpenSet.push_back(neighborNode);
							cellData[neighborZ][neighborX].m_State = State::OPEN;
							cellData[neighborZ][neighborX].m_DistanceToStart = newDistanceToStart;
							cellData[neighborZ][neighborX].m_DistanceToGoal = newDistanceToGoal;
							cellData[neighborZ][neighborX].m_TotalDistance = newTotalDistance;
							cellData[neighborZ][neighborX].m_ParentPosition = current;
						}
					}
				}
			}
		}
	}
}
//end basecross
