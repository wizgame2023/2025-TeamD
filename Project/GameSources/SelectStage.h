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
		void CreateViewLight();
		void CreateResource();
		void CreateSelect();
		std::shared_ptr<basecross::XAudio2Manager> m_ptrXA = App::GetApp()->GetXAudio2Manager();
		
		bool m_IsOpen;
		int m_Count;
		int m_DifficultyLevel;
		int m_StageNumber;
		shared_ptr<Sprite> m_LowSprite;
		shared_ptr<Sprite> m_MiddleSprite;
		shared_ptr<Sprite> m_HighSprite;
		shared_ptr<Sprite> m_TitleSprite;
		shared_ptr<Sprite>m_StageSprite;
		shared_ptr<NumberSprite> m_Number;
		shared_ptr<SpriteFade> m_Fade;

		vector<shared_ptr<Sprite>> m_Difficultys;
		//bool m_fade;
	public:
		//構築と破棄
		SelectStage() : Stage(), m_TotalTimer(Timer(0.5f, false)){};
		virtual ‾SelectStage() {};
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void OnPushA();

		void t(shared_ptr<ObjectInterface> object){}

		void AcceptStage(int index);
		void AcceptDifficulty(int index);
		void StartStage();
		void Select();
	};

}
