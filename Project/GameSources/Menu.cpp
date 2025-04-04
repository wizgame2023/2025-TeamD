/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Menu::AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<Stage>&)> func) {
		ButtonManager::Create(GetThis<Stage>(), m_GroupName, defaultTex, selectedTex, pos, size, func);
	}
	void Menu::AddSelectButton(InputData date) {
		ButtonManager::instance->SetInput(m_GroupName, date);
	}
	void Menu::AddAcceptButton(WORD input) {
		ButtonManager::instance->AddAcceptButton(m_GroupName, input);
	}
	void Menu::Open() {
		for (auto& obj : m_MenuObjects) {
			obj->SetDrawActive(false);
			obj->SetUpdateActive(false);
		}
		ButtonManager::instance->OpenAndUse(m_GroupName);
	}
	void Menu::Close() {
		for (auto& obj : m_MenuObjects) {
			obj->SetDrawActive(false);
			obj->SetUpdateActive(false);
		}
		ButtonManager::instance->Close(m_GroupName);
	}

	void PauseMenu::OnCreate() {

		AddButton(L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),std::bind(&PauseMenu::Close,GetThis<PauseMenu>())
			/*[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				currentStage->ClosePose();
			}*/);
		AddButton(L"POSE_SOUND", L"POSE_SOUND_SELECTED", Vec3(0.0f, -150.0f, 0.0f), Vec2(200, 50), std::bind(&PauseMenu::OpenSoundTest, GetThis<PauseMenu>())
			/*[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				ButtonManager::instance->Close(L"POSE");
				ButtonManager::instance->OpenAndUse(L"SOUND_TEST");
			}*/);
		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);

		Close();
	}

	void SoundTestMenu::OnCreate() {
		/*
		* //SE
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"SE_VOLUME", L"SE_VOLUME_SELECTED", Vec3(0.0f, 0.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().SEVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().SEVolumeDown(0.1f);
				}
			});
		//BGM
		ButtonManager::Create(GetThis<Stage>(), L"SOUND_TEST", L"BGM_VOLUME", L"BGM_VOLUME_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
				if (press & XINPUT_GAMEPAD_DPAD_UP) {
					SoundManager::Instance().BGMVolumeUp(0.1f);
				}
				else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
					SoundManager::Instance().BGMVolumeDown(0.1f);
				}
			});

		ButtonManager::instance->SetInput(L"SOUND_TEST", InputData(StickMode::LY, 1, 0.1f));
		ButtonManager::instance->AddAcceptButton(L"SOUND_TEST", XINPUT_GAMEPAD_DPAD_UP);
		ButtonManager::instance->AddAcceptButton(L"SOUND_TEST", XINPUT_GAMEPAD_DPAD_DOWN);

		ButtonManager::instance->Close(L"SOUND_TEST");
		*/
		AddButton(L"POSE_TITLE", L"POSE_TITLE_SELECTED", Vec3(0.0f, 150.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_ENDGAME", L"POSE_ENDGAME_SELECTED", Vec3(0.0f, 50.0f, 0.0f), Vec2(200, 50),
			[](shared_ptr<Stage> stage) {
				stage->PostEvent(0.0f, stage, App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
			});
		AddButton(L"POSE_START", L"POSE_START_SELECTED", Vec3(0.0f, -50.0f, 0.0f), Vec2(200, 50), std::bind(&PauseMenu::Close, GetThis<PauseMenu>())
			/*[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				currentStage->ClosePose();
			}*/);
		AddButton(L"POSE_SOUND", L"POSE_SOUND_SELECTED", Vec3(0.0f, -150.0f, 0.0f), Vec2(200, 50), std::bind(&PauseMenu::OpenSoundTest, GetThis<PauseMenu>())
			/*[](shared_ptr<Stage> stage) {
				auto currentStage = static_pointer_cast<GameStage>(stage);
				ButtonManager::instance->Close(L"POSE");
				ButtonManager::instance->OpenAndUse(L"SOUND_TEST");
			}*/);
		AddSelectButton(InputData(StickMode::LY, 1, 0.1f));
		AddAcceptButton(XINPUT_GAMEPAD_A);
	}
}
//end basecross
