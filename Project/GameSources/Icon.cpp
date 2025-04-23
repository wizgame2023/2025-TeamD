/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void UltIcon::OnCreate() {
		m_IconBackGround = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT_FRAME", Vec3(499.0f, -228.0f, 0.0f), Vec2(100.0f));
		m_Icon = GetStage()->AddGameObject<Sprite>(L"ACTION_ULT", Vec3(499.0f, -228.0f, 0.0f), Vec2(100.0f));
	}
	void UltIcon::OnUpdate() {
		Vec2 maxSize = m_Icon->GetSize();
		m_Icon->SetVertex({
			Vec3(0,-(maxSize.y * (1.0f - m_Charge)),0),
			Vec3(maxSize.x,-(maxSize.y * (1.0f - m_Charge)),0),
			Vec3(0,-maxSize.y,0),
			Vec3(maxSize.x,-maxSize.y,0)
			});
		m_Icon->UpdateUV({
			Vec2(0,(1.0f - m_Charge)),
			Vec2(1,(1.0f - m_Charge)),
			Vec2(0,1),
			Vec2(1,1)
			});
	}
	void NormalIcon::OnCreate() {
		m_Icon = GetStage()->AddGameObject<Sprite>(m_TexKey, m_Position, Vec2(100.0f));
	}
	void NormalIcon::OnUpdate() {
		auto device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		float elapsed = App::GetApp()->GetElapsedTime();
		if (device.bConnected) {
			if (device.wPressedButtons & m_Input) {
				m_IsPressed = true;
				m_PressTime = m_MaxPressTime;
			}
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
}
//end basecross
