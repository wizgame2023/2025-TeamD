/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class ScoreManager{
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
			m_ParryCount(0){}
	public:
		static ScoreManager* Instance() {
			static ScoreManager instance;
			return &instance;
		}
		void UpdateTime(float elapsed) {
			m_Time += elapsed;
		}
		void AddDamage(float damage) {
			m_Damage += damage;
		}
		void AddParryCount() {
			m_ParryCount++;
		}
		void AddEliminateEnemyCount() {
			m_EliminateEnemyCount++;
		}
		void SetMaxEnemyCount(int count) {
			m_MaxEnemyCount = count;
		}
		float GetEliminateEnemyCount() {
			return m_EliminateEnemyCount;
		}
		float CalculateEliminateEnemyRate() {
			m_EliminateEnemyRate = (m_EliminateEnemyCount / m_MaxEnemyCount) * 100.0f;
			return m_EliminateEnemyRate;
		}
	};
}
//end basecross
