/*!
@file TitleScenee.h
@brief タイトルシーン
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class SpriteFade;
	class TirleStageModel;

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class Movie : public MovieStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
	public:
		//構築と破棄
		Movie() :MovieStage() {}
		virtual ‾Movie() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}
