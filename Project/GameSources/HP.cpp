/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void HP::SetMaxHp(float maxHp) {
		float damage = m_MaxHp - m_Hp;
		m_MaxHp = maxHp;
		m_Hp = m_MaxHp - damage;
	}

	void HP::Damage(float amount) {
		m_Hp -= amount;
		if (m_DamageCallBack) {
			m_DamageCallBack(amount);
		}
		if (m_Hp < 0) {
			m_Hp = 0;
			if (m_DeadCallBack) {
				m_DeadCallBack();
			}
		}
	}
	void HP::Heal(float amount) {
		m_Hp += amount;
		if (m_HealCallBack) {
			m_HealCallBack(amount);
		}
		if (m_Hp > m_MaxHp) {
			m_Hp = m_MaxHp;
		}
	}
}
//end basecross
