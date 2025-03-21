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
	class GameStageS : public Stage {
		//ビューの作成
		void CreateViewLight();
		void CreatePlayer();
		void CreateEnemy();
		void CreateCamera();
		void CreateWall();
		void CreateResource();
		void RegisterObjects();
		void CreatePose();
		void CreateSoundTest();
		void SetAllGameObjectActive(bool flag);
		bool m_IsPose;

	public:
		//構築と破棄
		GameStageS() :Stage() {}
		virtual ~GameStageS() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void ClosePose();
		void OpenPose();
	};


}
//end basecross

