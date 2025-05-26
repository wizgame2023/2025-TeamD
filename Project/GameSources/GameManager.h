/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Timer.h"
namespace basecross{
	enum class Difficulty {
		Easy = 1,
		Normal = 2,
		Hard = 3
	};
	class GameManager {
		float m_TimeRate;
		float m_ZoneRate;
		Timer m_ZoneTimer;

		Difficulty m_GameDiffculty;

		bool m_IsDebug;
		GameManager() :
			m_ZoneRate(1.0f), m_TimeRate(1.0f), m_ZoneTimer(Timer(0.0f, false)),
			m_GameDiffculty(Difficulty::Easy), m_IsDebug(false)
		{ }
	public:
		static GameManager* Instance() {
			static GameManager instance;
			return &instance;
		}

		void Update();

		void StartZone(float time = 5.0f) {
			m_ZoneTimer.SetTime(time, true);
		}
		void SetTimeRate(float rate) {
			m_TimeRate = rate;
		}
		void SetZoneRate(float rate) {
			m_ZoneRate = rate;
		}
		
		float GetTimeRate() {
			return m_TimeRate;
		}

		void SetDifficulty(Difficulty difficulty) {
			m_GameDiffculty = difficulty;
		}
		Difficulty GetDifficulty() {
			return m_GameDiffculty;
		}

		bool IsDebug() {
			return m_IsDebug;
		}
	};
}
//end basecross
