/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class Character : public GameObject
	{
	public:
		int m_HP;

		Character(const shared_ptr<Stage>& stage) :
			GameObject(stage) 
		{};
		~Character() {};

		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;
	};

	class FixedBox : public GameObject
	{
	public:
		FixedBox(const shared_ptr<Stage>& stage);
		~FixedBox();
		virtual void OnCreate() override;

	};
}
//end basecross
