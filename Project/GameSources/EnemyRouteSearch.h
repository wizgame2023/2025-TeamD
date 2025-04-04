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

		Vec3 GetAStarForword(const Vec3 Position);

		Vec3 GetBefperePosition() {
			return m_BeforeTarget;
		}

		Vec3 AvoidBlock(const Vec3& Position, const Vec3& Target);


		virtual void OnUpdate()override {}
		virtual void OnDraw()override {}

	private:
		void AStarAlgorithm(Vec3 index, Vec3 goal);
		Vec3 OpenCell(Vec3 index);
		bool UpdateDistance(Vec3 index);
		bool m_DireChange;
		float m_MapWidth;
		float m_MapHeight;
		vector<shared_ptr <GameObject>> m_CellData;
		Vec3 m_BeforeTarget;
		Vec3 m_BeforePosition;
		Dire m_Dire;
		Vec3 m_TargetPosition;
		Vec3 m_Index;
		Vec3 m_StartPosition;  // 開始位置を記憶する
		Vec3 m_HalfPosition;
		std::stack<Vec3> points;
		shared_ptr<GameObject> m_NearObject = nullptr;

		// セルのサイズ
		float m_CellSize = 5.0f;

		// デバッグ用 (A*アルゴリズムの実行を制御)
		bool m_debug_pause = false;		
	};
}

//end basecross
