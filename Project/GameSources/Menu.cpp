/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	vector<weak_ptr<Menu>> Menu::m_ActiveMenus = {};

	void Menu::OnCreate() {
		AddTag(L"Menu");

		m_ActiveMenus.push_back(GetThis<Menu>());
	}
	void Menu::AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectedTex, pos, size, func);
	}
	void Menu::AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectedTex, pos, size, object, func);
	}
	void Menu::AddButton(const wstring& defaultTex, const Col4& selectColor, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectColor, pos, size, func);
	}
	void Menu::AddButton(const wstring& defaultTex, const Col4& selectColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectColor, pos, size, object, func);
	}


	void Menu::AddSelectButton(InputData date) {
		ButtonManager::instance->SetInput(m_GroupName, date);
	}
	void Menu::AddKeyboradSelect(InputData date) {
		ButtonManager::instance->SetKeyborad(m_GroupName, date);
	}
	void Menu::AddAcceptButton(WORD input) {
		ButtonManager::instance->AddAcceptButton(m_GroupName, input);
	}
	void Menu::AddKeyboradAccept(WORD input) {
		ButtonManager::instance->AddKeyboradAccept(m_GroupName, input);
	}
	void Menu::Open() {
		for (auto& obj : m_MenuObjects) {
			obj->SetDrawActive(true);
			obj->SetUpdateActive(true);
		}
		if (m_IsPouse) {
			static_pointer_cast<GameStage>(m_Stage)->SetAllGameObjectActive(false);
		}
		ButtonManager::instance->OpenAndUse(m_GroupName);
		SoundManager::GetInstance().PauseBGM(true);
		m_IsOpen = true;
	}
	void Menu::Close() {
		for (auto& obj : m_MenuObjects) {
			obj->SetDrawActive(false);
			//obj->SetUpdateActive(false);
		}
		//EffeckPause();
		if (m_IsPouse) {
			static_pointer_cast<GameStage>(m_Stage)->SetAllGameObjectActive(true);
		}
		ButtonManager::instance->Close(m_GroupName);
		SoundManager::GetInstance().PauseBGM(false);
		m_IsOpen = false;
	}

	/*void Menu::EffeckPause()
	{
		auto GetStage = GetTypeStage<GameStage>();
		auto Effect = GetStage->GetCreateEffect();
		Effect->SetEffectPause(false);
	}*/
	void PauseMenu::OnUpdate() {
		//auto& app = App::GetApp();
		//auto& device = app->GetInputDevice().GetControlerVec()[0];
		//if (device.wPressedButtons & XINPUT_GAMEPAD_START) {
		//	if (PauseMenu::IsOpen()) {
		//		Close();
		//	}
		//}
	}

	void PauseMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"POSE_BACK", Vec3(0, 10, 0), Vec2(600, 600), Vec2(0.5f));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SETTING", Vec3(0, 220, 0), Vec2(200, 100), Vec2(0.5f));
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);

		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SOUND2", Vec3(35.0f, 105.0f, 0), Vec2(280, 110), Vec2(0.5f));
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE3", Vec3(35.0f, - 135.0f, 0), Vec2(280, 110), Vec2(0.5f));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"SELECT_SRAGE", Vec3(35.0f, -15.0f, 0), Vec2(280, 110), Vec2(0.5f));
		AddSprite(sprite);
		//sprite = GetStage()->AddGameObject<Sprite>(L"POSE_START2", Vec3(35.0f, -175.0f, 0), Vec2(280, 110), Vec2(0.5f));
		//AddSprite(sprite);

		float flashSpeed = 2.0f;
		auto menu = GetThis<PauseMenu>();
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, 105.0f, 0.0f), Vec2(100, 100), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<PauseMenu>(object);
				menu->OpenSoundTest();
			});
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -15.0f, 0.0f), Vec2(100, 100),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
			});
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -135.0f, 0.0f), Vec2(100, 100),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});

		//AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -175.0f, 0.0f), Vec2(100, 100), menu,
		//	[](shared_ptr<ObjectInterface> object) {
		//		auto menu = static_pointer_cast<Menu>(object);
		//		auto getStage = menu->GetTypeStage<GameStage>();
		//		auto player = getStage->GetSharedGameObject<Player>(L"Player", false);
		//		menu->Close();
		//		auto setEffect = getStage->GetCreateEffect();
		//		setEffect->SetEffectPause(false);
		//		auto getCamera = menu->OnGetDrawCamera();
		//		auto setCamera = static_pointer_cast<FollowCamera>(getCamera);
		//		setCamera->SetCameraPause(false);
		//		player->SetIsGaol(false);
		//	});

		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		AddKeyboradSelect(InputData('W', -1));
		AddKeyboradSelect(InputData('S', 1));
		AddKeyboradAccept(VK_SPACE);
		Close();

		
	}
	void SoundTestMenu::TuningSE() {
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		auto& keyState = App::GetApp()->GetInputDevice().GetKeyState();
		WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
		if (cntlVec.bConnected) {
			if (cntlVec.fThumbLX > 0.5f) {
				SoundManager::GetInstance().SEVolumeUp(0.01f);
			}
			else if (cntlVec.fThumbLX < -0.5f) {
				SoundManager::GetInstance().SEVolumeDown(0.01f);
			}
		}
		if (keyState.m_bPushKeyTbl['D']) {
			SoundManager::GetInstance().SEVolumeUp(0.01f);
		}
		else if (keyState.m_bPushKeyTbl['A']) {
			SoundManager::GetInstance().SEVolumeDown(0.01f);
		}
		float volume = SoundManager::GetInstance().GetSEVolume();
		auto button = GetSprite<Sprite>(9);
		Vec3 pos = button->GetPosition();
		pos.x = GetPositionX(volume);
		button->SetPosition(pos);

	}
	void SoundTestMenu::TuningBGM() {
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		auto& keyState = App::GetApp()->GetInputDevice().GetKeyState();
		WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
		if (cntlVec.bConnected) {
			if (cntlVec.fThumbLX > 0.5f) {
				SoundManager::GetInstance().BGMVolumeUp(0.01f);
			}
			else if (cntlVec.fThumbLX < -0.5f) {
				SoundManager::GetInstance().BGMVolumeDown(0.01f);
			}
		}
		if (keyState.m_bPushKeyTbl['D']) {
			SoundManager::GetInstance().BGMVolumeUp(0.01f);
		}
		else if (keyState.m_bPushKeyTbl['A']) {
			SoundManager::GetInstance().BGMVolumeDown(0.01f);
		}
		float volume = SoundManager::GetInstance().GetBGMVolume();
		auto button = GetSprite<Sprite>(6);
		Vec3 pos = button->GetPosition();
		pos.x = GetPositionX(volume);
		button->SetPosition(pos);

	}

	void SoundTestMenu::OnUpdate() {
		if (!IsOpen()) return;
		if (ButtonManager::instance->GetSelectIndex(L"SOUND_TEST") == 0) {
			TuningBGM();
		}
		else if(ButtonManager::instance->GetSelectIndex(L"SOUND_TEST") == 1) {
			TuningSE();
		}

	}
	void SoundTestMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"POSE_BACK", Vec3(0, 10, 0), Vec2(600, 600), Vec2(0.5f));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SETTING", Vec3(0, 220, 0), Vec2(200, 100), Vec2(0.5f));
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);		

		sprite = GetStage()->AddGameObject<Sprite>(L"SE_VOLUME", Vec3(-70, -70, 0), Vec2(120, 80), Vec2(0.5f));
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"BGM_VOLUME", Vec3(-60, 105, 0), Vec2(120, 80), Vec2(0.5f));
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);

		//auto choice = GetStage()->AddGameObject<Sprite>(L"POSE_CIRCLE", Vec3(-50.0f, 85.0f, 0.0f), Vec2(60, 80), true);
		//int stick = 1;
		//auto choiceCitcle = GetStage()->AddGameObject<Sprite>(L"POSE_CIRCLE", Vec3(-30.0f, -50.0f, 0.0f), Vec2(60, 80), true);
		//AddSprite(choiceCitcle);

		auto menu = GetThis<SoundTestMenu>();
		float volumeSE = SoundManager::GetInstance().GetSEVolume();
		float volumeBGM = SoundManager::GetInstance().GetBGMVolume();
		float x = GetPositionX(volumeBGM);

		auto backBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BACKBAR", Vec3(0, 35.0f, 0.0f), Vec2(300, 80), Vec2(0.5f));
		AddSprite(backBarSprite);

		auto barSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BAR", Vec3(0, 35.0f, 0.0f), Vec2(300, 80), Vec2(0.5f));
		AddSprite(barSprite);

		auto choice = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, 35.0f, 0.0f), Vec2(40, 60), Vec2(0.5f));
		AddSprite(choice);


		//auto slideBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, 30.0f, 0.0f), Vec2(40, 60), true);
		//slideBarSprite->SetPosition(Vec3(-x,0.0f,0.0f));
		//AddSprite(slideBarSprite);

		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, 105.0f, 0.0f), Vec2(100, 100), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<SoundTestMenu>(object);
				menu->TuningBGM();

				float volume = SoundManager::GetInstance().GetBGMVolume();

				auto button = menu->GetSprite<Sprite>(9);
				Vec3 pos = button->GetPosition();
				pos.x = menu->GetPositionX(volume);
				button->SetPosition(pos);
			});
		x = GetPositionX(volumeSE);

		backBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BACKBAR", Vec3(0, -140.0f, 0.0f), Vec2(300, 80), Vec2(0.5f));
		AddSprite(backBarSprite);

		barSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BAR", Vec3(0, -140.0f, 0.0f), Vec2(300, 80), Vec2(0.5f));
		AddSprite(barSprite);

		choice = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, -140.0f, 0.0f), Vec2(40, 60), Vec2(0.5f));
		AddSprite(choice);

		//slideBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, -130.0f, 0.0f), Vec2(40, 60), true);
		//AddSprite(slideBarSprite);

		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -70.0f, 0.0f), Vec2(100, 100), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<SoundTestMenu>(object);
				menu->TuningSE();
				float volume = SoundManager::GetInstance().GetSEVolume();
				auto button = menu->GetSprite<Sprite>(6);

				Vec3 pos = button->GetPosition();
				pos.x = menu->GetPositionX(volume);
				button->SetPosition(pos);
			});
		//auto sprited = GetStage()->AddGameObject<Sprite>(L"POSE_START2", Vec3(35.0f, -175.0f, 0), Vec2(280, 110), Vec2(0.5f));
		//AddSprite(sprited);

		//AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -175.0f, 0.0f), Vec2(100, 100), menu,
		//	[](shared_ptr<ObjectInterface> object) {
		//		auto menu = static_pointer_cast<SoundTestMenu>(object);
		//		menu->OpenPauseMenu();
		//		auto getStage = menu->GetTypeStage<GameStage>();
		//		auto setEffect = getStage->GetCreateEffect();
		//		setEffect->SetEffectPause(false);
		//		auto getCamera = menu->OnGetDrawCamera();
		//		auto setCamera = static_pointer_cast<FollowCamera>(getCamera);
		//		setCamera->SetCameraPause(false);

		//	});

		AddSelectButton(InputData(StickMode::LY, 1, 0.2f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		AddKeyboradSelect(InputData('W', -1));
		AddKeyboradSelect(InputData('S', 1));
		AddKeyboradAccept(VK_SPACE);

		Close();
	}

	void ResultMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_BACK", Vec3(-650.0f, 350, 0), Vec2(700, 750));
		AddSprite(sprite);
		//auto number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-187.0f, 230, 0), Vec2(75, 100), 2);
		//AddSprite(number);
		auto number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-220.0f, 150, 0), Vec2(50, 100), 1);
		AddSprite(number);
		number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-155.0f, 150, 0), Vec2(75, 100), 2);
		AddSprite(number);
		number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-190.0f, 0, 0), Vec2(110, 100), 3);
		AddSprite(number);


		auto menu = GetThis<ResultMenu>();

		//auto score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f,230.0f,0.0f), Vec2(33, 100), 1);
		////int damegeRank = ScoreManager::Instance()->GetDamageRank();
		////score->UpdateNumber(damegeRank);
		//score->SetDiffuse(Col4(0, 0, 0, 0));
		//AddSprite(score);
		//AddSprite(score);
		auto score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-60.0f, 150.0f, 0.0f), Vec2(33, 100), 1);
		int timeRank = ScoreManager::Instance()->GetTimeRank();
		score->UpdateNumber(timeRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-60.0f, 0.0f, 0.0f), Vec2(33, 100), 1);
		int parryRank =  ScoreManager::Instance()->GetParryRank();
		score->UpdateNumber(parryRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		//score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f, -110.0f, 0.0f), Vec2(33, 90), 1);
		////int totalRank = ScoreManager::Instance()->GetTotalRank();
		////score->UpdateNumber(totalRank);
		//score->SetDiffuse(Col4(0, 0, 0, 0));
		//AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-180.0f, 150.0f, 0.0f), Vec2(33, 100), 1);
		score->UpdateNumber(5);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);


		auto text = GetStage()->AddGameObject<Sprite>(L"RESULT_TEXT", Vec3(-600, 180, 0.0f), Vec2(455, 345));
		text->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(text);
		text = GetStage()->AddGameObject<Sprite>(L"RESULT_MENU", Vec3(-360, 330, 0.0f), Vec2(130, 130));
		text->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(text);
		//text = GetStage()->AddGameObject<Sprite>(L"RESULT_SCORE2", Vec3(-600, -100, 0.0f), Vec2(120, 120));
		//text->SetDiffuse(Col4(0, 0, 0, 1));
		//AddSprite(text);

		auto  choices = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE", Vec3(-150.0f, -225.0f, 0.0f) /*Vec3(-585.0f, -225.0f, 0.0f)*/, Vec2(160, 70));
		AddSprite(choices);
		choices = GetStage()->AddGameObject<Sprite>(L"SELECT_RESULT", Vec3(-345.0f, -225.0f, 0.0f), Vec2(160, 70));
		AddSprite(choices);
		choices = GetStage()->AddGameObject<Sprite>(L"RESULT_NEXT_STAGE",Vec3(-585.0f, -210.0f, 0.0f)/* Vec3(-210.0f, -210.0f, 0.0f)*/, Vec2(210, 100));
		AddSprite(choices);


		//次のステージ
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-600.0f, -260.0f, 0.0f) /*Vec3(-220.0f, -260.0f, 0.0f)*/, Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				//auto gamestage = static_pointer_cast<GameStage>(object);
				auto stage = static_pointer_cast<Stage>(object);
				auto scene = App::GetApp()->GetScene<Scene>();
				auto gameStage = dynamic_pointer_cast<GameStage>(stage);

				auto data = gameStage->GetStageData();
				auto shareddata = make_shared<StageData>(data);
				shareddata->stageNum += 1;
				stage->PostEvent(0.0f, stage, scene, L"ToGameStage", shareddata);
			});
		//セレクト
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-355.0f, -260.0f, 0.0f), Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
			});
		//タイトル
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), /*Vec3(-600.0f, -260.0f, 0.0f)*/Vec3(-160.0f, -260.0f, 0.0f), Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});

		AddSelectButton(InputData(StickMode::LX, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		AddKeyboradSelect(InputData('A', -1));
		AddKeyboradSelect(InputData('D', 1));
		AddKeyboradAccept(VK_SPACE);
		Close();
	}
	void ResultMenu::Open() {
		Menu::Open();
		//auto damage = static_pointer_cast<NumberSprite>(m_MenuObjects[1]);
		auto time_min = static_pointer_cast<NumberSprite>(m_MenuObjects[1]);
		auto time_sec = static_pointer_cast<NumberSprite>(m_MenuObjects[2]);
		auto parry = static_pointer_cast<NumberSprite>(m_MenuObjects[3]);

		int minute = static_cast<int>(ScoreManager::Instance()->GetTime()) / 60;
		int second = static_cast<int>(ScoreManager::Instance()->GetTime()) % 60;
		time_min->UpdateNumber(minute);
		time_sec->UpdateNumber(second);
		parry->UpdateNumber(ScoreManager::Instance()->GetParryCount());
		//damage->UpdateNumber(ScoreManager::Instance()->GetDamage());

		//auto damageRank = static_pointer_cast<NumberSprite>(m_MenuObjects[5]);
		auto timeRank = static_pointer_cast<NumberSprite>(m_MenuObjects[4]);
		auto parryRank = static_pointer_cast<NumberSprite>(m_MenuObjects[5]);
		//auto totalRank = static_pointer_cast<NumberSprite>(m_MenuObjects[7]);

		//damageRank->UpdateNumber(ScoreManager::Instance()->GetDamageRank());
		timeRank->UpdateNumber(ScoreManager::Instance()->GetTimeRank());
		parryRank->UpdateNumber(ScoreManager::Instance()->GetParryRank());
		//totalRank->UpdateNumber(ScoreManager::Instance()->GetTotalRank());
	}

	void GameOverMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_BACK", Vec3(-610.0f, 350, 0), Vec2(1200, 700));
		sprite->SetDiffuse(Col4(1, 1, 1, 1));
		AddSprite(sprite);
		auto text = GetStage()->AddGameObject<Sprite>(L"GAMEOVER_TEXT", Vec3(-300, 300, 0.0f), Vec2(600, 300));
		text->SetDiffuse(Col4(1, 1, 1, 1));
		AddSprite(text);
		auto gameOverSprite = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE",Vec3(-270.0f,-200.0f,0.0f),Vec2(180,90));
		AddSprite(gameOverSprite);
		gameOverSprite = GetStage()->AddGameObject<Sprite>(L"GO_RESTART", Vec3(130.0f, -200.0f, 0.0f), Vec2(180, 90));
		AddSprite(gameOverSprite);
		//タイトル
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-290.0f, -250.0f, 0.0f), Vec2(70, 90),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//セレクト
		//AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(-50.0f, -250.0f, 0.0f), Vec2(250, 120),
		//	[](shared_ptr<ObjectInterface> object) {

		//	});
		//リスタート
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(90.0f, -250.0f, 0.0f), Vec2(70, 90),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				auto scene = App::GetApp()->GetScene<Scene>();
				auto gameStage = dynamic_pointer_cast<GameStage>(stage);

				auto data = gameStage->GetStageData();
				auto shareddata = make_shared<StageData>(data);
				stage->PostEvent(0.0f, stage, scene, L"ToGameStage", shareddata);
			});

		AddSelectButton(InputData(StickMode::LX, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		AddKeyboradSelect(InputData('A', -1));
		AddKeyboradSelect(InputData('D', 1));
		AddKeyboradAccept(VK_SPACE);
		Close();
	}
	void GameOverMenu::Open() {
		Menu::Open();
	}


}
