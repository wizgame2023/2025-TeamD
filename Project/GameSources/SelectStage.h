#pragma once
#include "stdafx.h"
#include "Timer.h"

namespace basecross {
	class SpriteFade;
	class Sprite;
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
		Vec2 GetInputState() const;
		Vec3 GetMoveVector(float& rot)const;
		std::shared_ptr<basecross::XAudio2Manager> m_ptrXA = App::GetApp()->GetXAudio2Manager();

		shared_ptr<Sprite> mConut;
		shared_ptr<SpriteFade> m_Fade;
		//bool m_fade;
	public:
		//構築と破棄
		SelectStage() : Stage(), m_TotalTimer(Timer(0.5f, false)) {};
		virtual ~SelectStage() {};
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		bool Which;
		void OnPushA();

	};

}