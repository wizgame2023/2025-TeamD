/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class Ground : public Object
	{
	public:
		Ground(const shared_ptr<Stage>& stage);
		‾Ground();
		virtual void OnCreate() override;
	};
	class Way : public Object
	{
	public:
		Way(const shared_ptr<Stage>& stage);
		‾Way();
		virtual void OnCreate() override;
		virtual void OnUpdate()override;
	};

	class LimitArea : public Object {
	public:
		LimitArea(const shared_ptr<Stage>& stage);
		‾LimitArea();
		virtual void OnCreate() override;
	};

	class Building : public Object {
	public:
		Building(const shared_ptr<Stage>& stage);
		‾Building();
		virtual void OnCreate() override;
	};
}

//end basecross
