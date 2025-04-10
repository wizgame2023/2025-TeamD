/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Sprite;
	class UltIcon : public GameObject {
		shared_ptr<Sprite> m_IconBackGround;
		shared_ptr<Sprite> m_Icon;

		float m_Charge;
	public:
		UltIcon(const shared_ptr<Stage>& stage) : GameObject(stage),m_Charge(0.0f){}
		virtual ~UltIcon(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetCharge(float charge) {
			m_Charge = charge;
		}
	};
}

//end basecross
