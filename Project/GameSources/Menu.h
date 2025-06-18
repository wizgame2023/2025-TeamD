/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Sprite.h"

namespace basecross {
	class Menu : public GameObject {
		bool m_IsOpen;
		bool m_IsPouse;
	protected:
		vector<shared_ptr<GameObject>> m_MenuObjects;
		shared_ptr<Stage> m_Stage;
		shared_ptr<EffectManeger> m_Effect;
		shared_ptr<FollowCamera>m_Camera;
		wstring m_GroupName;
	public:
		Menu(const shared_ptr<Stage>& stage, const wstring& group) : GameObject(stage),m_Stage(stage), m_GroupName(group), m_IsOpen(false),m_IsPouse(false) {}
		virtual ‾Menu() {}
		virtual void OnCreate()override;
		void AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func);
		void AddButton(const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);
		void AddButton(const wstring& defaultTex, const Col4& selectColor, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func);
		void AddButton(const wstring& defaultTex, const Col4& selectColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);

		template<class Comp, typename... params>
		void AddButton(const wstring& defaultTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func, params&&... param) {
			ButtonManager::Create<Comp>(GetStage(), m_GroupName, defaultTex, pos, size, object, func, param...);
		}
		template<class Comp, typename... params>
		void AddButton(const wstring& defaultTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func, params&&... param) {
			ButtonManager::Create<Comp>(GetStage(), m_GroupName, defaultTex, pos, size, func, param...);
		}

		template<typename T>
		shared_ptr<T>GetSprite(int number){
			return static_pointer_cast<T>(m_MenuObjects[number]);
		}

		void AddSprite(const shared_ptr<GameObject>& sprite) {
			sprite->AddTag(L"Menu");
			m_MenuObjects.push_back(sprite);
		}

		void AddSelectButton(InputData date);
		void AddAcceptButton(WORD input);

		virtual void Close();
		virtual void Open();

		bool IsOpen() {
			return m_IsOpen;
		}
		void SetIsPouse(bool flag) {
			m_IsPouse = true;
		}
	};

	class PauseMenu : public Menu {
		shared_ptr<Menu> m_SoundTestMenu;
	public:
		PauseMenu(const shared_ptr<Stage>& stage, const wstring& group, shared_ptr<Menu>& menu) : Menu(stage, group), m_SoundTestMenu(menu) {}
		virtual ‾PauseMenu() {}
		shared_ptr<GameStage> m_Stage;

		virtual void OnCreate()override;
		void OpenSoundTest() {
			Close();
			m_SoundTestMenu->Open();
		}
	};
	class SoundTestMenu : public Menu {
		float m_LeftX;
		float m_RightX;
		shared_ptr<Menu> m_PauseMenu;
	public:
		SoundTestMenu(const shared_ptr<Stage>& stage, const wstring& group) : Menu(stage, group),m_LeftX(-100),m_RightX(100){}
		virtual ‾SoundTestMenu() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void TuningSE();
		void TuningBGM();

		float GetPositionX(float volume) {
			return m_LeftX + abs(m_LeftX - m_RightX) * volume;
		}


		void SetPauseMenu(const shared_ptr<Menu>& menu) {
			m_PauseMenu = menu;
		}

		void OpenPauseMenu() {
			Close();
			m_PauseMenu->Open();
		}

	};

	class ResultMenu : public Menu {
	public:
		ResultMenu(const shared_ptr<Stage>& stage, const wstring& group) : Menu(stage, group) {}
		virtual ‾ResultMenu() {}
		virtual void OnCreate()override;

		virtual void Open();
	};

	class GameOverMenu : public Menu {
	public:
		GameOverMenu(const shared_ptr<Stage>& stage, const wstring& group) : Menu(stage, group) {}
		virtual ‾GameOverMenu() {}
		virtual void OnCreate()override;

		virtual void Open();
	};
}

//end basecross
