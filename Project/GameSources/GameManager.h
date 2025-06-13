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
		float m_GameSpeed;
		float m_TimeRate;
		float m_ZoneRate;
		Timer m_ZoneTimer;

		Difficulty m_GameDiffculty;

		bool m_IsDebug;
		bool m_IsUpdate;
		GameManager();
	public:
		static GameManager* Instance() {
			static GameManager instance;
			return &instance;
		}

		void Update();

		void StartZone(float time = 5.0f) {
			m_ZoneTimer.SetTime(time, true);
			m_IsUpdate = true;
		}
		void SetTimeRate(float rate) {
			m_TimeRate = rate;
			m_IsUpdate = false;
		}
		void SetZoneRate(float rate) {
			m_ZoneRate = rate;
		}
		void SetGameSpeed(float speed) {
			m_GameSpeed = speed;
		}
		
		float GetGameSpeed() {
			return m_GameSpeed;
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
