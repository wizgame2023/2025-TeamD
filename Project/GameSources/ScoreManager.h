/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include<tuple>
namespace basecross{
	enum class JudgeMode {
		UpperOrder,
		LowerOrder
	};
	template<typename T>
	struct ScoreBorder {
		vector<T> m_Border;
		JudgeMode m_JundgeMode;
	public:
		ScoreBorder(vector<T> border,const JudgeMode& mode = JudgeMode::LowerOrder) :m_Border(border), m_JundgeMode(mode) {
		}
		void SetMode(JudgeMode mode) {
			m_JundgeMode = mode;
		}

		int CalcRank(T score) {
			for (int i = 0; i < m_Border.size(); i++) {
				switch (m_JundgeMode) {
				case JudgeMode::UpperOrder:
					if (m_Border[i] > score) {
						return i;
					}
					break;
				case JudgeMode::LowerOrder:
					if (m_Border[i] < score) {
						return i;
					}
					break;
				default:
					break;
				}
			}
			return static_cast<int>(m_Border.size());
		}
	};
	class ScoreManager{
		//ボーダー計算用
		ScoreBorder<float> m_TimeBorder;
		ScoreBorder<float> m_DamageBorder;
		ScoreBorder<int> m_ParryBorder;
		//スコア計算用
		float m_Time;
		float m_Damage;
		float m_EliminateEnemyRate;
		int m_ParryCount;
		//敵の討伐情報
		float m_MaxEnemyCount;
		float m_EliminateEnemyCount;
		ScoreManager() :
			m_Time(0.0f),
			m_Damage(0.0f),
			m_EliminateEnemyRate(0.0f), m_MaxEnemyCount(0.0f), m_EliminateEnemyCount(0.0f),
			m_ParryCount(0),
			m_TimeBorder(ScoreBorder<float>({ 50.0f,90.0f,130.0f })), m_DamageBorder(ScoreBorder<float>({ 10.0f,20.0f,30.0f })),
			m_ParryBorder(ScoreBorder<int>({ 5,3,1 }, JudgeMode::UpperOrder))
		{}
	public:
		static ScoreManager* Instance() {
			static ScoreManager instance;
			return &instance;
		}
		void Init() {
			m_Time = 0.0f;
			m_Damage = 0.0f;
			m_EliminateEnemyCount = 0.0f;
			m_MaxEnemyCount = 0.0f;
			m_EliminateEnemyRate = 0.0f;
			m_ParryCount = 0;
		}
		void UpdateTime(float elapsed) {
			m_Time += elapsed;
		}
		float GetTime() {
			return m_Time;
		}
		void AddDamage(float damage) {
			m_Damage += damage;
		}
		float GetDamage() {
			return m_Damage;
		}
		void AddParryCount() {
			m_ParryCount++;
		}
		int GetParryCount() {
			return m_ParryCount;
		}
		void AddEliminateEnemyCount() {
			m_EliminateEnemyCount++;
		}
		void SetMaxEnemyCount(float count) {
			m_MaxEnemyCount = count;
		}
		float GetEliminateEnemyCount() {
			return m_EliminateEnemyCount;
		}
		float CalculateEliminateEnemyRate() {
			m_EliminateEnemyRate = (m_EliminateEnemyCount / m_MaxEnemyCount) * 100.0f;
			return m_EliminateEnemyRate;
		}

		int GetTimeRank() {
			return m_TimeBorder.CalcRank(m_Time);
		}
		int GetDamageRank() {
			return m_DamageBorder.CalcRank(m_Damage);
		}
		int GetParryRank() {
			return m_ParryBorder.CalcRank(m_ParryCount);
		}

		float GetTotal()
		{
			return(GetTimeRank() + GetDamageRank() + GetParryRank()) / 3;
		}

		float GetTotalRank()
		{
			return roundf(GetTotal());
		}
	};
}
//end basecross
