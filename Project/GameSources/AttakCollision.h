/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	struct AttackDate {
		float m_Damage;
		float m_Range;
		float m_MaxExitTime;
		float m_ExistenceTime;
		float m_Cooldown;
		float m_MaxCooldown;
		float m_CharacterCooldown;

		void Reset() {
			m_ExistenceTime = m_MaxExitTime;
			m_Cooldown = m_MaxCooldown;
		}
		AttackDate(float damage, float range, float time, float cooldown, float charaCooldown) :
			m_Damage(damage),m_Range(range),
			m_ExistenceTime(0),m_MaxCooldown(cooldown), m_Cooldown(0),
			m_CharacterCooldown(charaCooldown), m_MaxExitTime(time)
		{}
	};
	class Attack : public Object {
	protected:
		AttackDate m_Date;
	public:
		Attack(const shared_ptr<Stage>& stage, AttackDate date) :
			Object(stage,Vec3(1000,1000,1000), Vec3(), Vec3(1)),
			m_Date(date) {}
		virtual ~Attack() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) {
			if (Other->FindTag(L"Player")) {
				ContactPlayerEffect();
				ContactPlayer(Other);
			}
			if (Other->FindTag(L"Stage")) {
				ContactObjectEffect();
				ContactStage(Other);
			}
		}
		void Play(Vec3 position) {
			SetDrawActive(true);
			m_Transform->SetPosition(position);
			m_Date.Reset();
		}
		virtual void LoopEffect() {}
		virtual void ContactObjectEffect() {}
		virtual void ContactPlayerEffect() {}
		virtual void ContactPlayer(shared_ptr<GameObject>& player) {}
		virtual void ContactStage(shared_ptr<GameObject>& object) {}

		virtual float GetDamage() {
			return m_Date.m_Damage;
		}
		float GetCooldown() {
			return m_Date.m_Cooldown;
		}
		float GetRange() {
			return m_Date.m_Range;
		}
		float GetCharaCooldown() {
			return m_Date.m_CharacterCooldown;
		}

	};
	class AttackCollision : public Attack {
		Vec3 m_Size;
	protected:
		shared_ptr<CollisionObb> m_Collision;
	public:
		AttackCollision(const shared_ptr<Stage>& stage,Vec3 size,AttackDate date) :
			Attack(stage,date),
			m_Size(size){}
		virtual ~AttackCollision() {}

		virtual void OnCreate()override;
	};

	class CrushAttack : public AttackCollision {
		float m_BlowForce;
	public:
		CrushAttack(const shared_ptr<Stage>& stage,Vec3 size,AttackDate date,float force) :
			AttackCollision(stage,size,date),m_BlowForce(force){}
		virtual~CrushAttack(){}

		virtual void ContactPlayer(shared_ptr<GameObject>& player);
	};
	class MachineGun : public Attack {
		shared_ptr<GameObject> m_Target;
		float m_LaunchNum;
		float m_ShotInterval;
	public:
		MachineGun(const shared_ptr<Stage>& stage, const shared_ptr<GameObject>& target,AttackDate date,float num) :
			Attack(stage, date), m_Target(target),m_LaunchNum(num),m_ShotInterval(0.0f){
		}
		virtual ~MachineGun() {}
		virtual void OnUpdate()override;
	};
	class Missile : public AttackCollision {
		float m_ExplodePower;
		Vec3 m_Target;
	public:
		Missile(const shared_ptr<Stage>& stage,const Vec3 target, Vec3 size,AttackDate date,float power) :
			AttackCollision(stage,size,date),m_Target(target),m_ExplodePower(power){ }
		virtual ~Missile(){}
		virtual void OnUpdate()override;
		virtual void ContactStage(shared_ptr<GameObject>& object)override;
	};
}



//end basecross
