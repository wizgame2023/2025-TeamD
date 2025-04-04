/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Menu : public GameObject {
	protected:
		vector<shared_ptr<GameObject>> m_MenuObjects;
		wstring m_GroupName;
	public:
		Menu(const shared_ptr<Stage>& stage,const wstring& group) : GameObject(stage),m_GroupName(group){}
		virtual ~Menu(){}

		void AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<Stage>&)> func);
		void AddSprite(const shared_ptr<Sprite>& sprite) {
			m_MenuObjects.push_back(sprite);
		}

		void AddSelectButton(InputData date);
		void AddAcceptButton(WORD input);

		void Close();
		void Open();
	};

	class PauseMenu : public Menu {
		shared_ptr<Menu> m_SoundTestMenu;
	public:
		PauseMenu(const shared_ptr<Stage>& stage, const wstring& group, shared_ptr<Menu>& menu) : Menu(stage, group),m_SoundTestMenu(menu) {}
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

		}
	};
}

//end basecross
