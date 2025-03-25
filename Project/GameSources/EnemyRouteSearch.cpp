/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyRouteSearch.h"

namespace basecross {
	Navigate::Navigate(const shared_ptr<GameObject>& GameObjectPtr) :
	Component(GameObjectPtr)
	{
		m_MapWidth  = 100.0f;
		m_MapHeight = 100.0f;
		m_CellData.resize(m_MapWidth / 10.0f,vector<Data>(m_MapHeight / 10.0f));
	}
	Navigate::~Navigate()
	{
	}

	void Navigate::SetTargetPosition(Vec3 newTargetPosition)
	{
		m_TargetPosition = newTargetPosition;
		AStarAlgorithm( newTargetPosition);
	}

	void Navigate::AStarAlgorithm(Vec3 goal)
	{
		while (m_Index != goal)
		{
			OpenCell(m_Index);
			Vec3 min = Vec3(-1.0f, -1.0f, -1.0f);
			int updateNum = 0;
			for (int z = 0; z < m_CellData.size(); z++)
			{
				for (int x = 0; x < m_CellData.max_size(); x++)
				{
					if (m_CellData[x][z].m_State == State::OPEN)
					{
						updateNum++;
						if (min == Vec3(-1, -1, -1))
						{
							min = Vec3(x, 0, z);
						}
						else if (m_CellData[min.x][min.z].m_TotalDistance > m_CellData[x][z].m_TotalDistance)
						{
							min = Vec3(x, 0, z);
						}
					}
				}
			}

			m_Index = min;
		}

		while (m_Index != m_StartPosition)
		{
			points.push(m_Index);
			m_Index = m_CellData[m_Index.x][m_Index.z].m_ParentPosition;
			m_HalfPosition = points.top();
		}
	}
	void Navigate::OpenCell(Vec3 index)
	{
		float x = 5.0f;
		float z = 5.0f;
		vector<Vec3> openIndex
		{
			Vec3(index.x + x, index.y, index.z),
			Vec3(index.x - x, index.y, index.z),
			Vec3(index.x, index.y, index.z + z),
			Vec3(index.x, index.y, index.z - z)
		};

		for (int i = 0; i < openIndex.max_size(); i++)
		{
			if (UpdateDistance(openIndex[i]))
			{
				m_CellData[openIndex[i].x][openIndex[i].z].m_ParentPosition = index;
			}
		}
	}


	bool Navigate::UpdateDistance(Vec3 index)
	{
		if (index.x >= m_MapWidth || index.x < 0) return false; // 幅の範囲外
		if (index.y >= m_MapHeight || index.y < 0) return false; // 高さの範囲外	

		Data& cellDate = m_CellData[index.x][index.z];


		//// セルが壁またはプレイヤーによってブロックされているか確認
		//// MasterData がシングルトンで、tileDate と playerCell が 2 次元配列であると仮定
		//if (MasterData::GetInstance()->tileDate[index.y][index.x].isEnable ||
		//	MasterData::GetInstance()->playerCell[index.y][index.x] != 0)
		//{
		//	// セルが壁の場合、通過を避けるために大きな totalDistance を設定
		//	usingCellDate.totalDistance = 10000.0f;
		//	return false; // 壁であるため、距離が更新されなかったことを示す
		//}

		if (cellDate.m_State != State::CLOSE)
		{
			if (cellDate.m_State != State::NONE)
			{
				cellDate.m_State = State::OPEN;
				float distanceX = std::abs(m_TargetPosition.x - index.x);
				float distanceZ = std::abs(m_TargetPosition.z - index.z);

				cellDate.SetDistance(distanceX, distanceZ);

				return true;
			}
			else {
				float goalDistance = std::abs(m_TargetPosition.z - index.z);
				float startDistance = std::abs(m_TargetPosition.x - index.x);

				float newDistance = goalDistance + startDistance;
				if (cellDate.m_TotalDistance > newDistance)
				{
					cellDate.SetDistance(startDistance, goalDistance);

					return true;
				}
				else {
					return false;
				}
			}
		}
		return false;
	}
}
//end basecross
