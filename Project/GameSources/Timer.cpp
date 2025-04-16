/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Timer::Timer(const bool& isReset) : Timer(0,isReset){}
	Timer::Timer(float time, const bool& isReset) {
		m_Timer = 0;
		m_MaxTime = time;
		m_IsReset = isReset;
	}
	Timer::~Timer(){}

	bool Timer::UpdateTimer() {
		if (m_Timer > m_MaxTime) {
			if (m_IsReset) {
				m_Timer = 0;
			}
			return true;
		}
		else {
			float elapsed = App::GetApp()->GetElapsedTime();
			m_Timer += elapsed;
		}
		
		return false;
	}
}
//end basecross
