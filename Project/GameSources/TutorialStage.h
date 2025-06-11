/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Sprite;
	class NumberSprite;
	class SharpFan;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class TutorialStage : public GameStage {

		shared_ptr<NumberSprite> m_Fps;
		shared_ptr<Sprite> m_backGround;
		shared_ptr<Sprite> m_backStage;
		shared_ptr<Sprite> m_backSprite;

		float m_ElapsedTime;
		bool m_EnemyDead;
		bool m_BossDead;
		bool m_PlayerDead;
		//ビューの作成
		void CreateResource();
		void CreateSelect();
		void ClearSprite();

		//void UpdateStop(bool ClearSprite);
		bool m_IsPose;

	public:
		//構築と破棄
		TutorialStage(const wstring& file) : GameStage(file), m_IsPose(false) {}
		virtual ‾TutorialStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnEvent(const shared_ptr<Event>& event)override;

		void ClosePose();
		void OpenPose();

	};

}
//end basecross
