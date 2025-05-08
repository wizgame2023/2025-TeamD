
#pragma once
#include "stdafx.h"
#include "StageBuilder.h"

namespace basecross {
	class Player;
	class HitSphere;
	class flyobject : public Object
	{
		Vec3 GetForward();
		void flyPosison(shared_ptr<GameObject>& other);
		shared_ptr<HitSphere> m_Sphere;
	public:
		flyobject(const shared_ptr<Stage>& stage);
		~flyobject();
		virtual void OnCreate() override;
		//virtual void OnUpdate() override;
		void OnCollisionEnter(shared_ptr<GameObject>& other) override;

	};

}