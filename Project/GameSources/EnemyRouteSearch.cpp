/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyRouteSearch.h"
#include <queue>
#include <unordered_set>

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
        m_DireChange = false;
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
            if (m_NaviPoint.size() != 0)
            {
                if ((Position - m_TargetPosition).lengthSqr() >= 1.5f)
                {
                    Vec3 nextPoint = m_NaviPoint.front(); // 次のウェイポイントを取得

                    Vec3 rezult = Vec3();
                    m_Dire = std::abs(Position.x - nextPoint.x) > std::abs(Position.z - nextPoint.z) ? Dire::X : Dire::Z;

                    if (m_Dire == Dire::X)
                    {
                        m_BeforePosition = Position;
                        if (nextPoint.x < Position.x)
                        {
                            return Vec3(-1, 0, 0);
                        }
                        else if (nextPoint.x > Position.x)
                        {
                            return Vec3(1, 0, 0);
                        }
                    }
                    else if (m_Dire == Dire::Z)
                    {
                        m_BeforePosition = Position;
                        if (nextPoint.z < Position.z)
                        {
                            return Vec3(0, 0, -1);
                        }
                        else if (nextPoint.z > Position.z)
                        {
                            return Vec3(0, 0, 1);
                        }
                    }
                }
                else {
                    m_NaviPoint.erase(m_NaviPoint.begin());
                }
            }
        }
        if ((Position - m_TargetPosition).length() < 1.5f)
        {
            m_DireChange = false;
            m_BossPause = false;
            m_Index = m_TargetPosition;
            m_BeforeTarget = m_TargetPosition;
            return Vec3(0, 0, 0);
        }
        return Vec3(0, 0, 0);
    }

    std::vector<Vec3> Navigate::FindPathWithWaypoints(const Vec3& index, const Vec3& goal) {
        auto heuristic = [](const Vec3& a, const Vec3& b) {
            return (b - a).length(); // ゴールへの推定距離を計算するヒューリスティクス関数
            };

        // オープンリスト: 未探索のノードを優先度付きで管理するキュー（ノードインデックスとスコアを保持）
        auto compare = [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
            return a.second > b.second; // スコアが小さい順に処理
            };
        std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, decltype(compare)> openList(compare);

        std::vector<bool> visitedNodes(m_CellData.size(), false); // 全ノードを未探索状態で初期化

        // gCosts: 各ノードに到達するためのコストを格納する配列
        std::vector<float> gCosts(m_CellData.size(), std::numeric_limits<float>::infinity());

        // cameFrom: 各ノードがどのノードから来たのかを追跡する配列（経路復元用）
        std::vector<int> cameFrom(m_CellData.size(), -1);

        // スタート地点のインデックスを取得
        int startIndex = GetIndexFromPosition(index); // 独自関数：Vec3からインデックスを取得
        int goalIndex = GetIndexFromPosition(goal);   // ゴール地点のインデックスを取得

        // 初期状態を設定する
        openList.emplace(startIndex, 0.0f);            // スタート地点をオープンリストに追加
        gCosts[startIndex] = 0.0f;                    // スタート地点のg値を0に設定

        // 探索開始
        while (!openList.empty()) {
            int current = openList.top().first; // オープンリストの最優先ノードを取得（インデックス）
            openList.pop(); // オープンリストから取り出し

            // ゴールに到達した場合、経路を復元して返す
            if (current == goalIndex) {
                std::vector<Vec3> path;
                while (current != -1) { // スタート地点まで辿る
                    path.push_back(m_CellData[current]->GetComponent<Transform>()->GetPosition());
                    current = cameFrom[current]; // 直前のノードに戻る
                }
                reverse(path.begin(), path.end()); // 経路を正しい順序に並べ替え
                return path; // 最終的な経路を返す
            }

            // 近隣ノードを取得
            auto neighbors = GetNeighborsForWaypoints(current);

            for (int neighbor : neighbors) {
                float tentativeG = gCosts[current] +
                    (m_CellData[current]->GetComponent<Transform>()->GetPosition() -
                        m_CellData[neighbor]->GetComponent<Transform>()->GetPosition()).length();

                // 新しいg値が以前の値より小さい場合、または初めて訪問する場合
                if (tentativeG < gCosts[neighbor]) {
                    gCosts[neighbor] = tentativeG; // g値を更新

                    // 探索済みかどうかをチェック
                    if (visitedNodes[current]) {
                        continue; // 探索済みの場合はスキップ
                    }
                    visitedNodes[current] = true; // 探索済みとしてマーク


                    float fCost = tentativeG +
                        heuristic(m_CellData[neighbor]->GetComponent<Transform>()->GetPosition(),
                            m_CellData[goalIndex]->GetComponent<Transform>()->GetPosition()); // f値を計算

                    openList.emplace(neighbor, fCost); // オープンリストに追加
                    cameFrom[neighbor] = current;     // 経路情報を更新
                }
            }
        }

        // ゴールに到達できない場合は空の経路を返す
        return {};
    }

    void Navigate::AStarAlgorithm(Vec3 index, Vec3 goal)
    {
        m_Index = index;
        if (index != goal)
        {
            m_NaviPoint = FindPathWithWaypoints(index, goal);
            m_DireChange = true;
            m_TargetPosition = goal;
        }
    }

    int Navigate::GetIndexFromPosition(const Vec3& position) {

        float memoryPos = 100000000;
        Vec3 outCome = Vec3();
        Vec3 nearPoint = Vec3();
        shared_ptr<RootPointer> nearPointMemory;

        for (int i = 0; i < m_CellData.size(); i++)
        {
            Vec3 vec = m_CellData[i]->GetComponent<Transform>()->GetPosition();
            if (nearPoint == Vec3())
            {
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
            if ((vec - position).length() < (nearPoint - position).length())
            {
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
        }

        for (int i = 0; i < m_CellData.size(); i++) {
            if (m_CellData[i]->GetComponent<Transform>()->GetPosition() == nearPoint) {
                return i;
            }
        }
        return -1; // 該当なしの場合
    }

    // WayPoint対応の近隣ノードを取得する関数
    std::vector<int> Navigate::GetNeighborsForWaypoints(int currentIndex) {
        std::vector<int> neighbors;
        auto waypointIndices = WstrToVecInt(m_CellData[currentIndex]->GetPointerNumber());
        for (int index : waypointIndices) {
            neighbors.push_back(index);
        }
        return neighbors;
    }


    Vec3 Navigate::NextWayPoint(const Vec3& Pos, const Vec3& Target)
    {
        float memoryPos = 100000000;
        Vec3 outCome = Vec3();
        Vec3 nearPoint = Vec3();
        shared_ptr<RootPointer> nearPointMemory;

        for (int i = 0; i < m_CellData.size(); i++)
        {
            Vec3 vec = m_CellData[i]->GetComponent<Transform>()->GetPosition();
            if (nearPoint == Vec3())
            {
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
            if ((vec - Pos).length() < (nearPoint - Pos).length())
            {
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
        }

        vector<int> num;
        wstring nearPossible = nearPointMemory->GetPointerNumber();
        num = WstrToVecInt(nearPossible);
        for (int j = 0; j < num.size(); j++)
        {
            Vec3 nearPos = nearPointMemory->GetPosition();
            Vec3 nearPossibleVec = m_CellData[num[j]]->GetComponent<Transform>()->GetPosition();
           float wayPos = (Target - nearPossibleVec).length() + (Pos - nearPossibleVec).length();

            if (Pos == nearPossibleVec )
            {
                continue;
            }
            if (memoryPos == 100000000)
            {
                memoryPos = wayPos;
            }
            else if (wayPos < memoryPos)
            {
                memoryPos = wayPos;
                outCome = nearPossibleVec;
            }
        }
        return outCome;
    }


    void Navigate::AvoidBlock(const Vec3& Position, const Vec3& Target)
    {
        Vec3 SetPoint = Vec3();
        Vec3 result = Vec3();
        if (m_CellData.size() == 0) return;

        //if (m_BossPause == false)
        //{
        //    
        //    SetPoint = NextWayPoint(Position, Target);

        //}
        SetTargetPosition(Position, Target);
    }
}
//end basecross
