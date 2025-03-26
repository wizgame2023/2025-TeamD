/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class CameraCollision :public GameObject
	{
		Vec3 m_GetPosition;
		Vec3 m_TargetPosition;

		static bool m_Hit;
		bool m_Point;

		shared_ptr<CollisionSphere> m_Collision;
		shared_ptr<GameObject> m_HitObject;

	public:
		CameraCollision(const shared_ptr<Stage>& StagePtr);
		virtual ~CameraCollision() {}

		Vec3 GetCompareVertex(Vec2 verx, Vec2 very);
		virtual void OnUpdate()override;
		virtual void OnCreate()override;

		//当たリ判定
		virtual void OnCollisionEnter(shared_ptr<GameObject>& other);
		//出たとき
		virtual void OnCollisionExit(shared_ptr<GameObject>& other);

		Vec3 GetAfterPosition(Vec3 beforePosi, Vec3 tergetPosi);
		float Cross(Vec2 crox, Vec2 croy);

		static bool GetHit()
		{
			return  m_Hit;
		}

	};

	class FollowCamera : public Camera {
		Vec3 m_Direction;
		Vec3 m_Eye;
		Vec3 m_Position;
		float m_Angle;
		float m_RotateSpeed;
		bool m_HitCollision;
		shared_ptr<Stage>m_Stage;
		
		int m_Width;
		int m_Height;

		shared_ptr<Transform> m_PlayerTransform;
		shared_ptr<CameraCollision> m_CameraCollision;

	public:
		FollowCamera(const shared_ptr<Stage>& StagePtr);
		virtual ~FollowCamera() {}
		virtual void OnUpdate();
		virtual void OnCreate();
		virtual void LogCamera();

		void SetTarget(const shared_ptr<Transform> playerTransform) {
			m_PlayerTransform = playerTransform;
		}

		float GetAngle() const
		{
			return m_Angle;
		}

	};


}