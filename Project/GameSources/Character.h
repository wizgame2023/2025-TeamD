/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross{
	
	class Character : public GameObject
	{
	public:
		int m_HP;

		Character(const shared_ptr<Stage>& stage) :
			GameObject(stage) ,m_HP(0)
		{};
		virtual ~Character() {};

		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnUpdate2() override;
		virtual void OnDraw() override;
	};

	class FixedBox : public Object
	{
		bool Wicth_FixedBox;
	public:
		FixedBox(const shared_ptr<Stage>& stage);
		~FixedBox();
		virtual void OnCreate() override;
		virtual shared_ptr<Object> Create()override;
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
