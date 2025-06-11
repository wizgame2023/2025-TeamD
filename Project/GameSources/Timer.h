/*!
@file Character.h
@brief タイマーの実装
*/

#pragma once
#include "stdafx.h"
namespace basecross {

	class Timer {
		float m_Timer;
		float m_MaxTime;
		bool m_IsReset;
	public:
		Timer(const bool& isReset = true);
		Timer(float time,const bool& isReset = true);
		Timer(float time,float firstTime, const bool& isReset = true);
		‾Timer();
		void SetTime(float time,const bool& isReset = false) {
			m_MaxTime = time;
			if (isReset) {
				m_Timer = 0;
			}
		}
		void Reset() {
			m_Timer = 0;
		}
		float GetTime() {
			return m_Timer;
		}
		float GetMaxTime() {
			return m_MaxTime;
		}
		bool UpdateTimer();
	};
}

//end basecross
