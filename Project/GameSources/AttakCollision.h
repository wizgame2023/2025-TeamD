/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Timer.h"
namespace basecross {
	class Character;
	struct AttackDate {
		shared_ptr<Character> m_Owner;
		float m_Damage;
		float m_Range;
		Timer m_ExitTimer;
		Timer m_CooldownTimer;
		float m_CharacterCooldown;

		void Reset() {
			m_ExitTimer.SetTime(m_ExitTimer.GetMaxTime(), true);
			m_CooldownTimer.SetTime(m_CooldownTimer.GetMaxTime(), true);
		}
		AttackDate(const shared_ptr<Character>& owner,float damage, float range, float time, float cooldown, float charaCooldown) :
			m_Owner(owner),
			m_Damage(damage),m_Range(range),
			m_ExitTimer(Timer(time,false)),m_CooldownTimer(Timer(cooldown,cooldown,false)),
			m_CharacterCooldown(charaCooldown)
		{}
	};
	class Attack : public Object {
	protected:
		AttackDate m_Date;
		bool m_IsFinish;
	public:
		Attack(const shared_ptr<Stage>& stage, AttackDate date) :
			Object(stage,Vec3(1000,1000,1000), Vec3(), Vec3(1)),m_IsFinish(false),
			m_Date(date) {}
		virtual ~Attack() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) {
			if (Other->FindTag(L"Player")) {
				ContactPlayerEffect();
				ContactPlayer(Other);
				auto player = dynamic_pointer_cast<Player>(Other);
				player->Damage(true, 4.0f);
			}
			if (Other->FindTag(L"Stage")) {
				ContactObjectEffect();
				ContactStage(Other);
			}
			if (Other->FindTag(L"HitJudge")) {
				Other->OnCollisionEnter(GetThis<GameObject>());
			}
		}
		void Play(Vec3 position) {
			SetDrawActive(true);
			m_Transform->SetPosition(position);
			m_Date.Reset();
		}
		void Stop() {
			SetDrawActive(false);
			m_IsFinish = true;
			m_Transform->SetPosition(Vec3(1000, 1000, 1000));
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
			return m_Date.m_CooldownTimer.GetMaxTime() - m_Date.m_CooldownTimer.GetTime();
		}
		float GetRange() {
			return m_Date.m_Range;
		}
		bool IsInRange(float distance) {
			return distance < m_Date.m_Range;
		}
		float GetCharaCooldown() {
			return m_Date.m_CharacterCooldown;
		}
		bool IsFinish() {
			return m_IsFinish;
		}
	};
	template<class CollisionType>
	class AttackCollision : public Attack {
		Vec3 m_Size;
	protected:
		shared_ptr<CollisionType> m_Collision;
	public:
		AttackCollision(const shared_ptr<Stage>& stage,Vec3 size,AttackDate date) :
			Attack(stage,date),
			m_Size(size){}
		virtual ~AttackCollision() {}

		virtual void OnCreate()override {
			Attack::OnCreate();
			m_Collision = AddComponent<CollisionType>();
			m_Collision->SetDrawActive(true);
			m_Collision->SetAfterCollision(AfterCollision::None);

			m_Transform->SetScale(m_Size);
			AddTag(L"BossAttack");
		}
	};
	//class AttackCollisionCircle : public Attack {
	//	float m_Size;
	//protected:
	//	shared_ptr<CollisionSphere> m_Collision;
	//public:
	//	AttackCollisionCircle(const shared_ptr<Stage>& stage, float size, AttackDate date) :
	//		Attack(stage, date),
	//		m_Size(size) {
	//	}
	//	virtual ~AttackCollisionCircle() {}

	//	virtual void OnCreate()override;
	//};

	class CrushAttack : public AttackCollision<CollisionSphere> {
		float m_BlowForce;
	public:
		CrushAttack(const shared_ptr<Stage>& stage,Vec3 size,AttackDate date,float force) :
			AttackCollision(stage,size,date),m_BlowForce(force){}
		virtual~CrushAttack(){}

		virtual void ContactPlayer(shared_ptr<GameObject>& player);
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other);
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
	class Missile : public AttackCollision<CollisionObb> {
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
