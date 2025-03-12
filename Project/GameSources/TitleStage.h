/*!
@file TitleScenee.h
@brief タイトルシーン
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class TitleStage : public Stage {
		//コントローラー取得
		InputHandler<TitleStage> m_InputHandler;
		//ビューの作成
		void CreateViewLight();
		std::shared_ptr<basecross::XAudio2Manager> m_ptrXA = App::GetApp()->GetXAudio2Manager();

	public:
		//構築と破棄
		TitleStage() :Stage() {}
		virtual ~TitleStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		bool Which;
		void OnPushA();

	};

}