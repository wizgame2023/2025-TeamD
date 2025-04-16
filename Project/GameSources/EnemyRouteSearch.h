/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class RootPointer;
	class Navigate : public Component {
	public:
		// ヘルパー関数: Vec3のハッシュ化 (unordered_mapで利用する場合)
		std::size_t Vec3HashFunction(const Vec3& v) {
			std::hash<float> floatHash;
			return floatHash(v.getX()) ^ (floatHash(v.getY()) << 1) ^ (floatHash(v.getZ()) << 2);
		}

		// 状態を表すenum
		enum class State {
			NONE,
			OPEN,
			CLOSE
		};

		// セルのデータを保持する構造体
		struct Data {
			State m_State = State::NONE;
			float m_StartDistance = 0.0f; // 開始位置からの距離
			float m_GoalDistance = 0.0f;  // 目標位置からの距離
			float m_TotalDistance = 0.0f; // 合計距離 (初期値は無限大)
			Vec3 m_ParentPosition = Vec3(-1, -1, -1); // 親の位置

			// 距離を設定する関数
			void SetDistance(float start, float goal) {
				m_StartDistance = start;
				m_GoalDistance = goal;
				m_TotalDistance = start + goal;
			}
		};

		enum class Dire {
			X, // 水平方向
			Z  // 垂直方向
		};

		Navigate(const std::shared_ptr<GameObject>& GameObjectPtr);
		~Navigate() override;

		void SetTargetPosition(const Vec3& Position, const Vec3& target);

		float Heuristic(Vec3 a, Vec3 b)
		{
			return std::abs(a.x - b.x) + std::abs(a.z - b.z); // XZ平面のみの距離
		}

		// Vec3同士が同じ位置にあるかをチェックする関数
		bool SamePosition(Vec3 a, Vec3 b) {
			float tolerance = 0.1f;
			return (std::abs(a.x - b.x) < tolerance && std::abs(a.z - b.z) < tolerance);
		}


		Vec3 GetBefperePosition() {
			return m_BeforeTarget;
		}

		void AvoidBlock(const Vec3& Position, const Vec3& Target);

		vector<Vec3> GetWayPointNavigate()
		{
			return m_NaviPoint;
		}

		virtual void OnUpdate()override {}
		virtual void OnDraw()override {}

		shared_ptr<RootPointer> GetNearPinter(const Vec3& position);

		vector<Vec3> FindPathWithWaypoints2(const shared_ptr<RootPointer>& pointer, const Vec3& goal);
		vector<Vec3> FindPathWithWaypoints(const shared_ptr<RootPointer>& pointer, const Vec3& goal);

	private:
		vector<int> GetNeighborsForWaypoints(int current);
		//void AStarAlgorithm(Vec3 index, Vec3 goal);
		int GetIndexFromPosition(const Vec3& position);
		Vec3 NextWayPoint(const Vec3& s, const Vec3& e);
		bool m_DireChange;
		float m_MapWidth;
		float m_MapHeight;
		vector<shared_ptr <RootPointer>> m_CellData;
		vector<Vec3> m_NaviPoint;
		Vec3 m_BeforeTarget;
		Vec3 m_BeforePosition;
		Dire m_Dire;
		Vec3 m_TargetPosition;
		Vec3 m_Index;
		Vec3 m_StartPosition;  // 開始位置を記憶する
		Vec3 m_HalfPosition;
		std::stack<Vec3> points;
		shared_ptr<GameObject> m_NearObject = nullptr;
		vector<int> m_number;
		// セルのサイズ
		float m_CellSize = 5.0f;

		// デバッグ用 (A*アルゴリズムの実行を制御)
		bool m_debug_pause = false;
		bool m_BossPause = false;

		float WstrToFlt(const wstring& data) {
			if (data == L"") return NULL;
			return stof(data);
		}

		vector<int> WstrToVecInt(const wstring& data) {
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
}

//end basecross
