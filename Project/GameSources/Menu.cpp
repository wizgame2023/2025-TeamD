/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	class ScoreManager;
	void Menu::OnCreate() {
		AddTag(L"Menu");
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
	void Menu::AddAcceptButton(WORD input) {
		ButtonManager::instance->AddAcceptButton(m_GroupName, input);
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
		SoundManager::Instance().PauseBGM(true);
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
		SoundManager::Instance().PauseBGM(false);
		m_IsOpen = false;
	}

	/*void Menu::EffeckPause()
	{
		auto GetStage = GetTypeStage<GameStage>();
		auto Effect = GetStage->GetCreateEffect();
		Effect->SetEffectPause(false);
	}*/

	void PauseMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"POSE_BACK", Vec3(0, 0, 0), Vec2(700, 700), true);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SETTING", Vec3(0, 230, 0), Vec2(200, 100), true);
		AddSprite(sprite);

		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SOUND", Vec3(0, 125.0f, 0), Vec2(200, 100), true);
		sprite->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE", Vec3(0, 25.0f, 0), Vec2(200, 100), true);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"SELECT_SRAGE", Vec3(0, -75.0f, 0), Vec2(200, 100), true);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_START", Vec3(50.0f, -175.0f, 0), Vec2(300, 100), true);
		AddSprite(sprite);

		float flashSpeed = 2.0f;
		auto menu = GetThis<PauseMenu>();
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, 125.0f, 0.0f), Vec2(100, 100), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<PauseMenu>(object);
				menu->OpenSoundTest();
			});
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, 25.0f, 0.0f), Vec2(100, 100),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -75.0f, 0.0f), Vec2(100, 100),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
			});
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-150.0f, -175.0f, 0.0f), Vec2(100, 100), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<Menu>(object);
				menu->Close();
				auto getStage = menu->GetTypeStage<GameStage>();
				auto setEffect = getStage->GetCreateEffect();
				setEffect->SetEffectPause(false);
				auto getCamera = menu->OnGetDrawCamera();
				auto setCamera = static_pointer_cast<FollowCamera>(getCamera);
				setCamera->SetCameraPause(false);
			});

		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		Close();
	}

	void SoundTestMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"POSE_BACK", Vec3(0, 0, 0), Vec2(600, 600), true);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"POSE_SETTING", Vec3(0, 200, 0), Vec2(200, 100), true);
		AddSprite(sprite);

		sprite = GetStage()->AddGameObject<Sprite>(L"SE_VOLUME", Vec3(-70, 100, 0), Vec2(120, 80), true);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<Sprite>(L"BGM_VOLUME", Vec3(-60, -50, 0), Vec2(120, 80), true);
		AddSprite(sprite);

		//auto choice = GetStage()->AddGameObject<Sprite>(L"POSE_CIRCLE", Vec3(-50.0f, 85.0f, 0.0f), Vec2(60, 80), true);
		//int stick = 1;
		//auto choiceCitcle = GetStage()->AddGameObject<Sprite>(L"POSE_CIRCLE", Vec3(-30.0f, -50.0f, 0.0f), Vec2(60, 80), true);
		//AddSprite(choiceCitcle);

		auto menu = GetThis<SoundTestMenu>();
		float volumeSE = SoundManager::Instance().GetSEVolume();
		float volumeBGM = SoundManager::Instance().GetBGMVolume();
		float x = GetPositionX(volumeSE);

		auto backBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BACKBAR", Vec3(0, 30.0f, 0.0f), Vec2(300, 80), true);
		AddSprite(backBarSprite);

		auto barSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BAR", Vec3(0, 30.0f, 0.0f), Vec2(300, 80), true);
		AddSprite(barSprite);

		auto choice = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR",Vec3(x,30.0f,0.0f),Vec2(40,60),true);
		AddSprite(choice);



		//auto slideBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, 30.0f, 0.0f), Vec2(40, 60), true);
		//slideBarSprite->SetPos(Vec3(-x,0.0f,0.0f));
		//AddSprite(slideBarSprite);

		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-135.0f, 90.0f, 0.0f), Vec2(60, 80), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<SoundTestMenu>(object);
				menu->TuningSE();
				float volume = SoundManager::Instance().GetSEVolume();
				auto button = menu->GetSprite<Sprite>(6);

				Vec3 pos = button->GetPos();
				pos.x = menu->GetPositionX(volume);
				button->SetPos(pos);

			});
		x = GetPositionX(volumeBGM);

		backBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BACKBAR", Vec3(0, -130.0f, 0.0f), Vec2(300, 80), true);
		AddSprite(backBarSprite);

		barSprite = GetStage()->AddGameObject<Sprite>(L"BGM_BAR", Vec3(0, -130.0f, 0.0f), Vec2(300, 80), true);
		AddSprite(barSprite);

		choice = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, -130.0f, 0.0f), Vec2(40, 60), true);
		AddSprite(choice);

		//slideBarSprite = GetStage()->AddGameObject<Sprite>(L"BGM_SLIDEBAR", Vec3(x, -130.0f, 0.0f), Vec2(40, 60), true);
		//AddSprite(slideBarSprite);

		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-135.0f, -50.0f, 0.0f), Vec2(60, 80), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<SoundTestMenu>(object);
				menu->TuningBGM();

				float volume = SoundManager::Instance().GetBGMVolume();

				auto button = menu->GetSprite<Sprite>(9);
				Vec3 pos = button->GetPos();
				pos.x = menu->GetPositionX(volume);
				button->SetPos(pos);
			});

		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		//AddSelectButton(InputData(StickMode::LX, 1, 0.5f));
		AddAcceptButton(XINPUT_GAMEPAD_DPAD_LEFT);
		AddAcceptButton(XINPUT_GAMEPAD_DPAD_RIGHT);


		Close();
	}

	void ResultMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_BACK", Vec3(-610.0f, 320, 0), Vec2(600, 650));
		AddSprite(sprite);
		auto number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-187.0f, 230, 0), Vec2(75, 100), 2);
		AddSprite(number);
		number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-255.0f, 125, 0), Vec2(50, 100), 1);
		AddSprite(number);
		number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-190.0f, 125, 0), Vec2(75, 100), 2);
		AddSprite(number);
		number = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(-225.0f, 20, 0), Vec2(110, 100), 3);
		AddSprite(number);



		auto score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f,230.0f,0.0f), Vec2(33, 100), 1);
		int damegeRank = ScoreManager::Instance()->GetDamageRank();
		score->UpdateNumber(damegeRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f, 125.0f, 0.0f), Vec2(33, 100), 1);
		int timeRank = ScoreManager::Instance()->GetTimeRank();
		score->UpdateNumber(timeRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f, 20.0f, 0.0f), Vec2(33, 100), 1);
		int parryRank =  ScoreManager::Instance()->GetParryRank();
		score->UpdateNumber(parryRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-100.0f, -110.0f, 0.0f), Vec2(33, 90), 1);
		int totalRank = ScoreManager::Instance()->GetTotalRank();
		score->UpdateNumber(totalRank);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);
		score = GetStage()->AddGameObject<NumberSprite>(L"RESULT_SCORE", Vec3(-215.0f, 125.0f, 0.0f), Vec2(33, 100), 1);
		score->UpdateNumber(5);
		score->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(score);


		auto text = GetStage()->AddGameObject<Sprite>(L"RESULT_TEXT2", Vec3(-600, 250, 0.0f), Vec2(455, 345));
		text->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(text);
		text = GetStage()->AddGameObject<Sprite>(L"RESULT_MENU", Vec3(-400, 330, 0.0f), Vec2(130, 130));
		text->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(text);
		text = GetStage()->AddGameObject<Sprite>(L"RESULT_SCORE2", Vec3(-600, -100, 0.0f), Vec2(120, 120));
		text->SetDiffuse(Col4(0, 0, 0, 1));
		AddSprite(text);

		auto  choices = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE", Vec3(-560.0f, -240.0f, 0.0f), Vec2(180, 80));
		AddSprite(choices);
		choices = GetStage()->AddGameObject<Sprite>(L"POSE_START", Vec3(-320.0f, -240.0f, 0.0f), Vec2(220, 80));
		AddSprite(choices);
		choices = GetStage()->AddGameObject<Sprite>(L"RESULT_NEXT_STAGE", Vec3(-210.0f, -230.0f, 0.0f), Vec2(210, 100));
		AddSprite(choices);

		//タイトル
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-580.0f, -280.0f, 0.0f), Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});

		//セレクト
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-340.0f, -280.0f, 0.0f), Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
			});
		//次のステージ
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-220.0f, -280.0f, 0.0f), Vec2(50, 70),
			[](shared_ptr<ObjectInterface> object) {
				auto scene = App::GetApp()->GetScene<Scene>();
				int count = scene->GetCount();
				auto stage = static_pointer_cast<Stage>(object);
				if (count == scene->GetMaxCount()) {
					scene->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToSelectStage");
				}
				else {
					scene->ChangeCountStage(++count);
				}

			});

		AddSelectButton(InputData(StickMode::LX, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		Close();
	}
	void ResultMenu::Open() {
		Menu::Open();
		auto damage = static_pointer_cast<NumberSprite>(m_MenuObjects[1]);
		auto time_min = static_pointer_cast<NumberSprite>(m_MenuObjects[2]);
		auto time_sec = static_pointer_cast<NumberSprite>(m_MenuObjects[3]);
		auto parry = static_pointer_cast<NumberSprite>(m_MenuObjects[4]);

		int minute = ScoreManager::Instance()->GetTime() / 60;
		int second = static_cast<int>(ScoreManager::Instance()->GetTime()) % 60;
		time_min->UpdateNumber(minute);
		time_sec->UpdateNumber(second);
		parry->UpdateNumber(ScoreManager::Instance()->GetParryCount());
		damage->UpdateNumber(ScoreManager::Instance()->GetDamage());

		auto damageRank = static_pointer_cast<NumberSprite>(m_MenuObjects[5]);
		auto timeRank = static_pointer_cast<NumberSprite>(m_MenuObjects[6]);
		auto parryRank = static_pointer_cast<NumberSprite>(m_MenuObjects[7]);
		auto totalRank = static_pointer_cast<NumberSprite>(m_MenuObjects[7]);

		damageRank->UpdateNumber(ScoreManager::Instance()->GetDamageRank());
		timeRank->UpdateNumber(ScoreManager::Instance()->GetTimeRank());
		parryRank->UpdateNumber(ScoreManager::Instance()->GetParryRank());
		totalRank->UpdateNumber(ScoreManager::Instance()->GetTotalRank());
	}

	void GameOverMenu::OnCreate() {
		Menu::OnCreate();
		auto sprite = GetStage()->AddGameObject<Sprite>(L"RESULT_BACK", Vec3(-610.0f, 350, 0), Vec2(1200, 700));
		sprite->SetDiffuse(Col4(1, 1, 1, 1));
		AddSprite(sprite);
		auto text = GetStage()->AddGameObject<Sprite>(L"GAMEOVER_TEXT", Vec3(-400, 340, 0.0f), Vec2(800, 400));
		text->SetDiffuse(Col4(1, 1, 1, 1));
		AddSprite(text);
		auto gameOverSprite = GetStage()->AddGameObject<Sprite>(L"RESULT_TITLE",Vec3(-220.0f,-180.0f,0.0f),Vec2(250,150));
		AddSprite(gameOverSprite);
		gameOverSprite = GetStage()->AddGameObject<Sprite>(L"POSE_START", Vec3(150.0f, -180.0f, 0.0f), Vec2(280, 130));
		AddSprite(gameOverSprite);

		//タイトル
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(-240.0f, -250.0f, 0.0f), Vec2(70, 90),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//セレクト
		//AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(-50.0f, -250.0f, 0.0f), Vec2(250, 120),
		//	[](shared_ptr<ObjectInterface> object) {

		//	});
		//リスタート
		AddButton(L"POSE_CIRCLE", Col4(1, 1, 1, 1), Vec3(120.0f, -250.0f, 0.0f), Vec2(70, 90),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				auto scene = App::GetApp()->GetScene<Scene>();		
				scene->ChangeCountStage(scene->GetCount());
			});

		AddSelectButton(InputData(StickMode::LX, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		Close();
	}
	void GameOverMenu::Open() {
		Menu::Open();
	}


}
