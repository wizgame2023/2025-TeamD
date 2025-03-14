/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Ballet : public GameObject
	{
		Vec3 m_Position;
		float m_Speed;
		Vec3 m_Direction;

		float m_EffectiveRange;
		shared_ptr<Transform> m_Transform;
		float m_ZoneElapsedTime;

	public:
		Ballet(const shared_ptr<Stage>& stage, Vec3 position, float speed, Vec3 direction, float range);
		~Ballet();
		virtual void OnCreate() override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& other);

		void ZoneSpeedSet();
	};
}
//end basecross
