/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Menu::AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectedTex, pos, size, func);
	}
	void Menu::AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		ButtonManager::Create(GetStage(), m_GroupName, defaultTex, selectedTex, pos, size, object, func);
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
		ButtonManager::instance->OpenAndUse(m_GroupName);
		SoundManager::Instance().PauseBGM(true);
		m_IsOpen = true;
	}
	void Menu::Close() {
		for (auto& obj : m_MenuObjects) {
			obj->SetDrawActive(false);
			obj->SetUpdateActive(false);
		}
		ButtonManager::instance->Close(m_GroupName);
		SoundManager::Instance().PauseBGM(false);
		m_IsOpen = false;
	}

	void PauseMenu::OnCreate() {
		auto sprite = GetStage()->AddGameObject<Sprite>(L"BGM_VOLUME_SELECTED", Vec3(0, 0, 0), Vec2(300, 300), true);
		AddSprite(sprite);

		auto menu = GetThis<PauseMenu>();
		AddButton(L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<Menu>(object);
				menu->Close();
			});
		AddButton(L"POSE_SOUND", L"POSE_SOUND_SELECTED", Vec3(0.0f, -150.0f, 0.0f), Vec2(200, 50), menu,
			[](shared_ptr<ObjectInterface> object) {
				auto menu = static_pointer_cast<PauseMenu>(object);
				menu->OpenSoundTest();
			});
		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		Close();
	}

	void SoundTestMenu::OnCreate() {
		auto menu = GetThis<SoundTestMenu>();

		AddButton(L"SE_VOLUME", L"SE_VOLUME_SELECTED", Vec3(0.0f, 0.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().SEVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().SEVolumeDown(0.1f);
				}
			});
		AddButton(L"BGM_VOLUME", L"BGM_VOLUME_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().BGMVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().BGMVolumeDown(0.1f);
				}
			});
		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_DPAD_UP);
		AddAcceptButton(XINPUT_GAMEPAD_DPAD_DOWN);

		
		Close();
	}

	void ResultMenu::OnCreate() {
		auto sprite = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(0, 300, 0), Vec2(200, 200), 3);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(0, 150, 0), Vec2(200, 200), 3);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(0, 0, 0), Vec2(200, 200), 3);
		AddSprite(sprite);
		sprite = GetStage()->AddGameObject<NumberSprite>(L"NUMBER", Vec3(0, -150, 0), Vec2(200, 200), 3);
		AddSprite(sprite);
		//タイトル
		AddButton(L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				auto stage = static_pointer_cast<Stage>(object);
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		//次のステージ
		AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				
			});
		//セレクト
		AddButton(L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<ObjectInterface> object) {
				
			});
		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
		Close();
	}
	void ResultMenu::Open() {
		Menu::Open();
		auto time = static_pointer_cast<NumberSprite>(m_MenuObjects[0]);
		auto eliminate = static_pointer_cast<NumberSprite>(m_MenuObjects[1]);
		auto parry = static_pointer_cast<NumberSprite>(m_MenuObjects[2]);
		auto damage = static_pointer_cast<NumberSprite>(m_MenuObjects[3]);

		time->UpdateNumber(ScoreManager::Instance()->GetTime());
		eliminate->UpdateNumber(static_cast<int>(ScoreManager::Instance()->CalculateEliminateEnemyRate()));
		parry->UpdateNumber(ScoreManager::Instance()->GetParryCount());
		damage->UpdateNumber(ScoreManager::Instance()->GetDamage());
	}
}
//end basecross
