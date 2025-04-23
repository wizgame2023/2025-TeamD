/*!
@file Character.h
@brief ƒLƒƒƒ‰ƒNƒ^[‚È‚Ç
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
		float m_LineLength;
	public:
		Bullet(const shared_ptr<Stage>& stage, Vec3 position, float speed, Vec3 direction, float range);
		~Bullet();
		virtual void OnCreate() override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& other);
		void Delete();
		void ZoneSpeedSet();
	};
}
//end basecross
