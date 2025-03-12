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
		//“ü—Íƒnƒ“ƒhƒ‰[
		Vec3 m_Position;
		Vec3 m_Rotation;
		Vec3 m_Scale;
	public:
		Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale);
		~Enemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		void OnCollisionEnter(shared_ptr<GameObject>& other);
	private:

	};
}
//end basecross
