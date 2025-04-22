/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Timer;

	class BossAppearText : public Object {
		Timer m_ExitTime;
		shared_ptr<Sprite> m_Text;
	public:
		BossAppearText(const shared_ptr<Stage>& stage, Vec3 position, Vec3 size);
		virtual ~BossAppearText() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}

//end basecross
