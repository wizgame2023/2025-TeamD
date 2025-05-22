/*!
@file TitleScne.cpp
@brief タイトルシーン実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void TitleStage::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		//ビューのカメラの設定
		auto PtrCamera = ObjectFactory::Create<Camera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}

	void TitleStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		app->RegisterTexture(L"TITLESPRITE", uiPath + L"Title.png");
		app->RegisterTexture(L"STRATA", uiPath + L"StartA.png");
		app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
	}

	void TitleStage::CreateTitle() {
		auto titleSprite = AddGameObject<Sprite>(L"TITLESPRITE", Vec3(0.0f, 100.0f, 0.0f), Vec2(700.0f, 600.0f),true);
		auto stratASprite = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f),true);
		auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		//点滅設定
		stratASprite->AddComponent<SpriteFlash>(0.8f);
		m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		m_Fade->FadeOut();
		m_Fade->Stop();
	}

	void TitleStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			//OnUpdate();
			CreateResource();
			CreateTitle();
		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<TitleStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (cntlVec.bConnected) {
			if (cntlVec.wPressedButtons & XINPUT_GAMEPAD_A) {
				OnPushA();
			}
		}
		if (m_Fade->IsFinish())
		{
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
		}

	}

	void TitleStage::OnPushA() {
		//ボタンを押されたらtrue
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		m_Fade->Play();
	}

}
