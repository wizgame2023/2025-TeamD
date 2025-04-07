/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class NumberSprite;
	class Menu;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {

		wstring m_MapFileName;
		bool m_IsPose;
		shared_ptr<NumberSprite> m_ProtoHpNumber;
		shared_ptr<NumberSprite> m_ProtoScoreNumber;
		shared_ptr<Menu> m_PauseMenu;
		shared_ptr<Menu> m_SoundTestMenu;
		shared_ptr<Menu> m_ResultMenu;
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void RegisterObjects();
		void CreateBossEnemy();

		void CreatePose();
		void CreateSoundTest();
		void CreateResult();

		void ClosePose();
		void OpenPose();
		void SetAllGameObjectActive(bool flag);
		void GameOver(){}
		void GameClear(){}
		
	public:
		//構築と破棄
		GameStage(const wstring& file) : Stage(),m_MapFileName(file), m_IsPose(false){}
		virtual ~GameStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;
		virtual void OnDestroy()override;
		virtual void OnEvent(const shared_ptr<Event>& event)override;
	};


}
//end basecross

