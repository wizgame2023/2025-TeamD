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
	protected:
		//“ü—Íƒnƒ“ƒhƒ‰[
		Vec3 m_Position;
		Vec3 m_Rotation;
		Vec3 m_Scale;

		shared_ptr<Character> m_Intruder;
		shared_ptr<Transform> m_Transform;

		float m_ZoneElapsedTime;
	public:
		Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		virtual ~Enemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		void SetIntruder(const shared_ptr<Character>& character) {
			m_Intruder = character;
		}
		Vec3 GetDirectionToIntruder();
		void ZoneSpeedSet();

	private:

	};
}
//end basecross
