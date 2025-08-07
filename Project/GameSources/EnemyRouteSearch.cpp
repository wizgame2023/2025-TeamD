/*!
 @file EnemyRouteSearch.cpp
 @brief A*アルゴリズムを用いた敵のルート探索コンポーネント定義
*/


#include "stdafx.h"
#include "Project.h"
#include <queue>
#include <unordered_set>

namespace basecross {
    // コンストラクタ: マップ情報およびナビゲーションポイントを初期化
    Navigate::Navigate(const std::shared_ptr<GameObject>& GameObjectPtr)
        : Component(GameObjectPtr)
    {
        // マップの横幅・縦幅を設定
        m_MapWidth = 100.0f;
        m_MapHeight = 100.0f;

        // StageBuilderからPointerGroupを取得し、RootPointerオブジェクトをセルデータに格納
        auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
        auto pointers = pointerGroup->GetGroupVector();
        for (auto point : pointers)
        {
            auto shObj = point.lock();
            auto Obj = dynamic_pointer_cast<RootPointer>(shObj);
            if (Obj)
            {
                m_CellData.push_back(Obj);
            }
        }

        // 開始位置をマップ中央に設定
        m_StartPosition = Vec3(m_MapWidth / 2.0f, 0.0f, m_MapHeight / 2.0f);
        m_Index = m_StartPosition;   // 現在セルインデックス初期化
        m_DireChange = false;             // 方向転換フラグ初期化
        m_Dire = Dire::X;           // 初期移動軸はX方向
        m_BeforePosition = Vec3();            // 直前位置は空ベクトル
    }

    // デストラクタ（特にリソース解放処理はなし）
    Navigate::‾Navigate()
    {
    }

    // 目標位置だけを更新（開始位置はSetTargetPosition呼び出し元で保持／更新を想定）
    void Navigate::SetTargetPosition(const Vec3& Position, const Vec3& target)
    {
        // 引数Positionは未使用（必要ならm_StartPositionを更新する実装に変更）
        m_TargetPosition = target;
    }

