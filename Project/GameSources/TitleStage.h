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
	class TitleStage : public Stage {
		//コントローラー取得
		InputHandler<TitleStage> m_InputHandler;
		shared_ptr<EffectManager> m_Effect;
		Effekseer::Handle m_Handle;
		Vec3 m_EffectPos;
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreateTitle();
		std::shared_ptr<basecross::XAudio2Manager> m_ptrXA = App::GetApp()->GetXAudio2Manager();
		shared_ptr<SpriteFade> m_Fade;
		shared_ptr<Sprite> m_BackGround;
		shared_ptr<Sprite> m_Title;
		shared_ptr<Sprite> m_Start;
		shared_ptr<Sprite> m_StartKey;
		shared_ptr<TirleStageModel> m_Titlemodel;
		float m_TotalTime;
		//bool m_fade;
	public:
		//構築と破棄
		TitleStage() :Stage() {}
		virtual ‾TitleStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void DrawIcon();
		void OnPushA();
		shared_ptr<EffectManager> GetEffect(){
			return m_Effect;
		}
	};

    class TirleStageModel : public Object {
		bool m_Flag = false; //フラグ
		bool m_EndFlag = false; //フラグ
		shared_ptr<EffectManager> m_Effect;
		Effekseer::Handle m_Handle;

    public:
        TirleStageModel(const shared_ptr<Stage>& stage) : Object(stage) {}
        virtual ‾TirleStageModel() {}
        //初期化
        virtual void OnCreate() override;
		virtual void OnUpdate()override;

		const void SetAnim(wstring animname, float time = 0.0f) {
			auto draw = GetComponent<BcPNTBoneModelDraw>();
			if (draw->GetCurrentAnimation() != animname)
				draw->ChangeCurrentAnimation(animname, time);
				/*if (draw->GetAnimeLoop()) draw->ChangeCurrentAnimation(animname, time);
				else
					if (draw->IsTargetAnimeEnd()) draw->ChangeCurrentAnimation(animname, time);*/
		}

		void SetFlag(bool flag) { 
			m_Flag = flag; 
		}
		bool GetFlag() const {
			return m_Flag;
		}
		bool GetEndFlag() const {
			return m_EndFlag;
		}
	};
}
