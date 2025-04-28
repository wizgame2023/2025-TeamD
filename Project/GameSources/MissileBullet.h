/*!
@file Character.h
@brief ƒLƒƒƒ‰ƒNƒ^[‚È‚Ç
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class AreaOfEffect;

	class MissileBullet : public Object {
		float m_Speed;
		Vec3 m_Target;
		float m_ExplodeSize;
		shared_ptr<AreaOfEffect> m_AreaEffect;

		bool m_IsTarget;
	public:
		MissileBullet(const shared_ptr<Stage>& stage, Vec3 target,float speed,float explodeSize);
		virtual ~MissileBullet(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other)override;
	};
}
//end basecross
