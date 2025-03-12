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
	class GameStageM : public Stage {
		//ビューの作成
		void CreateViewLight();
	public:
		//構築と破棄
		GameStageM() :Stage() {}
		virtual ~GameStageM() {}
		//初期化
		virtual void OnCreate()override;
	};


}
//end basecross

