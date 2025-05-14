
#pragma once
#include "stdafx.h"
#include "StageBuilder.h"

namespace basecross {
	class Player;
	class HitSphere;
	class Character;
	class flyobject : public Object
	{
		Vec3 GetForward();
		void flyPositison(shared_ptr<GameObject>& other);
		shared_ptr<HitSphere> m_Sphere;
		float m_ZoneElapsedTime;
		float m_Speed;
		bool m_Zone;

	public:
		flyobject(const shared_ptr<Stage>& stage);
		~flyobject();
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		void OnCollisionEnter(shared_ptr<GameObject>& other) override;
		void ZoneSpeedSet();

	};

}