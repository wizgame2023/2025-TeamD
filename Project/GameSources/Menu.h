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
		void AddSprite();

		void AddSelectButton(InputData date);
		void AddAcceptButton(WORD input);

		
	};
}

//end basecross
