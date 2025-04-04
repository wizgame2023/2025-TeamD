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
            auto Obj = dynamic_pointer_cast<RootPointer>(shObj);
            m_CellData.push_back(Obj);
        }
        // 開始位置の初期化 (例: マップの中心)
        m_StartPosition = Vec3(m_MapWidth / 2.0f, 0.0f, m_MapHeight / 2.0f);
        m_Index = m_StartPosition;
        m_DireChange = true;
        m_Dire = Dire::X;
        m_BeforePosition = Vec3();
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
            if ((Position - m_TargetPosition).lengthSqr() >= 1.5f)
            {
                Vec3 rezult = Vec3();
                m_Dire = std::abs(Position.x - m_TargetPosition.x) > std::abs(Position.z - m_TargetPosition.z) ? Dire::X : Dire::Z;

                if (m_Dire == Dire::X)
                {
                    m_BeforePosition = Position;
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
                    m_BeforePosition = Position;
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
            else {
                m_DireChange = false;
            }
        }
        if ((Position - m_TargetPosition).length() < 1.5f)
        {
            m_Index = m_TargetPosition;
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
        if (index != goal)
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


    Vec3 Navigate::NextWayPoint(const Vec3& Pos, const Vec3& Target)
    {
        float memoryPos = 0;
        Vec3 outCome = Vec3();
        for (int i = 0; i < m_CellData.size(); i++)
        {
            wstring num = m_CellData[i]->GetPointerNumber();
            vector<shared_ptr<RootPointer>> number = m_CellData[i]->GetRootPointer();
            
            vector<Vec3> objVec = {};
            for (int f = 0; f < number.size(); f++)
            {
                Vec3 vec = number[f]->GetComponent<Transform>()->GetPosition();
                objVec.push_back(vec);
            }

            for (int j = 0; j < objVec.size(); j++)
            {
                float wayPos = (objVec[j] - Target).length() + (objVec[j] - Pos).length();

                if (Pos == objVec[j])
                {
                    continue;
                }
                if (memoryPos == 0)
                {
                    memoryPos = wayPos;
                }
                else if (wayPos < memoryPos)
                {
                    memoryPos = wayPos;
                    outCome = objVec[j];
                }
            }
        }
        return outCome;
    }


    Vec3 Navigate::AvoidBlock(const Vec3& Position, const Vec3& Target)
    {
        Vec3 SetPoint = Vec3();
        Vec3 SetPoint1 = Vec3();
        Vec3 result = Vec3();
        SetPoint1 = NextWayPoint(Position, Target);
        SetTargetPosition(Position, SetPoint1);
        result = GetAStarForword(Position);
        return result;
    }
}
//end basecross
