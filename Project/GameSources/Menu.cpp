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
}
//end basecross
