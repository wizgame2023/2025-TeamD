/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyRouteSearch.h"

namespace basecross {
	Navigate::Navigate(const shared_ptr<Stage>& stage, vector<shared_ptr<RootPointer>> points):
		GameObject(stage),
		m_Points(points)
	{
	}
	Navigate::~Navigate()
	{
	}
	void Navigate::OnCreate()
	{
	}
	void Navigate::OnUpdate()
	{
	}
	void Navigate::SetTargetPosition(Vec3 newTargetPosition)
	{
		m_TargetPosition = newTargetPosition;
		AStarAlgorithm();
	}
	void Navigate::AStarAlgorithm()
	{
	}
	void Navigate::OpenCell(Vec3 index)
	{
		
	}
	bool Navigate::UpdateDistaince(Vec3 index)
	{
       return false;
    }
}
//end basecross
