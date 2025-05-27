/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Timer;
	class Sprite;
	class NumberSprite;

	class BossAppearText : public Object {
		Timer m_ExitTime;
		shared_ptr<Sprite> m_Text;
	public:
		BossAppearText(const shared_ptr<Stage>& stage, Vec3 position, Vec3 size);
		virtual ~BossAppearText() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};

	class NextWaveText : public Object {
		shared_ptr<Sprite> m_Text;
		shared_ptr<Sprite> m_BossText;
		shared_ptr<NumberSprite> m_CurrentWaveSprite;
		shared_ptr<NumberSprite> m_MaxWaveSprite;

		Vec3 m_CenterPosition;

		int m_CurrentWave;
		int m_MaxWave;
	public:
		NextWaveText(const shared_ptr<Stage>& stage, Vec3 center,int currect,int max);
		virtual ~NextWaveText(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}

//end basecross
