#include "stdafx.h"
#include "Project.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void SelectStage::CreateViewLight() {
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


	void SelectStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		app->RegisterTexture(L"SELECTSPRITE", uiPath + L"SelectStageToTitle.png");
		app->RegisterTexture(L"SELECT_TITLE", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"SELECT_STAGE", uiPath + L"SelectStage.png");
		app->RegisterTexture(L"SELECT_NUMBER", uiPath + L"Number.png");

		//app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
	}

	void SelectStage::CreateTitle() {
		auto titleSprite = AddGameObject<Sprite>(L"SELECTSPRITE", Vec3(0.0f, 100.0f, 0.0f), Vec2(700.0f, 600.0f), true);
		score = AddGameObject<NumberSprite>(L"SELECT_NUMBER", Vec3(0.0f, 100.0f, 0.0f), Vec2(33, 100), 1);
		//auto stratASprite = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		//auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		////点滅設定
		//stratASprite->AddComponent<SpriteFlash>(0.8f);
		//m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		//m_Fade->FadeOut();
		//m_Fade->Stop();
	}

	void SelectStage::OnCreate() {
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


	void SelectStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<SelectStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (cntlVec.bConnected) {
			if (cntlVec.wPressedButtons & XINPUT_GAMEPAD_A) {
				OnPushA();
			}
		}
		//if (m_Fade->IsFinish())
		//{
		//	PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		//}

		float rot;
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX > 0.5f) {
			count = (count + 1) % 4; // 0,1,2,3の範囲内ループ
			m_TotalTimer.Reset();
		}
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX < -0.5f) {
			count = (count + 3) % 4; // 0,1,2,3の範囲内ループ
			m_TotalTimer.Reset();
		}

		RemoveGameObject<Sprite>(mConut);
		//RemoveGameObject<NumberSprite>(score);

		switch (count) {
		case 0:		
			mConut = AddGameObject<Sprite>(L"SELECT_TITLE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			score->UpdateNumber(1);
			break;
		case 1:
			mConut = AddGameObject<Sprite>(L"SELECT_STAGE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			score->UpdateNumber(2);
			break;
		case 2:
			mConut = AddGameObject<Sprite>(L"SELECT_STAGE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			score->UpdateNumber(3);
			break;
		case 3:
			mConut = AddGameObject<Sprite>(L"SELECT_STAGE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			score->UpdateNumber(4);
			break;

		}

	}

	void SelectStage::OnPushA() {
		//ボタンを押されたらtrue
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		//m_Fade->Play();
		OnDestroy();
		if (count == 0) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
		}
		if (count == 1) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageM");
		}
		if (count == 2) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		}
		if (count == 3) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		}

	}

}
