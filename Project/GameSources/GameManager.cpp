/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void GameManager::Update() {
		if (m_ZoneTimer.UpdateTimer()) {
			m_TimeRate = 1.0f;
		}
		else {
			m_TimeRate = m_ZoneRate;
		}
	}
}
//end basecross
