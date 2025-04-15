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
                    Vec3 direction = (nextPoint - Position);
                    return direction.normalize();;
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

    std::vector<Vec3> Navigate::FindPathWithWaypoints(const shared_ptr<RootPointer>& pointer, const Vec3& goal) {
        auto heuristic = [](const Vec3& a, const Vec3& b) {
            return (b - a).length(); // ゴールへの推定距離を計算するヒューリスティクス関数
            };

        std::vector<std::shared_ptr<RootPointer>> openPointers;
        openPointers.reserve(m_CellData.size()); // 事前にメモリ確保

        std::vector<bool> isClosed(m_CellData.size(), false); // 全ノードを未探索状態で初期化

        // gCosts: 各ノードに到達するためのコストを格納する配列
        std::vector<float> gCosts(m_CellData.size(), std::numeric_limits<float>::infinity());

        // 各ノードから目標ノードまでの推定コスト (ヒューリスティックコスト, hCost)
        std::vector<float> hCosts(m_CellData.size(), std::numeric_limits<float>::infinity());

        // 各ノードの推定総コスト (tCost = gCost + hCost)
        std::vector<float> tCost(m_CellData.size(), std::numeric_limits<float>::infinity());

        // cameFrom: 各ノードがどのノードから来たのかを追跡する配列（経路復元用）
        std::vector<int> cameFrom(m_CellData.size(), -1);

        // 各ノードへの最短経路における親ノードを記録 (経路再構築用)
        std::vector<std::shared_ptr<RootPointer>> parentPointer(m_CellData.size(), nullptr);

        // 目標位置に対応するノードインデックスとポインタを取得
        int goalIndex = GetIndexFromPosition(goal);
        // ゴールインデックスが無効な場合は空のパスを返す
        if (goalIndex < 0 || goalIndex >= m_CellData.size()) {
            return {};
        }
        auto goalPointer = m_CellData[goalIndex];


        auto current = pointer;
        int currentNumber = current->GetNumber();

        gCosts[currentNumber] = 0.0f;
        hCosts[currentNumber] = (goalPointer->GetPosition() - current->GetPosition()).length();
        tCost[currentNumber] = gCosts[currentNumber] + hCosts[currentNumber];


        bool isGoal = false; // ゴールに到達したかどうかのフラグ
        if (currentNumber == goalIndex) {
            isGoal = true;
        }


        //// 初期状態を設定する
        //openList.emplace(startIndex, 0.0f);            // スタート地点をオープンリストに追加
        //gCosts[startIndex] = 0.0f;                    // スタート地点のg値を0に設定

        // 探索開始
        do {
            //int current = openList.top().first; // オープンリストの最優先ノードを取得（インデックス）
            //openList.pop(); // オープンリストから取り出し

            isClosed[currentNumber] = true; // 探索済みとしてマーク
            auto neghbors = current->GetRootPointer();

            // 近隣ノードを取得

            for (const auto& neighbor : neghbors) {
                int neigborNumber = neighbor->GetNumber();
                if (isClosed[neigborNumber]) {
                    continue;
                }

                float tempG = gCosts[currentNumber] + (current->GetPosition() - neighbor->GetPosition()).length();

                float tempH = (goalPointer->GetPosition() - neighbor->GetPosition()).length();

                float tempT = tempG + tempH;
                // ゴールに到達した場合、経路を復元して返す
                if (neigborNumber == goalIndex) {
                    parentPointer[neigborNumber] = current; // ゴールの親を設定
                    gCosts[neigborNumber] = tempG;           // ゴールのコストを記録 (経路長が必要な場合など)
                    hCosts[neigborNumber] = tempH;           // （記録は任意）
                    tCost[neigborNumber] = tempT;           // （記録は任意）
                    isGoal = true;                           // ゴールフラグを立てる
                    break;
                }

                bool isOpenList = false;
                for (const auto& openNode : openPointers) {
                    if (openNode->GetNumber() == neigborNumber) {
                        isOpenList = true;
                        break;
                    }
                }

                if (!isOpenList && parentPointer[neigborNumber] == nullptr)
                {
                    // コストと親ポインタを設定
                    gCosts[neigborNumber] = tempG;
                    hCosts[neigborNumber] = tempH;
                    tCost[neigborNumber] = tempT;
                    parentPointer[neigborNumber] = current;
                    // オープンリストに追加
                    openPointers.emplace_back(neighbor);
                }
                else if (tempT < tCost[neigborNumber])
                {
                    // コストと親ポインタを設定
                    gCosts[neigborNumber] = tempG;
                    hCosts[neigborNumber] = tempH;
                    tCost[neigborNumber] = tempT;
                    parentPointer[neigborNumber] = current;
                }
            }

            if (!isGoal && !openPointers.empty())
            {
                shared_ptr<RootPointer> minCostPoint = nullptr;
                int minIndex = -1;
                float minTCost = std::numeric_limits<float>::infinity();

                for (int i = 0; i < openPointers.size(); ++i) {
                    int nodeNum = openPointers[i]->GetNumber();
                    if (tCost[nodeNum] < minTCost) {
                        minTCost = tCost[nodeNum];
                        minCostPoint = openPointers[i];
                        minIndex = i;
                    }
                }

                current = minCostPoint;
                currentNumber = current->GetNumber();

                if (minIndex != -1)
                {
                    openPointers.erase(openPointers.begin() + minIndex);
                }
                else {
                    break;
                }
            }
            else if (!isGoal && openPointers.empty())
            {
                break;
            }

        } while (!openPointers.empty() && !isGoal);


        // ゴールが見つからなかった場合、空のパスを返す
        if (!isGoal) {
            // std::cout << "Path not found." << std::endl; // デバッグ用出力
            return {};
        }
        else {
            vector<Vec3> path = {};
            // ゴールノードから開始して親ポインタを辿って経路を構築
            std::shared_ptr<RootPointer> pathPointer = goalPointer;
            const int MAX_PATH_COUNT = 1000; // 無限ループ防止のための最大経路長
            int count = 0;
            do {
                if (pathPointer == nullptr) { // 親が nullptr になるのは開始ノードに到達した時
                    break;
                }
                int number = pathPointer->GetNumber();
                path.push_back(pathPointer->GetPosition()); // 現在のノード位置をパスに追加
                pathPointer = parentPointer[number];        // 親ノードへ移動
                count++;
                // 無限ループ防止
                if (count > MAX_PATH_COUNT) {
                    // std::cerr << "Error: Path reconstruction exceeded MAX_PATH_COUNT." << std::endl;
                    return {}; // エラーとして空のパスを返す
                }
            } while (true); // ループは pathPointer が nullptr になった時に break で抜ける

            // パスはゴールからスタートに向かって構築されたので、逆順にする
            std::reverse(path.begin(), path.end());

            // 構築されたパスを返す
            return path;
        }
        // 通常、ここには到達しないはず
        // return {};
    }

    std::vector<Vec3> Navigate::FindPathWithWaypoints2(const shared_ptr<RootPointer>& pointer, const Vec3& goal) {

        // オープンリスト: これから評価するノードのリスト
        std::vector<std::shared_ptr<RootPointer>> openPointers;
        openPointers.reserve(m_CellData.size()); // 事前にメモリ確保

        // クローズドリスト: 評価済みのノードをマーク (インデックスで管理)
        std::vector<bool> isClosed(m_CellData.size(), false);

        // 各ノードまでの開始ノードからの実際のコスト (gCost)
        std::vector<float> gCost(m_CellData.size(), std::numeric_limits<float>::infinity()); // 無限大で初期化 (-1.0f より適切)

        // 各ノードから目標ノードまでの推定コスト (ヒューリスティックコスト, hCost)
        std::vector<float> hCost(m_CellData.size(), std::numeric_limits<float>::infinity());

        // 各ノードの推定総コスト (tCost = gCost + hCost)
        std::vector<float> tCost(m_CellData.size(), std::numeric_limits<float>::infinity());

        // 各ノードへの最短経路における親ノードを記録 (経路再構築用)
        std::vector<std::shared_ptr<RootPointer>> parentPointer(m_CellData.size(), nullptr);

        // 最終的な経路を格納するベクター
        std::vector<Vec3> path;

        // 目標位置に対応するノードインデックスとポインタを取得
        int goalIndex = GetIndexFromPosition(goal);
        // ゴールインデックスが無効な場合は空のパスを返す
        if (goalIndex < 0 || goalIndex >= m_CellData.size()) {
            // std::cerr << "Error: Goal position not found or invalid." << std::endl; // エラーログ推奨
            return {};
        }
        auto goalPointer = m_CellData[goalIndex];

        // --- 開始ノードの設定 ---
        auto current = pointer;
        int currentNumber = current->GetNumber();

        // 開始ノードのコストを設定
        gCost[currentNumber] = 0.0f; // 開始地点までのコストは0
        hCost[currentNumber] = (goalPointer->GetPosition() - current->GetPosition()).length(); // ヒューリスティックコスト(直線距離)
        tCost[currentNumber] = gCost[currentNumber] + hCost[currentNumber]; // 総コスト

        // 開始ノードをオープンリストに追加（最初のループで処理されるため、直接追加は不要）
        // openPointers.push_back(current); // この実装ではdo-while開始時にcurrentが設定される

        bool isGoal = false; // ゴールに到達したかどうかのフラグ

        // 開始ノードが既にゴールの場合
        if (currentNumber == goalIndex) {
            isGoal = true;
        }

        // --- A* 探索メインループ ---
        // オープンリストが空になるか、ゴールが見つかるまで繰り返す
        do {
            // 現在のノードを評価済みとしてクローズドリストに追加
            isClosed[currentNumber] = true;

            // 現在のノードの隣接ノードを取得
            auto neighbors = current->GetRootPointer();

            // 隣接ノードを順に評価
            for (const auto& neighbor : neighbors) {
                int neighborNumber = neighbor->GetNumber();

                // 隣接ノードが既に評価済み(クローズドリストにある)場合はスキップ
                if (isClosed[neighborNumber]) {
                    continue;
                }

                // --- コスト計算 ---
                // 現在のノードを経由して隣接ノードに到達するコスト (tempG)
                float costToNeighbor = (neighbor->GetPosition() - current->GetPosition()).length();
                float tempG = gCost[currentNumber] + costToNeighbor;

                // 隣接ノードからゴールまでのヒューリスティックコスト (tempH)
                float tempH = (goalPointer->GetPosition() - neighbor->GetPosition()).length();

                // 推定総コスト (tempT)
                float tempT = tempG + tempH;

                // --- ゴールチェック ---
                // 隣接ノードがゴールの場合
                if (neighborNumber == goalIndex) {
                    parentPointer[neighborNumber] = current; // ゴールの親を設定
                    gCost[neighborNumber] = tempG;           // ゴールのコストを記録 (経路長が必要な場合など)
                    hCost[neighborNumber] = tempH;           // （記録は任意）
                    tCost[neighborNumber] = tempT;           // （記録は任意）
                    isGoal = true;                           // ゴールフラグを立てる
                    break; // 隣接ノードのループを抜ける (ゴールが見つかったため)
                }

                // --- オープンリスト/コスト更新 ---
                // この隣接ノードが既にオープンリストにあるか、または過去にあったか (parentPointer が設定されているかで判断)
                // より効率的なのは、オープンリストに存在するかどうかを直接チェックすること
                bool inOpenList = false;
                for (const auto& openNode : openPointers) {
                    if (openNode->GetNumber() == neighborNumber) {
                        inOpenList = true;
                        break;
                    }
                }

                // まだオープンリストになく、親も設定されていない場合（＝初めて見つけたノード）
                if (!inOpenList && parentPointer[neighborNumber] == nullptr) {
                    // コストと親ポインタを設定
                    gCost[neighborNumber] = tempG;
                    hCost[neighborNumber] = tempH;
                    tCost[neighborNumber] = tempT;
                    parentPointer[neighborNumber] = current;
                    // オープンリストに追加
                    openPointers.emplace_back(neighbor);
                }
                // 既にオープンリストにあるか、過去にあったノードの場合
                else if (tempT < tCost[neighborNumber]) {
                    // 今回計算した経路の方がコストが低い場合 (緩和処理)
                    // コストと親ポインタを更新
                    gCost[neighborNumber] = tempG;
                    hCost[neighborNumber] = tempH;
                    tCost[neighborNumber] = tempT;
                    parentPointer[neighborNumber] = current;
                    // priority_queue を使う場合は、ここで優先度を更新する必要がある
                }
            } // 隣接ノードのループ終了

            // ゴールに到達していなければ、次のノードを選択
            if (!isGoal && !openPointers.empty()) {
                // --- オープンリストから最小コストのノードを選択 ---
                // (注: この線形探索は非効率。priority_queue の使用を推奨)
                std::shared_ptr<RootPointer> minCostPointer = nullptr;
                int minIndex = -1;
                float minTCost = std::numeric_limits<float>::infinity();

                for (int i = 0; i < openPointers.size(); ++i) {
                    int nodeNum = openPointers[i]->GetNumber();
                    if (tCost[nodeNum] < minTCost) {
                        minTCost = tCost[nodeNum];
                        minCostPointer = openPointers[i];
                        minIndex = i;
                    }
                }

                // 最小コストのノードを次のカレントノードに設定
                current = minCostPointer;
                currentNumber = current->GetNumber();

                // 選択したノードをオープンリストから削除
                // (注: vector::erase は非効率。priority_queue なら pop() で済む)
                if (minIndex != -1) {
                    openPointers.erase(openPointers.begin() + minIndex);
                }
                else {
                    // ここに来る場合、オープンリストが空か、何らかのエラー
                    // std::cerr << "Error: Could not find minimum cost node in open list." << std::endl;
                    break; // ループを抜ける
                }
            }
            else if (!isGoal && openPointers.empty()) {
                // オープンリストが空になったのにゴールが見つからない場合、ループを終了
                break;
            }

        } while (!openPointers.empty() && !isGoal); // ループ条件

        // --- 経路の再構築 ---

        // ゴールが見つからなかった場合、空のパスを返す
        if (!isGoal) {
            // std::cout << "Path not found." << std::endl; // デバッグ用出力
            return {};
        }
        else {
            // ゴールノードから開始して親ポインタを辿って経路を構築
            std::shared_ptr<RootPointer> pathPointer = goalPointer;
            const int MAX_PATH_COUNT = 1000; // 無限ループ防止のための最大経路長
            int count = 0;
            do {
                if (pathPointer == nullptr) { // 親が nullptr になるのは開始ノードに到達した時
                    break;
                }
                int number = pathPointer->GetNumber();
                path.push_back(pathPointer->GetPosition()); // 現在のノード位置をパスに追加
                pathPointer = parentPointer[number];        // 親ノードへ移動
                count++;
                // 無限ループ防止
                if (count > MAX_PATH_COUNT) {
                    // std::cerr << "Error: Path reconstruction exceeded MAX_PATH_COUNT." << std::endl;
                    return {}; // エラーとして空のパスを返す
                }
            } while (true); // ループは pathPointer が nullptr になった時に break で抜ける

            // パスはゴールからスタートに向かって構築されたので、逆順にする
            std::reverse(path.begin(), path.end());

            // 構築されたパスを返す
            return path;
        }
        // 通常、ここには到達しないはず
        // return {};
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

    shared_ptr<RootPointer>  Navigate::GetNearPinter(const Vec3& position) {
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
        return nearPointMemory;
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

            if (Pos == nearPossibleVec)
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
