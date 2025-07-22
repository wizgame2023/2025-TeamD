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
		Vec3 m_CameraPos;
		
		static bool m_Hit;
		bool m_Point;

		shared_ptr<CollisionSphere> m_Collision;
		shared_ptr<GameObject> m_HitObject;

	public:
		CameraCollision(const shared_ptr<Stage>& StagePtr);
		virtual ‾CameraCollision() {}

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
		float m_Magnification;
		float m_Up;

		float m_Angle;
		float m_RotateSpeed;
		bool m_HitCollision;
		bool m_IsShaking;      // カメラがシェイク中かどうか
		float m_Duration;         // シェイク継続時間（残り）
		float m_InitialDuration;  // シェイク開始時の時間（for 減衰計算）
		float m_Magnitude;        // 最大振幅（単位：画素やワールド単位）
		float m_CurrntTime;
		shared_ptr<Stage>m_Stage;
		float m_MouseSensitivityX; // マウス感度（X軸）
		POINT m_CenterPt;             // 画面中央（スクリーン座標）

		int m_Width;
		int m_Height;
		int m_FrameCounter;
		int m_MouseUpdateInterval;
		bool m_StopCamera;
		// FollowCamera.h の適切な箇所に追加
		float m_Pitch = 0.0f;               // 現在の上下角度
		float m_MinPitch = -XM_PIDIV4;         // 下向き最大（-45°）
		float m_MaxPitch = 0;         // 上向き最大（+45°）
		float m_TargetAngle = 0.0f;   // 目標のヨー角
		float m_TargetPitch = 0.0f;   // 目標のピッチ角
		bool  m_IsSmoothLook = false;  // 滑らかに回転中かどうか
		float m_SmoothSpeed = 5.0f;   // 補完スピード係数（大きいほど速い）
		float m_StartAngle = 0.0f;    // 補完開始時の現在ヨー角
		float m_StartPitch = 0.0f;    // 補完開始時の現在ピッチ角
		float m_LerpDuration = 1.0f;    // 補完にかける時間（秒）
		float m_LerpElapsed = 0.0f;    // 経過時間

		shared_ptr<Transform> m_PlayerTransform;
		shared_ptr<CameraCollision> m_CameraCollision;
		// デッドゾーン（しきい値）
		static constexpr float kYawDeadZone = XMConvertToRadians(180.0f); // 2度


	public:
		FollowCamera(const shared_ptr<Stage>& StagePtr);
		virtual ‾FollowCamera() {}
		virtual void OnUpdate();
		virtual void OnCreate();
		virtual void LogCamera();
		
		void SetCameraPause(const bool& StopCamera);
		Vec2 CameraUp(float totaltime);
		void LookAtNearestEnemy();

		void SetTarget(const shared_ptr<Transform> playerTransform) {
			m_PlayerTransform = playerTransform;
		}
		void SetShaking(const bool& sh) {
			m_IsShaking = sh;
		}
		void ResetCursorPosition();

		float GetAngle() const
		{
			return m_Angle;
		}
		Vec3 SearchRange();
		shared_ptr<GameObject> ObjectSearch(const shared_ptr<GameObjectGroup>& group);
		float CalcAngleDiff(float from, float to);
		Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);


	};


}
