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
	class GameStageS : public GameStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreatePlayer();
		void CreateEnemy();
		void CreateBossEnemy();
		void RegisterObjects();
		void SetAllGameObjectActive(bool flag);
		bool m_IsPose;

		shared_ptr<NumberSprite> m_ProtoHpNumber;
	public:
		//構築と破棄
		GameStageS() : GameStage(), m_IsPose(false) {}
		virtual ~GameStageS() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void ClosePose();
		void OpenPose();
	};


}
//end basecross

