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
		bool Wicth_FixedBox;
	public:
		FixedBox(const shared_ptr<Stage>& stage);
		~FixedBox();
		virtual void OnCreate() override;

	};

	class Wall : public GameObject
	{
		bool Wicth_Wall;
	public:
		Wall(const shared_ptr<Stage>& stage);
		~Wall();
		virtual void OnCreate() override;

	};

}
//end basecross
