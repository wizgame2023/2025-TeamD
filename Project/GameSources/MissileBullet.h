/*!
@file Character.h
@brief
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class AreaOfEffect;

	class MissileBullet : public Object {
		float m_Speed;
		Vec3 m_TargetPosition;
		shared_ptr<Transform> m_Target;
		float m_ExplodeSize;
		shared_ptr<AreaOfEffect> m_AreaEffect;

		bool m_IsTarget;

		Vec3 m_LaunchPosition;
		Vec3 m_Direction;

		Effekseer::Handle m_EffectHandle;
	public:
		MissileBullet(const shared_ptr<Stage>& stage,Vec3 position,Vec3 direction, shared_ptr<Transform>& target,float speed,float explodeSize);
		virtual ~MissileBullet(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other)override;
	};
}
//end basecross
