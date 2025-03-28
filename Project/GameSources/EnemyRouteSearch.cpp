/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyRouteSearch.h"

namespace basecross {
    Navigate::Navigate(const std::shared_ptr<GameObject>& GameObjectPtr) :
        Component(GameObjectPtr)
    {
        m_MapWidth = 100.0f;
        m_MapHeight = 100.0f;
        //セルのサイズを10x10とする
        auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
        auto pointers = pointerGroup->GetGroupVector();
        for (auto point : pointers)
        {
            auto shObj = point.lock();
            m_CellData.push_back(shObj);
        }
        // 開始位置の初期化 (例: マップの中心)
        m_StartPosition = Vec3(m_MapWidth / 2.0f, 0.0f, m_MapHeight / 2.0f);
        m_Index = m_StartPosition;
        m_DireChange = true;
        m_Dire = Dire::X;
    }

    Navigate::~Navigate()
    {
    }

    void Navigate::SetTargetPosition(const Vec3& Position, const Vec3& target)
    {

        m_TargetPosition = target;
        AStarAlgorithm(Position, target);

    }

    Vec3 Navigate::GetAStarForword(const Vec3 Position)
    {
        if (m_DireChange)
        {
            m_Dire = std::abs(Position.x - m_TargetPosition.x) > std::abs(Position.y - m_TargetPosition.y) ? Dire::X : Dire::Z;

            if (m_Dire == Dire::X)
            {
                if (m_TargetPosition.x < Position.x)
                {
                    return Vec3(-1, 0, 0);
                }
                else if (m_TargetPosition.x > Position.x)
                {
                    return Vec3(1, 0, 0);
                }
            }
            else if (m_Dire == Dire::Z)
            {
                if (m_TargetPosition.z < Position.z)
                {
                    return Vec3(0, 0, -1);
                }
                else if (m_TargetPosition.z > Position.z)
                {
                    return Vec3(0, 0, 1);
                }
            }
        }
        if ((Position - m_TargetPosition).length() < 3.0f)
        {
            m_Index = m_TargetPosition;
            m_DireChange = false;
            m_BeforeTarget = m_TargetPosition;
            return Vec3(0, 0, 0);
        }
        return Vec3(0, 0, 0);
    }

    void Navigate::AStarAlgorithm(Vec3 index, Vec3 goal)
    {
        // 開始位置をA*アルゴリズムの開始点として設定
        m_Index = index;

        // 開始ノードの距離を初期化
        // 現在のノードの周囲のセルをOPENにする
        if (index != goal && m_BeforeTarget != goal)
        {
            m_DireChange = true;
            m_TargetPosition = goal;
        }
        else {
            return;
        }
    }


    Vec3 Navigate::OpenCell(Vec3 index)
    {
        Vec3 currentIndex = Vec3(0);
        for (int i = 0; i < m_CellData.size(); i++)
        {
            Vec3 pos = m_CellData[i]->GetComponent<Transform>()->GetPosition();

            if (currentIndex == Vec3(0))
            {
                currentIndex = pos;
            }
            else if ((index - pos).length() < (index - currentIndex).length() && index != pos && m_BeforeTarget != pos)
            {
                currentIndex = pos;
            }
        }
        if (UpdateDistance(currentIndex));  // ここでは更新だけを行う。OPENリストへの追加はAStarAlgorithmで行う。
        {
            return currentIndex;
        }
    }

    bool Navigate::UpdateDistance(Vec3 index)
    {

        return true;
    }
}
//end basecross
