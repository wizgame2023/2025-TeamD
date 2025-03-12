/*!
@file Enemy.h
@brief “G‚È‚Ç
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class Enemy : public Character
	{
	public:
		Enemy(const shared_ptr<Stage>& stage);
		~Enemy();
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;
	private:

	};
}
//end basecross
