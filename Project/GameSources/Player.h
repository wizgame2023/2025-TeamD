/*!
@file Player.h
@brief プレイヤーなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class ForecastLine;
	class TargetBoard;

	class Player : public Character
	{
		//入力ハンドラー
		float m_MoveSpeed;
		float m_EnergyCharge;
		float m_ZoneTime;
		float m_ParryTime;
		float m_TotalTime;
		float m_BoostTime;
		float m_BoostInterval;
		float m_Attacktime;
		float m_AttackInterval;
		float m_DamageInterval;
		bool m_ParryJudge;
		bool m_DamageIntervalStart;

		Vec3 m_BoostAngle;
		Vec3 m_BulletDire;
		shared_ptr<EffectManeger> m_Effect;

		shared_ptr<TargetBoard> m_TargetBoard;
		shared_ptr<GameObject> m_Target;

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
		Player(const shared_ptr<Stage>& stage);
		Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale);
		~Player();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		virtual void Dead();
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		Vec2 GetInputState() const;
		Vec3 GetMoveVector(float& rot);
		void MovePlayer(const float Speed);
		void BoostMove(const float Speed, const Vec3 Angle);
		void ZoneActivation();
		void Debug();
		Vec3 GetForward();
		int GetStates();
		float GetEnergy();
		Vec3 SearchRange();

		Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);
		void AimRock(Vec3 rotate);
		shared_ptr<GameObject> ObjectSearch(const shared_ptr<GameObjectGroup>& group);
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

