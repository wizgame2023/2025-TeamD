/*!
@file Player.h
@brief プレイヤーなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class TargetBoard;

	class Player : public Character
	{
		float m_MoveSpeed;
		float m_EnergyCharge;
		float m_ZoneTime;
		float m_ParryTime;
		float m_ParryDamageIntervalTime;
		float m_TotalTime;
		float m_BoostTime;
		float m_BoostInterval;
		float m_Attacktime;
		float m_AttackInterval;
		float m_DamageInterval;
		float m_BlinkingInterval;
		float m_Damage;
		bool m_ParryJudge;
		bool m_ParryDamageInterval;
		bool m_DamageIntervalStart;
		bool m_IsGoal;
		float m_ParryDamage;
		float m_zoneAnim;
		float m_SearchDistance;
		float m_Length;

		Vec3 m_BoostAngle;
		Vec3 m_HitScale;
		Vec3 m_BulletDire;
		Vec3 m_EffectVec;
		shared_ptr<EffectManeger> m_Effect;
		Effekseer::Handle m_Handle;
		Effekseer::Handle m_BrinkHandle;
		Effekseer::Handle m_ParryHandle;
		shared_ptr<TargetBoard> m_TargetBoard;
		wstring m_AttackAnim = L"Attack";

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
		‾Player();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDraw();
		virtual void Dead();
		virtual bool Damage(bool parry,  float damage,const shared_ptr<GameObject> sorce = nullptr);
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		void IntervalManagement();
		Vec2 GetInputState() const;
		Vec3 GetMoveVector(float& rot);
		void MovePlayer(const float Speed);
		void BoostMove(const float Speed, const Vec3 Angle);
		void ZoneActivation();
		void Debug();
		Vec3 GetForward();
		int GetStates();
		float GetEnergy();
		float GetDamage();
		bool GetParry();
		void SetParryPosition(const Vec3& position);
		void SetDamage(const float& damage);
		void SetIsGaol(const bool& goal);
		Vec3 SearchRange();
		void SetCharge(const float& charge);

		Vec3 RotateTowardsTarget(const Vec3& object, const Vec3& target);
		void AimRock(Vec3 rotate);
		shared_ptr<GameObject> ObjectSearch(const shared_ptr<GameObjectGroup>& group);

		void UpdateAnim();
		float Parry(float damage, const float& ParrySecond);
		void AddAnimation();
		void PlayAnimation();
		void Blinking();
		const void SetAnim(wstring animname, float time = 0.0f) {
			auto draw = GetComponent<BcPNTBoneModelDraw>();
			if (draw->GetCurrentAnimation() != animname)
				if (draw->GetAnimeLoop()) draw->ChangeCurrentAnimation(animname, time);
				else 
					if (draw->IsTargetAnimeEnd()) draw->ChangeCurrentAnimation(animname, time);
		}

		/// @brief 指定されたタイマー値とフレーム値に基づいて、インターバルタイマーが最大時間に達したかどうかを判定します。
		/// @param TimerStart タイマーを開始するかどうかを示すフラグ。
		/// @param MaxTimer タイマーが到達すべき最大時間（秒単位）。
		/// @param frame 経過したフレームまたは時間（秒単位）。
		/// @param Timer 現在のタイマー値。
		/// @return タイマーが最大時間に達した場合は true、それ以外は false を返します。
		bool IntervalTimer(const bool& TimerStart, const float& MaxTimer, const float& frame, float& Timer, const bool& Return);
		float GetLength()
		{
			return m_SearchDistance;
		}

	};

	class HitSphere : public Object
	{
		Vec3 m_HitPosition;
		Vec3 m_HitRotation;
		Vec3 m_HitScale;
		float m_FlyingTime;
		float m_TotalTime;
		float m_Speed;
		float m_ZoneElapsedTime;
		float m_Length;
		shared_ptr<EffectManeger> m_Effect;
		Effekseer::Handle m_Handle;
		Effekseer::Handle m_HitHandle;

		shared_ptr<GameObject> m_Player;

		float f = 0;
	public:
		HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward, const shared_ptr<GameObject> player, const Vec3 scale,const float& length);
		‾HitSphere();
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		void OnCollisionEnter(shared_ptr<GameObject>& other);
	};
}
//end basecross

