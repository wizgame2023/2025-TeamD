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
		wstring texPath = mediaPath + L"Textures/";
		app->RegisterTexture(L"SELECTSPRITE", uiPath + L"SelectStageToTitle.png");
		app->RegisterTexture(L"SELECT_TITLE", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"SELECT_STAGE", uiPath + L"SelectStage.png");
		app->RegisterTexture(L"SELECT_STAGE_WHITE", uiPath + L"SelectStageWhite.png");
		app->RegisterTexture(L"SELECT_GOING", uiPath + L"SelectStageToGoing.png");
		app->RegisterTexture(L"SELECT_GOING2", uiPath + L"StageToSelectGoing.png");
		app->RegisterTexture(L"SELECT_NUMBER", uiPath + L"Number.png");
		app->RegisterTexture(L"BACKGROUND", texPath + L"TitleBackGround.png");
		//app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
		app->RegisterTexture(L"DANGER_LOW", uiPath + L"Danger_Low.png");
		app->RegisterTexture(L"DANGER_MIDDLE", uiPath + L"Danger_Middle.png");
		app->RegisterTexture(L"DANGER_HIGH", uiPath + L"Danger_High.png");

		app->RegisterTexture(L"SELECT_EASY", uiPath + L"SelectEasy.png");
		app->RegisterTexture(L"SELECT_NORMAL", uiPath + L"SelectNormal.png");
		app->RegisterTexture(L"SELECT_HARD", uiPath + L"SelectHard.png");

		app->RegisterTexture(L"SELECT_BACK_STAGE", uiPath + L"StageBack.png");
		app->RegisterTexture(L"SELECT_BACK_SELCT", uiPath + L"Result_Back_Sel.png");
		app->RegisterTexture(L"SELECT_BACK", uiPath + L"Result_Back.png");
		app->RegisterTexture(L"POSE_CIRCLE", uiPath + L"SelectCircle_Menu.png");

		app->RegisterTexture(L"CHECKMARK", uiPath + L"Check_ClearStage.png");
		app->RegisterTexture(L"SELECT_START", uiPath + L"BackSelect.png");
		app->RegisterTexture(L"SELECT_START2", uiPath + L"BackGame.png");
		app->RegisterTexture(L"POSE_CIRCLE", uiPath + L"SelectCircle_Menu.png");

		app->RegisterTexture(L"SELECT_LFFT_RIGHT", uiPath + L"Select_UI_Left_Right.png");
		app->RegisterTexture(L"SELECT_UP_DOWN", uiPath + L"Select_UI_Up_Down.png");
	}

	void SelectStage::CreateSelect() {
		int cityCount = 3;
		int levelCount = 3;
		float leftX = -340.0f;

		auto backGround = AddGameObject<Sprite>(L"BACKGROUND", Vec3(0.0f, 0.0f, 0.0f), Vec2(1280.0f, 800.0f), true);
		//auto stageGo = AddGameObject<Sprite>(L"SELECT_GOING", Vec3(-200, -300, 0), Vec2(200, 100), true);
		//auto atageReturn = AddGameObject<Sprite>(L"SELECT_START", Vec3(200, -300, 0), Vec2(200, 100), true);
		AddGameObject<ButtonManager>();
		ButtonManager::instance->SetSound(L"SE_ACCEPT");

		m_UDselect = AddGameObject<Sprite>(L"SELECT_UP_DOWN", Vec3(550.0f, -300.0f, 0.0f), Vec2(500.0f, 500.0f), true);
		m_LRselect = AddGameObject<Sprite>(L"SELECT_LFFT_RIGHT", Vec3(550.0f, -310.0f, 0.0f), Vec2(550.0f, 550.0f), true);
		m_UDselect->SetDrawActive(false);
		m_LRselect->SetDrawActive(true);

		vector<wstring> dangerKey = { L"DANGER_LOW",L"DANGER_MIDDLE",L"DANGER_HIGH" };
		vector<Col4> dangerColor = { Col4(0,1,0,1),Col4(1,1,0,1),Col4(1,0,0,1) };
		auto& scene = App::GetApp()->GetScene<Scene>();

		for (int i = 0; i < cityCount; i++) {
			Vec3 position = Vec3(leftX - leftX * i, 0, 0);
			ButtonManager::Create(GetThis<Stage>(), L"City", L"SELECT_BACK", L"SELECT_BACK_SELCT",
				position , Vec2(212.5f,335.0f) * 1.5f, 
				[position](shared_ptr<ObjectInterface> object) {
					auto stage = static_pointer_cast<SelectStage>(object);
					stage->AcceptStage(ButtonManager::instance->GetSelectIndex(L"City"));
					auto white = stage->AddGameObject<Sprite>(L"SELECT_STAGE_WHITE", position + Vec3(0.0f, 175.0f, 0.0f), Vec2(150.0f, 50.0f), true);
					auto slide = stage->AddGameObject<SlideInSprite>(L"SELECT_BACK_STAGE", true, Vec2(150.0f, 50.0f), position + Vec3(0.0f, 175.0f, 0.0f), 1.0f);
					white->SetDrawLayer(1);
					slide->SetDiffuse(Col4(0,0,0,1));
				});

			AddGameObject<Sprite>(L"SELECT_STAGE", position + Vec3(0.0f, 175.0f, 0.0f), Vec2(150.0f, 50.0f), true);
			Vec3 dangerLow = Vec3(leftX - leftX * i, 90.0f, 0.0f);
			for (int j = 0; j < dangerKey.size(); j++) {
				ButtonManager::Create(GetThis<Stage>(), L"Difficulty" + to_wstring(i), L"POSE_CIRCLE", Col4(1, 1, 1, 1),
					dangerLow - Vec3(90.0f, 100.0f * j, 0.0f), Vec2(60.0f, 60.0f),
					[](shared_ptr<ObjectInterface> object) {
						auto stage = static_pointer_cast<SelectStage>(object);
						size_t selectCity = ButtonManager::instance->GetSelectIndex(L"City");
						stage->AcceptDifficulty(static_cast<int>(ButtonManager::instance->GetSelectIndex(L"Difficulty" + to_wstring(selectCity))));
					});

				auto sprite = AddGameObject<Sprite>(dangerKey[j], dangerLow - Vec3(0,100.0f * j,0.0f), Vec2(60.0f, 60.0f), true);
				sprite->SetDiffuse(dangerColor[j]);

				if (scene->IsClear({ i,j })) {
					sprite = AddGameObject<Sprite>(L"CHECKMARK", dangerLow - Vec3(-100.0f, 100.0f * j, 0.0f), Vec2(60.0f, 60.0f),true);
				}
			}
			ButtonManager::instance->AddAcceptButton(L"Difficulty" + to_wstring(i), XINPUT_GAMEPAD_A);
			ButtonManager::instance->AddKeyboradAccept(L"Difficulty" + to_wstring(i), VK_SPACE);

			ButtonManager::instance->SetInput(L"Difficulty" + to_wstring(i), InputData(StickMode::LY, 1, 0.1f));
			ButtonManager::instance->SetKeyborad(L"Difficulty" + to_wstring(i), InputData('W', -1));
			ButtonManager::instance->SetKeyborad(L"Difficulty" + to_wstring(i), InputData('S', 1));

		}
		ButtonManager::Create(GetThis<Stage>(), L"Accept", L"SELECT_GOING2", L"SELECT_GOING",
			Vec3(-200, -300, 0), Vec2(200, 100), [](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<SelectStage>(object);
				stage->StartStage();
				ButtonManager::instance->OpenAndUse(L"Accept");
			});
		ButtonManager::Create(GetThis<Stage>(), L"Accept", L"SELECT_START2", L"SELECT_START",
			Vec3(200, -300, 0), Vec2(200, 100), [](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<SelectStage>(object);
				stage->Select();
			});



		ButtonManager::instance->SetInput(L"City", InputData(StickMode::LX, 1, 0.1f));
		ButtonManager::instance->SetKeyborad(L"City", InputData('A', -1));
		ButtonManager::instance->SetKeyborad(L"City", InputData('D', 1));


		ButtonManager::instance->SetInput(L"Accept", InputData(StickMode::LX, 1, 0.1f));
		ButtonManager::instance->SetKeyborad(L"Accept", InputData('A', -1));
		ButtonManager::instance->SetKeyborad(L"Accept", InputData('D', 1));


		ButtonManager::instance->AddAcceptButton(L"City",XINPUT_GAMEPAD_A);
		ButtonManager::instance->AddKeyboradAccept(L"City", VK_SPACE);

		ButtonManager::instance->AddAcceptButton(L"Accept", XINPUT_GAMEPAD_A);
		ButtonManager::instance->AddKeyboradAccept(L"Accept", VK_SPACE);


		ButtonManager::instance->UseGroup(L"City");
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
	}

	void SelectStage::OnPushA() {
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		//m_Fade->Play();
		/*OnDestroy();
		auto scene = App::GetApp()->GetScene<Scene>();
		scene->ChangeCountStage(m_Count);
		SoundManager::Instance().PlaySE(L"SE_HIT_ENEMY");*/

	}

	void SelectStage::AcceptStage(int index) {
		m_StageNumber = index;
		ButtonManager::instance->UseGroup(L"Difficulty" + to_wstring(index));
		m_UDselect->SetDrawActive(true);
		m_LRselect->SetDrawActive(false);
	}
	void SelectStage::AcceptDifficulty(int index) {
		m_DifficultyLevel = index;
		ButtonManager::instance->SetSelectIndex(L"", 0);
		ButtonManager::instance->UseGroup(L"Accept");
		m_UDselect->SetDrawActive(false);
		m_LRselect->SetDrawActive(true);
	}
	void SelectStage::StartStage() {

		auto data = make_shared<StageData>(StageData{m_StageNumber,m_DifficultyLevel });
		PostEvent(0.0f, nullptr, App::GetApp()->GetScene<Scene>(), L"ToGameStage", data);
	}
	void SelectStage::Select() {
		PostEvent(0.0f, nullptr, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
	}
}
