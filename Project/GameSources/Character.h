/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {

	class Character : public Object
	{
	protected:
		float m_HP;
		float m_MaxHP;
		float m_Speed;
		float m_AttackDamage;
	public:
		float m_ZoneElapsedTime;

		Character(const shared_ptr<Stage>& stage, Vec3 position, Vec3 rotation, Vec3 scale) :
			Object(stage, position, rotation, scale), m_HP(0),m_MaxHP(0),m_Speed(1.0f),m_ZoneElapsedTime(1.0f) {
		}
		Character(const shared_ptr<Stage>& stage) :
			Character(stage, Vec3(), Vec3(), Vec3(1.0f))
		{
		};
		virtual ~Character() {};

		virtual void OnCreate() override;

		virtual void Dead() {}
		virtual void Damage(float damage,const bool& isSound = true){
			m_HP -= damage;
			if (m_HP <= 0) {
				m_HP = 0;
				Dead();
			}
		}

		double AngleBetweenVectors(const Vec3& v1, const Vec3& v2);
		bool IsWithinDetectionRange(const Vec3& direction, const Vec3& target, double angle);
		double DotProduct(const Vec3& v1, const Vec3& v2);
		double Magnitude(const Vec3& v);
		void ZoneSpeedSet();
		inline float GetElpasedTime();
		void Move(const Vec3& direction);
		void InitHP(int hp) {
			m_MaxHP = hp;
			m_HP = hp;
		}
		float GetMaxHP() {
			return m_MaxHP;
		}
		float GetHP() {
			return m_HP;
		}
		void SetSpeed(float speed) {
			m_Speed = speed;
		}
		float GetSpeed() {
			return m_Speed;
		}

		void SetAttackDamage(const float& Damage) {
			m_AttackDamage = Damage;
		}
		float GetAttackDamage(){
			return m_AttackDamage;
		}
	};

	class FixedBox : public Object
	{
		bool Wicth_FixedBox;
	public:
		FixedBox(const shared_ptr<Stage>& stage);
		~FixedBox();
		virtual void OnCreate() override;
	};

	class BraekBox : public Object
	{
	public:
		BraekBox(const shared_ptr<Stage>& stage);
		~BraekBox();
		virtual void OnCreate() override;
	};

	class Wall : public GameObject
	{
		bool Wicth_Wall;
	public:
		Wall(const shared_ptr<Stage>& stage);
		~Wall();
		virtual void OnCreate() override;

	};
}

//end basecross
