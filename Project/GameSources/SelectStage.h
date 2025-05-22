#pragma once
#include "stdafx.h"
#include "Timer.h"

namespace basecross {
	class SpriteFade;
	class Sprite;
	class NumberSprite;
	class Menu;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class SelectStage : public Stage {
		//コントローラー取得
		InputHandler<SelectStage> m_InputHandler;
		//ビューの作成
		Timer m_TotalTimer;
		int count;
		void CreateViewLight();
		void CreateResource();
		void CreateTitle();
		std::shared_ptr<basecross::XAudio2Manager> m_ptrXA = App::GetApp()->GetXAudio2Manager();

		shared_ptr<Sprite> mConut;
		shared_ptr<NumberSprite> score;
		shared_ptr<SpriteFade> m_Fade;
		//bool m_fade;
	public:
		//構築と破棄
		SelectStage() : Stage(), m_TotalTimer(Timer(0.5f, false)) {};
		virtual ~SelectStage() {};
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void OnPushA();

	};

}