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
            if (m_BeforePosition == Vec3())
            {
                m_BeforePosition = Position;
            }
            if ((Position - m_TargetPosition).lengthSqr() >= 1.5f)
            {
                Vec3 rezult = Vec3();
                m_Dire = std::abs(Position.x - m_TargetPosition.x) > std::abs(Position.z - m_TargetPosition.z) ? Dire::X : Dire::Z;

                //if ((m_BeforePosition - Position).lengthSqr() < 1.0f)
                //{
                //    if (m_Dire == Dire::X) m_Dire = Dire::Z;
                //    else if (m_Dire == Dire::Z) m_Dire = Dire::X;
                //}
                if (m_Dire == Dire::X)
                {
                    m_BeforePosition = Position;
                    if (m_TargetPosition.x < Position.x)
                    {
                        rezult += Vec3(-1, 0, 0);
                        if (m_TargetPosition.z < Position.z)
                        {
                            rezult += Vec3(0, 0, -1);
                        }
                        else if (m_TargetPosition.z > Position.z)
                        {
                            rezult += Vec3(0, 0, 1);
                        }
                    }
                    else if (m_TargetPosition.x > Position.x)
                    {
                        rezult += Vec3(1, 0, 0);
                        if (m_TargetPosition.z < Position.z)
                        {
                            rezult += Vec3(0, 0, -1);
                        }
                        else if (m_TargetPosition.z > Position.z)
                        {
                            rezult += Vec3(0, 0, 1);
                        }

                    }
                }
                else if (m_Dire == Dire::Z)
                {
                    m_BeforePosition = Position;
                    if (m_TargetPosition.z < Position.z)
                    {
                        rezult += Vec3(0, 0, -1);
                        if (m_TargetPosition.x < Position.x)
                        {
                            rezult += Vec3(-1, 0, 0);
                        }
                        else if (m_TargetPosition.x > Position.x)
                        {
                            rezult += Vec3(1, 0, 0);
                        }

                    }
                    else if (m_TargetPosition.z > Position.z)
                    {
                        rezult += Vec3(0, 0, 1);
                        if (m_TargetPosition.x < Position.x)
                        {
                            rezult += Vec3(-1, 0, 0);
                        }
                        else if (m_TargetPosition.x > Position.x)
                        {
                            rezult += Vec3(1, 0, 0);
                        }

                    }
                }
                return rezult;
            }
            else{
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


    Vec3 Navigate::AvoidBlock(const Vec3& Position, const Vec3& Target)
    {
        Vec3 SetPoint = Vec3();
        Vec3 SetPoint1 = Vec3();
        auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
        auto pointers = pointerGroup->GetGroupVector();
        for (auto& point : pointers)
        {
            auto Obj = point.lock();
            Vec3 pointVec = Obj->GetComponent<Transform>()->GetPosition();
            if (SetPoint == Vec3())
            {
                SetPoint = pointVec;
            }
            if ((Target - pointVec).length() < (Target - SetPoint).length())
            {
                SetPoint = pointVec;
            }
            else if((Target - pointVec).length() < (Target - SetPoint).length())
            {
                SetPoint1 = pointVec;
            }
        }

        if ((Position - SetPoint).lengthSqr() >= 1.0f)
        {
            Vec3 pointTarget = Vec3();
            m_Dire = std::abs(Position.x - SetPoint.x) > std::abs(Position.z - SetPoint.z) ? Dire::X : Dire::Z;
            if (m_Dire == Dire::X)
            {
                m_BeforePosition = Position;
                if (SetPoint.x < Position.x)
                {
                    pointTarget += Vec3(-1, 0, 0);
                }
                else if (SetPoint.x > Position.x)
                {
                    pointTarget += Vec3(1, 0, 0);
                }
            }
            else if (m_Dire == Dire::Z)
            {
                m_BeforePosition = Position;
                if (SetPoint.z < Position.z)
                {
                    pointTarget += Vec3(0, 0, -1);
                }
                else if (SetPoint.z > Position.z)
                {
                    pointTarget += Vec3(0, 0, 1);
                }
            }
            return pointTarget;
        }
        else {
            return Vec3(0);
        }
    }
}
//end basecross
