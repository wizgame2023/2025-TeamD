/*!
@file Character.h
@brief 
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class LineObject;
	class BulletLine;
	class Bullet : public Object
	{
		Vec3 m_Position;
		Vec3 m_EndPosition;
		Vec3 m_LineEndPosition;
		float m_Speed;
		Vec3 m_Direction;

		float m_EffectiveRange;
		float m_ZoneElapsedTime;

		shared_ptr<BulletLine> m_Line;
		shared_ptr<GameObject> m_Mob;
		float m_LineLength;
		bool m_bulletPally;
	public:
		Bullet(const shared_ptr<Stage>& stage, Vec3 position, float speed, Vec3 direction, float range, shared_ptr<GameObject> enemy);
		‾Bullet();
		virtual void OnCreate() override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& other);
		void Delete();
		void ZoneSpeedSet();
	};
}
//end basecross