    // Waypoint経由でA*探索を行い、Vec3リストを返す
    std::vector<Vec3> Navigate::FindPathWithWaypoints(
        const std::shared_ptr<RootPointer>& pointer,
        const Vec3& goal)
    {
        // ヒューリスティック関数: 現在ノードからゴールまでの距離を計算
        auto heuristic = [](const Vec3& a, const Vec3& b) {
            return (b - a).length();
            };

        // オープンリスト: 未展開ノードを一時的に保持
        std::vector<std::shared_ptr<RootPointer>> openPointers;
        openPointers.reserve(m_CellData.size());

        // クローズドリスト: 探索済みフラグ
        std::vector<bool> isClosed(m_CellData.size(), false);

        // gCosts: スタートから各ノードへの実コスト
        std::vector<float> gCosts(
            m_CellData.size(), std::numeric_limits<float>::infinity());

        // hCosts: 各ノードからゴールへの推定コスト
        std::vector<float> hCosts(
            m_CellData.size(), std::numeric_limits<float>::infinity());

        // tCost: 合計コスト = gCosts + hCosts
        std::vector<float> tCost(
            m_CellData.size(), std::numeric_limits<float>::infinity());

        // parentPointer: 経路復元用に各ノードの親ノードを記録
        std::vector<std::shared_ptr<RootPointer>> parentPointer(
            m_CellData.size(), nullptr);

        // 目標位置に対応するセルインデックスを計算
        int goalIndex = GetIndexFromPosition(goal);
        if (goalIndex < 0 || goalIndex >= (int)m_CellData.size())
        {
            // 無効なゴールなら空パス
            return {};
        }
        auto goalPointer = m_CellData[goalIndex];

        // 探索開始ノードを初期化
        auto current = pointer;
        int  currentNumber = current->GetNumber();
        gCosts[currentNumber] = 0.0f;
        hCosts[currentNumber] = heuristic(current->GetPosition(), goalPointer->GetPosition());
        tCost[currentNumber] = gCosts[currentNumber] + hCosts[currentNumber];

        bool isGoal = (currentNumber == goalIndex);

        // メインループ: オープンリストが空になるかゴール到達まで繰り返し
        while (!openPointers.empty() || !isGoal)
        {
            // 現在ノードをクローズドリストに移動
            isClosed[currentNumber] = true;

            // 現在ノードの隣接ノードを取得
            auto neighbors = current->GetRootPointer();
            for (auto& neighbor : neighbors)
            {
                int nbNum = neighbor->GetNumber();

                // 既に探索済み（クローズド）ならスキップ
                if (isClosed[nbNum]) continue;

                // gCostの暫定値 = 現在ノードへのコスト + 隣接ノード間の距離
                float tempG = gCosts[currentNumber]
                    + (current->GetPosition() - neighbor->GetPosition()).length();
                // hCostの暫定値 = 隣接ノードからゴールまでの直線距離
                float tempH = heuristic(neighbor->GetPosition(), goalPointer->GetPosition());
                float tempT = tempG + tempH;

                // 隣接ノードがゴールノードかどうかチェック
                if (nbNum == goalIndex)
                {
                    // ゴール到達時は親ノード等を記録してループを抜ける
                    parentPointer[nbNum] = current;
                    gCosts[nbNum] = tempG;
                    hCosts[nbNum] = tempH;
                    tCost[nbNum] = tempT;
                    isGoal = true;
                    break;
                }

                // オープンリストに既に登録済みかをチェック
                bool inOpen = false;
                for (auto& o : openPointers)
                {
                    if (o->GetNumber() == nbNum)
                    {
                        inOpen = true;
                        break;
                    }
                }

                if (!inOpen && parentPointer[nbNum] == nullptr)
                {
                    // 初めて訪れるノード: コストと親を設定しオープンリストに追加
                    gCosts[nbNum] = tempG;
                    hCosts[nbNum] = tempH;
                    tCost[nbNum] = tempT;
                    parentPointer[nbNum] = current;
                    openPointers.emplace_back(neighbor);
                }
                else if (tempT < tCost[nbNum])
                {
                    // 既登録ノードでもコストが改善したら親を更新
                    gCosts[nbNum] = tempG;
                    hCosts[nbNum] = tempH;
                    tCost[nbNum] = tempT;
                    parentPointer[nbNum] = current;
                }
            }

            // ゴール到達またはオープンリスト空ならループ終了
            if (isGoal || openPointers.empty())
            {
                break;
            }

            // オープンリストから最小tCostのノードを選択
            float minTCost = std::numeric_limits<float>::infinity();
            int   minIndex = -1;
            for (int i = 0; i < (int)openPointers.size(); ++i)
            {
                int nodeNum = openPointers[i]->GetNumber();
                if (tCost[nodeNum] < minTCost)
                {
                    minTCost = tCost[nodeNum];
                    current = openPointers[i];
                    currentNumber = nodeNum;
                    minIndex = i;
                }
            }
            // 選択したノードをオープンリストから除去
            if (minIndex >= 0)
            {
                openPointers.erase(openPointers.begin() + minIndex);
            }
            else
            {
                // 何らかの理由で選べない場合は探索中断
                break;
            }
        }

        // ゴール未到達なら空パスを返す
        if (!isGoal)
        {
            return {};
        }

        // 経路復元: ゴールから開始ノードへ辿る
        std::vector<Vec3> path;
        auto pathPointer = goalPointer;
        const int MAX_PATH_COUNT = 1000;
        int count = 0;
        while (pathPointer && count < MAX_PATH_COUNT)
        {
            path.push_back(pathPointer->GetPosition());
            pathPointer = parentPointer[pathPointer->GetNumber()];
            ++count;
        }

        // 逆順 (スタート→ゴール) に並べ替えて返却
        std::reverse(path.begin(), path.end());
        return path;
    }

