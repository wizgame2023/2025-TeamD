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
	class GameStageK : public GameStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreateBossEnemy();
		void RegisterObjects();
		bool m_IsPose;
	public:
		//構築と破棄
		GameStageK() : GameStage(), m_IsPose(false) {}
		virtual ~GameStageK() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void ClosePose();
		void OpenPose();
	};
}
//end basecross

