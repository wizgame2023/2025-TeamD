/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Board;
	class Character;
	class HPBar : public GameObject
	{
		shared_ptr<Transform> m_BoneTransform;
		shared_ptr<Board> m_HpFrame;
		shared_ptr<Board> m_HpBar;
		shared_ptr<Character> m_Target;
		float m_MaxHp;
		float m_CurrentHp;

		Vec3 m_Offset;
	public:
		HPBar(const shared_ptr<Stage>& stage, const shared_ptr<Character>& target, Vec3 offset) :
			GameObject(stage), m_Target(target), m_MaxHp(0), m_CurrentHp(0), m_Offset(offset) {
		}
		virtual ~HPBar() {}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetMaxHp(float maxHp) {
			m_MaxHp = maxHp;
		}
		void SetCurrentHp(float currentHp) {
			m_CurrentHp = currentHp;
		}
		void SetOffset(Vec3 offset) {
			m_Offset = offset;
		}
		void SetColor(Col4 color) {
			m_HpBar->SetColor(color);
		}
		void Destroy();
	};
}

//end basecross
