/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void UltIcon::OnCreate() {
		Vec3 position = Vec3(499.0f, -158.0f, 0.0f);
		m_IconBackGround = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT_FRAME", position, Vec2(150.0f));
		m_IconBackGround->SetDrawLayer(2);
		m_IconBackGround->SetDiffuse(Col4(1, 1, 1, 1));

		m_IconBackGroundKey = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT_FRAME_KEY", position, Vec2(150.0f));
		m_IconBackGroundKey->SetDrawLayer(2);
		m_IconBackGroundKey->SetDiffuse(Col4(1, 1, 1, 1));

		m_Icon = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT", position + Vec3(15,-10,0), Vec2(120.0f));
		m_Icon->SetDrawLayer(3);

		m_IconEffect = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT_EFFECT", position + Vec3(15, -10, 0), Vec2(120.0f));
		m_IconEffect->SetDrawLayer(1);
		m_IconEffect->SetDiffuse(Col4(1,1,1,1));

		m_IconEffectWaku = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT_WAKU_EFFECT", position, Vec2(150.0f));
		m_IconEffectWaku->SetDrawLayer(1);
		m_IconEffectWaku->SetDiffuse(Col4(1, 1, 1, 1));
		m_IconEffectWaku->SetDrawActive(false);
	}

	void UltIcon::SetDraw(bool a) {
		m_IconBackGround->SetDrawActive(a);
		m_IconBackGroundKey->SetDrawActive(a);
		m_Icon->SetDrawActive(a);
		m_IconEffect->SetDrawActive(a);
	}

	void UltIcon::SetIconDraw()
	{
		auto& device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected)
		{
			m_IconBackGround->SetDrawActive(true);
			m_IconBackGroundKey->SetDrawActive(false);
		}
		else
		{
			m_IconBackGround->SetDrawActive(false);
			m_IconBackGroundKey->SetDrawActive(true);
		}
	}

	void UltIcon::OnUpdate() {
		Vec2 maxSize = m_IconEffect->GetSize();
		m_IconEffect->SetVertex({
			Vec3(0,-(maxSize.y * (1.0f - m_Charge)),0),
			Vec3(maxSize.x,-(maxSize.y * (1.0f - m_Charge)),0),
			Vec3(0,-maxSize.y,0),
			Vec3(maxSize.x,-maxSize.y,0)
			});
		m_IconEffect->UpdateUV({
			Vec2(0,(1.0f - m_Charge)),
			Vec2(1,(1.0f - m_Charge)),
			Vec2(0,1),
			Vec2(1,1)
			});

		if (m_Charge >= 1)
		{
			m_IconEffect->SetDiffuse(Col4(1, 1, 1, 1));
			m_IconEffectWaku->SetDrawActive(true);
			//m_Icon->SetDiffuse(Col4(1, 1, 1, 1));
			m_Icon->SetDrawActive(true);
			m_IconBackGround->SetDrawActive(false);
			m_IconBackGround->SetDrawLayer(2);
			m_IconBackGroundKey->SetDrawActive(false);
			m_IconBackGroundKey->SetDrawLayer(2);
		}
		else 
		{
			m_IconEffect->SetDiffuse(Col4(1, 1, 1, 0.5f));
			m_IconEffectWaku->SetDrawActive(false);
			m_Icon->SetDrawActive(false);
			m_IconBackGround->SetDrawActive(false);
			m_IconBackGround->SetDrawLayer(2);
			m_IconBackGroundKey->SetDrawActive(false);
			m_IconBackGroundKey->SetDrawLayer(2);
			//m_Icon->SetDiffuse(Col4(1, 1, 1, 0.5f));
		}
		SetIconDraw();

	}
	void NormalIcon::OnCreate() {
		m_Icon = GetStage()->AddGameObject<Sprite>(m_TexKey, m_Position, Vec2(150.0f));
	}
	void NormalIcon::OnUpdate() {
		auto device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		auto& keyState = App::GetApp()->GetInputDevice().GetKeyState();

		float elapsed = App::GetApp()->GetElapsedTime();
		if (device.bConnected) {
			if (device.wPressedButtons & m_Input) {
				m_IsPressed = true;
				m_PressTime = m_MaxPressTime;
			}
		}
		if (keyState.m_bPressedKeyTbl[m_KeyInput]) {
			m_IsPressed = true;
			m_PressTime = m_MaxPressTime;
		}

		if (m_IsPressed) {
			m_Icon->SetDiffuse(m_PressedColor);
			m_PressTime -= elapsed;
			if (m_PressTime <= 0) {
				m_PressTime = m_MaxPressTime;
				m_IsPressed = false;
			}
		}
		else {
			m_Icon->SetDiffuse(m_NormalColor);
		}
	}
	void NormalIcon::SetDraw(bool a)
	{
		m_Icon->SetDrawActive(a);
	}

}
//end basecross
