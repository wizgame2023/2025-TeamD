/*!
@file Player.h
@brief プレイヤーなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class Player : public Character
	{
		//入力ハンドラー
		Vec3 m_Position;
		Vec3 m_Rotation;
		Vec3 m_Scale;
		float m_MoveSpeed;
		float m_EnergyCharge;
		float m_ZoneTime;
		float m_ParryTime;
		bool m_ParryJudge;
		float m_BoostTime;
		Vec3 m_BoostAngle;

	public:
		int m_PlayerStateNum;
		enum PlayerState
		{
			NORMAL = 0b00000001,
			WALK   = 0b00000010,
			RUN    = 0b00000100,
			ATTACK = 0b00001000,
			ZONE   = 0b00010000,
			DASH   = 0b00100000,
		};

		Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale);
		~Player();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();

		void OnCollisionEnter(shared_ptr<GameObject>& other);

		Vec2 GetInputState() const;
		Vec3 GetMoveVector();
		void MovePlayer(const float Speed);
		void BoostMove(const float Speed, const Vec3 Angle);
		void ZoneActivation();
		void Debug();
		Vec3 GetForward();
		int GetStates();
		int GetPlayerHP();
		void SearchRange();
		Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);
		shared_ptr<GameObject> BulletSearch();
	};

	class HitSphere : public GameObject
	{
		Vec3 m_HitPosition;
		Vec3 m_HitRotation;
		Vec3 m_HitScale;
		float m_FlyingTime;
		float m_TotalTime;
		float m_Speed;

		shared_ptr<GameObject> m_Player;
	public:
		HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward, const shared_ptr<GameObject> player);
		~HitSphere() {};
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		void OnCollisionEnter(shared_ptr<GameObject>& other);
	};
}
//end basecross