    // 指定位置に最も近いセルのインデックスを返す
    int Navigate::GetIndexFromPosition(const Vec3& position) {
        // 初期値: 比較用に非常に大きな距離を設定
        float memoryPos = 10000000;
        Vec3  nearPoint;               // 現時点で最も近い座標
        shared_ptr<RootPointer> nearPointMemory;  // 最も近いRootPointer

        // 全セルをループして、positionにもっとも近いポイントを探す
        for (int i = 0; i < m_CellData.size(); i++) {
            // セルのワールド座標を取得
            Vec3 vec = m_CellData[i]
                ->GetComponent<Transform>()
                ->GetPosition();

            // 初回またはより近い場合に更新
            float dist = (vec - position).length();
            if (dist < memoryPos) {
                memoryPos = dist;
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
        }

        // 見つかった座標と一致するセルのインデックスを再検索して返却
        for (int i = 0; i < m_CellData.size(); i++) {
            if (m_CellData[i]->GetComponent<Transform>()->GetPosition() == nearPoint) {
                return i;
            }
        }
        // 近傍セルが見つからなかった場合
        return -1;
    }

    // 指定位置に最も近いRootPointerオブジェクトを返す
    shared_ptr<RootPointer> Navigate::GetNearPointer(const Vec3& position) {
        Vec3  nearPoint;               // 現時点で最も近い座標
        shared_ptr<RootPointer> nearPointMemory;  // 最も近いRootPointer

        // 全セルをループして最も近いRootPointerを探索
        for (int i = 0; i < m_CellData.size(); i++) {
            Vec3 vec = m_CellData[i]->GetPosition();

            // 初回またはより近い場合に更新
            float dist = (vec - position).length();
            if (i == 0 || dist < (nearPoint - position).length()) {
                nearPoint = vec;
                nearPointMemory = m_CellData[i];
            }
        }
        return nearPointMemory;
    }

    // Waypoint情報から隣接ノードのインデックスリストを取得
    std::vector<int> Navigate::GetNeighborsForWaypoints(int currentIndex) {
        std::vector<int> neighbors;
        // ポインタに紐づく番号文字列を'_'区切りで解析しVector<int>に変換
        auto waypointIndices =
            WstrToIntVector(m_CellData[currentIndex]->GetPointerNumber());
        for (int idx : waypointIndices) {
            neighbors.push_back(idx);
        }
        return neighbors;
    }

    // 現在位置PosからTargetへ向かう際の次のWaypointを計算して返す
    Vec3 Navigate::NextWayPoint(const Vec3& Pos, const Vec3& Target) {
        float memoryPos = 10000000;
        Vec3  outCome;                  // 最適と判断した移動先座標
        Vec3  startNearest;             // Posに最も近いRootPointer座標
        shared_ptr<RootPointer> startPtr;

        // 1) Posに最も近いRootPointerを探索
        for (int i = 0; i < m_CellData.size(); i++) {
            Vec3 vec = m_CellData[i]
                ->GetComponent<Transform>()
                ->GetPosition();
            float dist = (vec - Pos).length();
            if (i == 0 || dist < (startNearest - Pos).length()) {
                startNearest = vec;
                startPtr = m_CellData[i];
            }
        }

        // 2) 最寄りポイントの隣接Waypointsを取得
        auto indices = WstrToIntVector(startPtr->GetPointerNumber());

        // 3) 各候補の総移動コストを計算し最小のものを選択
        for (int idx : indices) {
            Vec3 candidate = m_CellData[idx]
                ->GetComponent<Transform>()
                ->GetPosition();

            // 自分自身と同じセルならスキップ
            if (Pos == candidate) continue;

            // コスト = Pos→candidate + candidate→Target
            float cost = (Pos - candidate).length()
                + (candidate - Target).length();
            if (cost < memoryPos) {
                memoryPos = cost;
                outCome = candidate;
            }
        }

        return outCome;
    }

    // ブロック回避処理: 次のWaypointを計算しつつ目標を再設定
    void Navigate::AvoidBlock(const Vec3& Position, const Vec3& Target) {
        if (m_CellData.empty()) return;  // セルデータなしなら何もしない

        // ボス用一時停止状態でなければ次のWaypointを取得
        if (!m_BossPause) {
            Vec3 nextPoint = NextWayPoint(Position, Target);
            // 必要に応じてSetTargetPositionなどで内部状態を更新
            SetTargetPosition(Position, nextPoint);
        }

        // 目標位置（Target）は常に最新のものを保持
        SetTargetPosition(Position, Target);
    }
}
//end basecross
