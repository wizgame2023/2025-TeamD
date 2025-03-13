/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	class FollowCamera : public Camera {
		Vec3 m_Direction;
		Vec3 m_Eye;
		Vec3 m_Position;
		float m_Angle;
		float m_RotateSpeed;

		shared_ptr<Transform> m_PlayerTransform;
	public:
		FollowCamera();
		virtual ~FollowCamera() {}
		virtual void OnUpdate();
		virtual void OnCreate() {};
		virtual void LogCamera();
		void SetTarget(const shared_ptr<Transform> playerTransform) {
			m_PlayerTransform = playerTransform;
		}

	};
}