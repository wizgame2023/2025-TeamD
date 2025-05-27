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
	class GameStageM : public GameStage {

		shared_ptr<NumberSprite> m_Fps;


		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void RegisterObjects();
		bool m_IsPose;

	public:
		//構築と破棄
		GameStageM(const wstring& file) :GameStage(file), m_IsPose(false) {}
		virtual ~GameStageM() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void ClosePose();
		void OpenPose();

	};

}
//end basecross
