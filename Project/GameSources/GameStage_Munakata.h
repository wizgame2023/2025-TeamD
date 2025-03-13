/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStageM : public GameStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreatePlayer();
		void CreateEnemy();
		bool m_IsPose;
	public:
		//構築と破棄
		GameStageM() :GameStage(),m_IsPose(false) {}
		virtual ~GameStageM() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void ClosePose();
		void OpenPose();
	};


}
//end basecross

