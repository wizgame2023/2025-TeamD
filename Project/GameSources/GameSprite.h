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
		virtual ‾BossAppearText() {}

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
		virtual ‾NextWaveText(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};


	class HpSprite : public Object {
		shared_ptr<Character> m_Owner;

		vector<shared_ptr<Sprite>> m_Items;
		vector<shared_ptr<Sprite>> m_Edge;
		shared_ptr<Sprite> m_HpBar;
		shared_ptr<Sprite> m_HpBarBackGround;

		Col4 m_Color;
	public:
		HpSprite(const shared_ptr<Stage>& stage, shared_ptr<Character>& owner,Vec3 position,Vec3 size,Col4 color);
		virtual ‾HpSprite(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetBackColor(Col4 color) {
			m_HpBarBackGround->SetDiffuse(color);
		}
		shared_ptr<Sprite> AddSprite(const wstring& key, Vec3 offset, Vec2 size);
	};

	class StunSprite : public Object {
		shared_ptr<BossEnemy> m_Owner;

		vector<shared_ptr<Sprite>> m_Edge;
		shared_ptr<Sprite> m_HpBar;
		shared_ptr<Sprite> m_HpBarBackGround;

	public:
		StunSprite(const shared_ptr<Stage>& stage, shared_ptr<BossEnemy>& owner, Vec3 position, Vec3 size);
		virtual ‾StunSprite() {}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void SetBackColor(Col4 color) {
			m_HpBarBackGround->SetDiffuse(color);
		}
	};
}

//end basecross
