/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	GameManager::GameManager() :
	m_GameSpeed(1.0f), m_ZoneRate(1.0f), m_TimeRate(1.0f), m_ZoneTimer(Timer(0.0f, false)),
		m_GameDiffculty(Difficulty::Easy), m_IsDebug(false), m_IsUpdate(true)
	{ }
	void GameManager::Update() {
		if (!m_IsUpdate) return;

		if (m_ZoneTimer.UpdateTimer()) {
			m_TimeRate = 1.0f;
		}
		else {
			m_TimeRate = m_ZoneRate;
		}
	}
}
//end basecross
