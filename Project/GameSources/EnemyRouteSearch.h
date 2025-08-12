/*!
 @file EnemyRouteSearch.h
 @brief A*アルゴリズムを用いた敵のルート探索コンポーネント定義
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"

namespace basecross {

    /**
     * @brief ルート探索用ポイントを表すクラスの前方宣言
     */
    class RootPointer;

    /**
     * @brief 敵キャラクターの移動ルートをA*アルゴリズムで計算し、
     *        ナビゲーションポイントを提供するコンポーネント
     */
    class Navigate : public Component {
    public:
        /**
         * @brief Vec3のハッシュ関数 (unordered_mapのキーとして使用)
         * @param v ハッシュ化対象の3Dベクトル
         * @return ハッシュ値
         */
        //std::size_t Vec3HashFunction(const Vec3& v);

        /**
         * @brief ノード探索時の状態列挙
         */
        enum class State {
            NONE,  /**< 未探索      */
            OPEN,  /**< オープンリスト内 */
            CLOSE  /**< クローズリスト内 */
        };

        /**
         * @brief A*アルゴリズム用セル情報構造体
         */
        struct Data {
            State  m_State = State::NONE;        /**< 探索状態      */
            float  m_StartDistance = 0.0f;               /**< 開始位置からの距離 */
            float  m_GoalDistance = 0.0f;               /**< 目標位置からの推定距離 */
            float  m_TotalDistance = 0.0f;               /**< 総コスト(start+goal) */
            Vec3   m_ParentPosition = Vec3(-1, -1, -1);   /**< 探索木における親ノード位置 */

            /**
             * @brief スコアをまとめて設定する
             * @param start 開始ノードからの実コスト
             * @param goal  目標ノードまでの推定コスト
             */
            void SetDistance(float start, float goal) {
                m_StartDistance = start;
                m_GoalDistance = goal;
                m_TotalDistance = start + goal;
            }
        };

        /**
         * @brief 探索する軸の種類
         */
        enum class Dire {
            X,  /**< 水平方向移動 */
            Z   /**< 垂直方向移動 */
        };

        // 内部状態
        bool m_DireChange;                             /**< 方向転換フラグ              */
        float m_MapWidth;                              /**< マップ横幅                   */
        float m_MapHeight;                             /**< マップ縦幅                   */
        vector<shared_ptr<RootPointer>> m_CellData;    /**< セルポイントデータ           */
        vector<Vec3> m_NaviPoint;                      /**< 計算済みナビゲーションポイント */
        Vec3 m_BeforeTarget;                           /**< 直前の目標位置               */
        Vec3 m_BeforePosition;                         /**< 直前の現在位置               */
        Dire m_Dire;                                   /**< 現在の移動軸方向             */
        Vec3 m_TargetPosition;                         /**< 目標位置                     */
        Vec3 m_Index;                                  /**< 現在セルインデックス(Vector版) */
        Vec3 m_StartPosition;                          /**< 開始位置                     */
        Vec3 m_HalfPosition;                           /**< 半セルオフセット             */
        std::stack<Vec3> points;                       /**< 経路復元用スタック           */
        shared_ptr<GameObject> m_NearObject = nullptr; /**< 最寄りオブジェクト参照       */
        vector<int> m_number;                          /**< セル番号リスト               */
        float m_CellSize = 5.0f;                       /**< セルサイズ                   */

        // デバッグ用制御
        bool m_debug_pause = false;                    /**< アルゴリズム一時停止フラグ   */
        bool m_BossPause = false;                   /**< ボスパターン用一時停止フラグ */

        /**
         * @brief コンストラクタ
         * @param GameObjectPtr 所属するGameObjectの共有ポインタ
         */
        Navigate(const std::shared_ptr<GameObject>& GameObjectPtr);

        /**
         * @brief デストラクタ
         */
        ‾Navigate() override;

        /**
         * @brief 目標位置と開始位置を設定する
         * @param Position 開始位置
         * @param target   目標位置
         */
        void SetTargetPosition(const Vec3& Position, const Vec3& target);


        /**
         * @brief 配置されたブロックを考慮してルートを再計算する
         * @param Position 現在位置
         * @param Target   目標位置
         */
        void AvoidBlock(const Vec3& Position, const Vec3& Target);


        /**
         * @brief 更新処理 (本コンポーネントでは使用しない)
         */
        virtual void OnUpdate() override {}

        /**
         * @brief 描画処理 (本コンポーネントでは使用しない)
         */
        virtual void OnDraw() override {}

        /**
         * @brief 指定位置に最も近いRootPointerを検索する
         * @param position 検索したいワールド座標
         * @return 近傍のRootPointer共有ポインタ (見つからないときはnullptr)
         */
        shared_ptr<RootPointer> GetNearPointer(const Vec3& position);

        /**
         * @brief Waypoint経由で経路を探索する(A*アルゴリズム版1)
         * @param pointer 探索開始ポイントへのポインタ
         * @param goal    最終目標位置
         * @return 最適経路上のVec3リスト
         */
        vector<Vec3> FindPathWithWaypoints(const shared_ptr<RootPointer>& pointer, const Vec3& goal);

    private:
        /**
         * @brief 現在インデックスから隣接ノードのインデックスリストを取得する
         * @param current 現在のセルインデックス
         * @return 隣接セルのインデックスリスト
         */
        vector<int> GetNeighborsForWaypoints(int current);

        /**
         * @brief ワールド座標からセル配列のインデックスを取得する
         * @param position ワールド座標
         * @return セル配列インデックス
         */
        int GetIndexFromPosition(const Vec3& position);

        /**
         * @brief 次に向かうWaypointベクトルを計算する
         * @param s 開始地点
         * @param e 終了地点
         * @return s→e方向の単位ベクトルにセルサイズをかけた移動量
         */
        Vec3 NextWayPoint(const Vec3& s, const Vec3& e);

        /**
         * @brief wstringをfloatに変換するユーティリティ
         * @param data 変換対象のwstring
         * @return 変換後のfloat (変換失敗時は0)
         */
        float WstrToFlt(const wstring& data) {
            if (data == L"") return NULL;
            return stof(data);
        }

        /**
         * @brief 区切り文字'_'で分割されたwstringをintベクトルに変換するユーティリティ
         * @param data 区切り文字列
         * @return int型のベクトル
         */
        vector<int> WstrToIntVector(const wstring& data)
        {
            vector<wstring> vec3Str = {};
            vector<int> num = {};
            Util::WStrToTokenVector(vec3Str, data, L'_');
            for (int i = 0; i < vec3Str.size(); i++)
            {
                if (WstrToFlt(vec3Str[i]) != NULL)
                {
                    num.push_back(WstrToFlt(vec3Str[i]));
                }
            }
            return num;
        }
    };

}  // namespace basecross