/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Menu : public GameObject {
		bool m_IsOpen;
	protected:
		vector<shared_ptr<GameObject>> m_MenuObjects;
		shared_ptr<GameStage> m_Stage;
		shared_ptr<EffectManeger> m_Effect;
		wstring m_GroupName;
	public:
		Menu(const shared_ptr<Stage>& stage, const wstring& group) : GameObject(stage), m_GroupName(group), m_IsOpen(false) {}
		virtual ~Menu() {}
		virtual void OnCreate()override;
		void AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func);
		void AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);

		void AddSprite(const shared_ptr<GameObject>& sprite) {
			sprite->AddTag(L"Menu");
			m_MenuObjects.push_back(sprite);
		}

		void AddSelectButton(InputData date);
		void AddAcceptButton(WORD input);

		virtual void Close();
		virtual void Open();
		
		void EffeckPause();

		bool IsOpen() {
			return m_IsOpen;
		}
	};

	class PauseMenu : public Menu {
		shared_ptr<Menu> m_SoundTestMenu;
	public:
		PauseMenu(const shared_ptr<Stage>& stage, const wstring& group, shared_ptr<Menu>& menu) : Menu(stage, group), m_SoundTestMenu(menu) {}
		virtual ~PauseMenu() {}

		virtual void OnCreate()override;
		void OpenSoundTest() {
			Close();
			m_SoundTestMenu->Open();
		}
	};
	class SoundTestMenu : public Menu {
	public:
		SoundTestMenu(const shared_ptr<Stage>& stage, const wstring& group) : Menu(stage, group) {}
		virtual ~SoundTestMenu() {}

		virtual void OnCreate()override;

		void TuningSE() {
			WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
			if (press & XINPUT_GAMEPAD_DPAD_UP) {
				SoundManager::Instance().SEVolumeUp(0.1f);
			}
			else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
				SoundManager::Instance().SEVolumeDown(0.1f);
			}
		}
		void TuningBGM() {
			WORD press = ButtonManager::instance->GetPressedAccept(L"SOUND_TEST");
			if (press & XINPUT_GAMEPAD_DPAD_UP) {
				SoundManager::Instance().BGMVolumeUp(0.1f);
			}
			else if (press & XINPUT_GAMEPAD_DPAD_DOWN) {
				SoundManager::Instance().BGMVolumeDown(0.1f);
			}
		}
	};

	class ResultMenu : public Menu {
	public:
		ResultMenu(const shared_ptr<Stage>& stage, const wstring& group) : Menu(stage, group) {}
		virtual ~ResultMenu() {}
		virtual void OnCreate()override;

		virtual void Open();
	};
}

//end basecross
