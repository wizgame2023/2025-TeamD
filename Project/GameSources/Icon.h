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
		shared_ptr<Sprite> m_IconEffect;
		shared_ptr<Sprite> m_IconEffectWaku;

		float m_Charge;
	public:
		UltIcon(const shared_ptr<Stage>& stage) : GameObject(stage),m_Charge(0.0f){}
		virtual ‾UltIcon(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void SetDraw(bool a);
		void SetCharge(float charge) {
			m_Charge = charge;
		}
	};
	class NormalIcon : public GameObject {
		shared_ptr<Sprite> m_Icon;

		wstring m_TexKey;
		Vec3 m_Position;
		DWORD m_Input;
		DWORD m_KeyInput;
		bool m_IsPressed;

		Col4 m_PressedColor;
		Col4 m_NormalColor;
		float m_PressTime;
		float m_MaxPressTime;
	public:
		NormalIcon(const shared_ptr<Stage>& stage,const wstring& key,Vec3 position,Col4 pressed,Col4 normal,float time) :
			GameObject(stage),m_TexKey(key),m_Position(position),
			m_Input(0), m_KeyInput(0),m_IsPressed(false),m_PressedColor(pressed),m_NormalColor(normal), m_PressTime(time), m_MaxPressTime(time)
		{}
		virtual ‾NormalIcon() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetDraw(bool a);

		void SetInput(DWORD input) {
			m_Input = input;
		}
		void SetKeyInput(DWORD input) {
			m_KeyInput = input;
		}
	};
}

//end basecross
