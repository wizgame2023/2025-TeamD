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
        m_CellData.resize(static_cast<size_t>(m_MapWidth / m_CellSize), std::vector<Data>(static_cast<size_t>(m_MapHeight / m_CellSize)));
        // 開始位置の初期化 (例: マップの中心)
        m_StartPosition = Vec3(m_MapWidth / 2.0f, 0.0f, m_MapHeight / 2.0f);
        m_Index = m_StartPosition;
        m_DireChange = true;
        m_Dire = Dire::X;
    }

    Navigate::~Navigate()
    {
    }

    void Navigate::SetTargetPosition(Vec3 newTargetPosition)
    {
        m_TargetPosition = newTargetPosition;
        AStarAlgorithm(newTargetPosition);
    }

    Vec3 Navigate::GetAStarForword(Vec3 Position)
    {
        if (Position != m_TargetPosition || Position != m_HalfPosition)
        {
            if (m_DireChange)
            {
                m_Dire = std::abs(Position.x - m_HalfPosition.x) > std::abs(Position.y - m_HalfPosition.y) ? Dire::X : Dire::Z;

                m_DireChange = false;
            }
            if (m_Dire == Dire::X)
            {
                if (m_HalfPosition.x < Position.x)
                {
                    return Vec3(-1,0,0);
                }
                else if (m_HalfPosition.x > Position.x)
                {
                    return Vec3(1, 0, 0);
                }
            }
            if (m_Dire == Dire::Z)
            {
                if (m_HalfPosition.z < Position.z)
                {
                    return Vec3(0, 0, -1);
                }
                else if (m_HalfPosition.z > Position.z)
                {
                    return Vec3(0, 0, 1);
                }
            }
        }
        else {
            m_DireChange = true;
        }
    }

    void Navigate::AStarAlgorithm(Vec3 goal)
    {
        // 開始位置をA*アルゴリズムの開始点として設定
        m_Index = m_StartPosition;

        // セルの状態をリセットする (前のパス検索の影響をクリア)
        for (auto& row : m_CellData) {
            for (auto& cell : row) {
                cell.m_State = State::NONE;
                cell.m_ParentPosition = Vec3(-1, -1, -1);
                cell.m_StartDistance = 0.0f;
                cell.m_GoalDistance = 0.0f;
                cell.m_TotalDistance = cell.m_StartDistance + cell.m_GoalDistance;
            }
        }

        // 開始ノードの距離を初期化
        UpdateDistance(m_Index);

        //OPENリストをベクトルで管理する
        std::vector<Vec3> open_list;
        open_list.push_back(m_Index);

        while (!open_list.empty())
        {
            // OPENリストから最も有望なノードを見つける
            Vec3 current = open_list[0];
            int current_index = 0;
            for (int i = 1; i < open_list.size(); i++) {
                int a_x = static_cast<int>(open_list[i].x / m_CellSize);
                int a_z = static_cast<int>(open_list[i].z / m_CellSize);

                if (a_x < 0 || a_x >= m_CellData.size() || a_z < 0 || a_z >= m_CellData[0].size()) continue;

                int b_x = static_cast<int>(current.x / m_CellSize);
                int b_z = static_cast<int>(current.z / m_CellSize);

                if (b_x < 0 || b_x >= m_CellData.size() || b_z < 0 || b_z >= m_CellData[0].size()) {
                    current = open_list[i];
                    current_index = i;
                    continue;
                }


                if (m_CellData[a_x][a_z].m_TotalDistance < m_CellData[b_x][b_z].m_TotalDistance) {
                    current = open_list[i];
                    current_index = i;
                }
            }

            // 現在のノードをOPENリストから削除
            open_list.erase(open_list.begin() + current_index);
            m_Index = current;


            // 目標に到達した場合
            if (m_Index == goal) {
                break;
            }

            // 現在のノードをCLOSEリストに入れる
            int index_x = static_cast<int>(m_Index.x / m_CellSize);
            int index_z = static_cast<int>(m_Index.z / m_CellSize);
            if (index_x >= 0 && index_x < m_CellData.size() && index_z >= 0 && index_z < m_CellData[0].size()) {
                m_CellData[index_x][index_z].m_State = State::CLOSE;
            }

            // 現在のノードの周囲のセルをOPENにする
            OpenCell(m_Index);


            float x = m_CellSize / 2.0f; // セルの半分のサイズ
            float z = m_CellSize / 2.0f;
            std::vector<Vec3> neighbor_indices = {
                Vec3(m_Index.x + x, m_Index.y, m_Index.z),
                Vec3(m_Index.x - x, m_Index.y, m_Index.z),
                Vec3(m_Index.x, m_Index.y, m_Index.z + z),
                Vec3(m_Index.x, m_Index.y, m_Index.z - z)
            };

            for (const auto& neighbor : neighbor_indices) {
                if (UpdateDistance(neighbor)) {
                    // OPENリストに存在するかどうかをチェック
                    bool in_open_list = false;
                    for (const auto& open_node : open_list) {
                        if (open_node == neighbor) {
                            in_open_list = true;
                            break;
                        }
                    }
                    if (!in_open_list) {
                        open_list.push_back(neighbor);
                    }
                }
            }
        }

        // パスのバックトレース
        std::stack<Vec3> path;
        Vec3 current = goal;

        // 経路が見つからない場合を検出するために、最大反復回数に制限を追加
        int max_iterations = 1000;
        int iteration_count = 0;

        while (current != m_StartPosition) {

            int current_x_index = static_cast<int>(current.x / m_CellSize);
            int current_z_index = static_cast<int>(current.z / m_CellSize);


            if (current_x_index < 0 || current_x_index >= m_CellData.size() || current_z_index < 0 || current_z_index >= m_CellData[0].size()) {
                std::cout << "パスが見つかりませんでした！(範囲外)" << std::endl;
                return;
            }

            path.push(current);
            current = m_CellData[current_x_index][current_z_index].m_ParentPosition;


            if (current.x == -1 && current.y == -1 && current.z == -1) {
                std::cout << "パスが見つかりませんでした！(親ノードなし)" << std::endl;
                return;
            }

            iteration_count++;
            if (iteration_count > max_iterations) {
                std::cout << "パスが見つかりませんでした!(最大反復回数超過)" << std::endl;
                return;
            }
        }

        // 計算されたパスを points スタックにコピー
        while (!path.empty()) {
            points.push(path.top());
            path.pop();
        }

        // m_HalfPosition を設定 (必要に応じて)
        if (!points.empty()) {
            m_HalfPosition = points.top();
        }

        // デバッグ用: 計算されたパスをコンソールに出力
        std::cout << "パス: " << std::endl;
        std::stack<Vec3> temp_points = points; // コピーを作成して元のスタックを破壊しないようにする
        while (!temp_points.empty()) {
            Vec3 p = temp_points.top();
            std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ") -> ";
            temp_points.pop();
        }
        std::cout << "目標" << std::endl;
    }


    void Navigate::OpenCell(Vec3 index)
    {
        float x = m_CellSize / 2.0f; // セルの半分のサイズ
        float z = m_CellSize / 2.0f;
        std::vector<Vec3> openIndex = {
            Vec3(index.x + x, index.y, index.z),
            Vec3(index.x - x, index.y, index.z),
            Vec3(index.x, index.y, index.z + z),
            Vec3(index.x, index.y, index.z - z)
        };

        for (const auto& open_index : openIndex)
        {
            UpdateDistance(open_index);  // ここでは更新だけを行う。OPENリストへの追加はAStarAlgorithmで行う。
        }
    }


    bool Navigate::UpdateDistance(Vec3 index)
    {
        // セルのインデックスを計算
        int x_index = static_cast<int>(index.x / m_CellSize);
        int z_index = static_cast<int>(index.z / m_CellSize);

        // インデックスが有効範囲内にあることを確認
        if (x_index < 0 || x_index >= m_CellData.size() || z_index < 0 || z_index >= m_CellData[0].size()) {
            return false;
        }


        Data& cellDate = m_CellData[x_index][z_index];

        // セルがCLOSEの場合、無視する
        if (cellDate.m_State == State::CLOSE) {
            return false;
        }

        // ゴールまでの距離を計算する(ヒューリスティック)
        float heuristic_distance = std::sqrt(std::pow(index.x - m_TargetPosition.x, 2) + std::pow(index.z - m_TargetPosition.z, 2));

        // スタート地点からの距離
        float start_distance = std::sqrt(std::pow(index.x - m_StartPosition.x, 2) + std::pow(index.z - m_StartPosition.z, 2));

        // startDistanceの計算を変更
        float new_start_distance;
        if (cellDate.m_ParentPosition.x == -1 && cellDate.m_ParentPosition.y == -1 && cellDate.m_ParentPosition.z == -1) {
            new_start_distance = start_distance; // 親がない場合
        }
        else {
            //親ノードがある場合、親ノードのstartDistance + 現在のノードから親ノードへの距離を計算
            int parent_x_index = static_cast<int>(cellDate.m_ParentPosition.x / m_CellSize);
            int parent_z_index = static_cast<int>(cellDate.m_ParentPosition.z / m_CellSize);

            if (parent_x_index < 0 || parent_x_index >= m_CellData.size() || parent_z_index < 0 || parent_z_index >= m_CellData[0].size()) {
                return false; // 親が無効
            }

            new_start_distance = m_CellData[parent_x_index][parent_z_index].m_StartDistance + std::sqrt(std::pow(index.x - cellDate.m_ParentPosition.x, 2) + std::pow(index.z - cellDate.m_ParentPosition.z, 2));
        }


        // 合計距離を計算する
        float total_distance = new_start_distance + heuristic_distance;

        // 以前に見ている場合でも、より良い経路が見つかったかどうかを確認します。
        if (cellDate.m_State == State::NONE || total_distance < cellDate.m_TotalDistance) {
            cellDate.m_State = State::OPEN;
            cellDate.SetDistance(new_start_distance, heuristic_distance);
            cellDate.m_TotalDistance = total_distance; // 合計距離を更新
            cellDate.m_ParentPosition = m_Index;
            return true; // 距離が更新された
        }

        return false; // 距離は更新されなかった
    }
}
//end basecross
