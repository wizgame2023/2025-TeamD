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
		float m_EnergyCharge;
		float m_ZoneTime;
	public:
		int m_PlayerStateNum;
		enum PlayerState
		{
			NORMAL = 0b00001, // 2進数 : (0000 0000 0000 0001)
			WALK   = 0b00010, // 2進数 : (0000 0000 0000 0010)
			RUN    = 0b00011, // 2進数 : (0000 0000 0000 0100)
			GUARD  = 0b00100, // 2進数 : (0000 0000 0000 1000)
			PUNCH  = 0b00101, // 2進数 : (0000 0000 0001 0000)
			ZONE   = 0b00110, // 2進数 : (0000 0000 0010 0000)
		};

		Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale);
		~Player();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		Vec2 GetInputState() const;
		Vec3 GetMoveVector();
		void MovePlayer();
		void ZoneActivation();
		void Debug();
		void PlayerHit();
		Vec3 GetForward();
		int GetStates();
	};

	class HitSphere : public GameObject
	{
		Vec3 m_HitPosition;
		Vec3 m_HitRotation;
		Vec3 m_HitScale;
		float m_FlyingTime;
		float m_totalTime;
		float m_speed;
	public:
		HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward);
		~HitSphere() {};
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
	};
}
//end basecross

