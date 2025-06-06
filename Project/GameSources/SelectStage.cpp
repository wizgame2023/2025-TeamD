#include "stdafx.h"
#include "Project.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	
	//--------------------------------------------------------------------------------------
	void SelectStage::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		auto PtrCamera = ObjectFactory::Create<Camera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		auto PtrMultiLight = CreateLight<MultiLight>();
		PtrMultiLight->SetDefaultLighting();
	}


	void SelectStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/ ";
		app->RegisterTexture(L"SELECTSPRITE", uiPath + L"SelectStageToTitle.png");
		app->RegisterTexture(L"SELECT_TITLE", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"SELECT_STAGE", uiPath + L"SelectStage.png");
		app->RegisterTexture(L"SELECT_NUMBER", uiPath + L"Number.png");
		app->RegisterTexture(L"BACKGROUND", texPath + L"TitleBackGround.png");
		//app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
		app->RegisterTexture(L"DANGER_LOW", uiPath + L"Danger_Low.png");
		app->RegisterTexture(L"DANGER_MIDDLE", uiPath + L"Danger_Middle.png");
		app->RegisterTexture(L"DANGER_HIGH", uiPath + L"Danger_High.png");

		app->RegisterTexture(L"SELECT_EASY", uiPath + L"SelectEasy.png");
		app->RegisterTexture(L"SELECT_NORMAL", uiPath + L"SelectNormal.png");
		app->RegisterTexture(L"SELECT_HARD", uiPath + L"SelectHard.png");
	}

	void SelectStage::CreateSelect() {
		auto backGround = AddGameObject<Sprite>(L"BACKGROUND", Vec3(0.0f, 0.0f, 0.0f), Vec2(1280.0f, 800.0f), true);

		auto titleSprite = AddGameObject<Sprite>(L"SELECTSPRITE", Vec3(0.0f, 100.0f, 0.0f), Vec2(600.0f, 200.0f), true);
		m_Number = AddGameObject<NumberSprite>(L"SELECT_NUMBER", Vec3(120.0f, -140.0f, 0.0f), Vec2(33, 100), 1);
		m_TitleSprite = AddGameObject<Sprite>(L"SELECT_TITLE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 150.0f), true);
		m_StageSprite = AddGameObject<Sprite>(L"SELECT_STAGE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		m_LowSprite = AddGameObject<Sprite>(L"DANGER_LOW", Vec3(0.0f, -300.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		m_MiddleSprite = AddGameObject<Sprite>(L"DANGER_MIDDLE", Vec3(0.0f, -300.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		m_HighSprite = AddGameObject<Sprite>(L"DANGER_HIGH", Vec3(0.0f, -300.0f, 0.0f), Vec2(100.0f, 80.0f), true);
		//auto stratASprite = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		//auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		//stratASprite->AddComponent<SpriteFlash>(0.8f);
		//m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		//m_Fade->FadeOut();
		//m_Fade->Stop();

		m_Difficultys.push_back(AddGameObject<Sprite>(L"SELECT_EASY", Vec3(0.0f, -200.0f, 0.0f), Vec2(250.0f, 150.0f), true));
		m_Difficultys.push_back(AddGameObject<Sprite>(L"SELECT_NORMAL", Vec3(0.0f, -200.0f, 0.0f), Vec2(250.0f, 150.0f), true));
		m_Difficultys.push_back(AddGameObject<Sprite>(L"SELECT_HARD", Vec3(0.0f, -200.0f, 0.0f), Vec2(250.0f, 150.0f), true));
	}

	void SelectStage::OnCreate() {
		try {
			CreateViewLight();
			//OnUpdate();
			CreateResource();
			CreateSelect();
			//SoundManager::Instance().PlayBGM(L"BGM_SELECT");

		}
		catch (...) {
			throw;
		}
	}


	void SelectStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<SelectStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		//if (m_Fade->IsFinish())
		//{
		//	PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		//}

		float rot;
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX > 0.5f) {
			m_Count = (m_Count + 1) % 4;
			m_TotalTimer.Reset();
		}
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX < -0.5f) {
			m_Count = (m_Count + 3) % 4;
			m_TotalTimer.Reset();
		}
		m_Number->SetDrawActive(false);
		switch (m_Count) {
		case 0:
			//m_Number->UpdateNumber(1);
			m_TitleSprite->SetDrawActive(false);
			m_StageSprite->SetDrawActive(false);
			break;
		case 1:
			//m_Number->UpdateNumber(2);
			m_TitleSprite->SetDrawActive(false);
			//m_StageSprite->SetDrawActive(true);
			break;
		case 2:
			//m_Number->UpdateNumber(3);
			m_TitleSprite->SetDrawActive(false);
			//m_StageSprite->SetDrawActive(true);
			break;
		case 3:
			//m_Number->UpdateNumber(4);
			m_TitleSprite->SetDrawActive(true);
			m_StageSprite->SetDrawActive(false);
			//m_StageSprite->SetDrawActive(true);
			break;

		}
		//m_LowSprite->SetDrawActive(true);
		//m_MiddleSprite->SetDrawActive(true);
		//m_HighSprite->SetDrawActive(true);
		m_LowSprite->SetDrawActive(false);
		m_MiddleSprite->SetDrawActive(false);
		m_HighSprite->SetDrawActive(false);
		//if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLY > 0.5f) {
		//	m_DifficultyLevel = (m_DifficultyLevel + 1) % 4;
		//	m_TotalTimer.Reset();
		//}
		//if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLY < -0.5f) {
		//	m_DifficultyLevel = (m_DifficultyLevel + 3) % 4;
		//	m_TotalTimer.Reset();
		//}
		//switch (m_DifficultyLevel) {
		//case 0:
		//	//m_Number->UpdateNumber(2);
		//	m_LowSprite->SetDiffuse(Col4(0, 1, 0, 1));
		//	m_MiddleSprite->SetDrawActive(false);
		//	m_HighSprite->SetDrawActive(false);
		//	//m_StageSprite->SetDrawActive(true);
		//	break;
		//case 1:
		//	//m_Number->UpdateNumber(3);
		// 	m_LowSprite->SetDrawActive(false);
		//	m_MiddleSprite->SetDiffuse(Col4(1, 1, 0, 1));
		// 	m_HighSprite->SetDrawActive(false);
		//	//m_StageSprite->SetDrawActive(true);
		//	break;
		//case 2:
		//	//m_Number->UpdateNumber(4);
		// 	m_LowSprite->SetDrawActive(false);
		// 	m_MiddleSprite->SetDrawActive(false);
		//	m_HighSprite->SetDiffuse(Col4(1, 0, 0, 1));
		//	//m_StageSprite->SetDrawActive(true);
		//	break;
		//case 3:
		//	//m_Number->UpdateNumber(4);
		//	m_LowSprite->SetDrawActive(false);
		//	m_MiddleSprite->SetDrawActive(false);
		//	m_HighSprite->SetDrawActive(false);
		//	//m_StageSprite->SetDrawActive(true);
		//	break;
		//}

		for (int i = 0; i < m_Difficultys.size(); i++) {
			if (m_Count != 4 && i == m_Count - 0) {
				m_Difficultys[i]->SetDrawActive(true);
			}
			else {
				m_Difficultys[i]->SetDrawActive(false);
			}
		}
	}

	void SelectStage::OnPushA() {
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		//m_Fade->Play();
		OnDestroy();
		auto scene = App::GetApp()->GetScene<Scene>();
		scene->ChangeCountStage(m_Count);
		SoundManager::Instance().PlaySE(L"SE_HIT_ENEMY");
	}

}
