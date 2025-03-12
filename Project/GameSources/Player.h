/*!
@file Player.h
@brief プレイヤーなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class Character;
	class Player : public Character
	{
		//入力ハンドラー
		Vec3 m_Position;
		Vec3 m_Rotation;
		Vec3 m_Scale;
	public:
		Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale);
		~Player();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		Vec2 GetInputState() const;
		Vec3 GetMoveVector();
		void MovePlayer();
	};
}
//end basecross

