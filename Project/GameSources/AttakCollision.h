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
		AttackDate(float range,float time, float cooldown, float charaCooldown) : AttackDate(nullptr,0.0f, range,time,cooldown,charaCooldown){}
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
		virtual ‾Attack() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnCollisionEnter(shared_ptr<GameObject>& Other) {
			if (Other->FindTag(L"Player")) {
				ContactPlayerEffect();
				ContactPlayer(Other);
				auto player = dynamic_pointer_cast<Player>(Other);
				player->Damage(true, GetDamage(),GetThis<Attack>());
			}
			if (Other->FindTag(L"Stage")) {
				ContactObjectEffect();
				ContactStage(Other);
			}
			if (Other->FindTag(L"HitJudge")) {
				Other->OnCollisionEnter(GetThis<GameObject>());
			}
		}
		virtual void Play(Vec3 position,Vec3& direction = Vec3(0,0,1)) {
			SetDrawActive(true);
			m_Transform->SetPosition(position);

			auto rotateMatrix = (Mat4x4)XMMatrixLookAtLH(Vec3(), direction, Vec3(0, 1, 0));
			rotateMatrix = inverse(rotateMatrix);
			Quat qt = rotateMatrix.quatInMatrix();
			qt = Quat(0, sin(XMConvertToRadians(45)), 0, cos(XMConvertToRadians(45))) * qt;
			m_Transform->SetQuaternion(qt);

			m_Date.Reset();
		}
		virtual void Stop() {
			SetDrawActive(false);
			m_IsFinish = true;
			m_Transform->SetPosition(Vec3(1000, 1000, 1000));
		}
		virtual void LoopEffect() {}
		virtual void ContactObjectEffect() {}
		virtual void ContactPlayerEffect() {}
		virtual void ContactPlayer(shared_ptr<GameObject>& player) {}
		virtual void ContactStage(shared_ptr<GameObject>& object) {}
		virtual void ReflectParry(Vec3 position){}
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
		shared_ptr<Character> GetDete()
		{
			return m_Date.m_Owner;
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
		virtual ‾AttackCollision() {}

		virtual void OnCreate()override {
			Attack::OnCreate();
			m_Collision = AddComponent<CollisionType>();
			m_Collision->SetDrawActive(GameManager::Instance()->IsDebug());
			m_Collision->SetAfterCollision(AfterCollision::None);

			m_Transform->SetScale(m_Size);
			AddTag(L"BossAttack");
		}

		Vec3 GetSize() {
			return m_Size;
		}
	};
	class ImpactAttack : public AttackCollision<CollisionObb> {
		float m_BlowForce;

	public:
		ImpactAttack(const shared_ptr<Stage>& stage, Vec3 size, AttackDate date, float force) :
			AttackCollision(stage, size, date), m_BlowForce(force) {
		}
		virtual‾ImpactAttack() {}

		virtual void ContactPlayer(shared_ptr<GameObject>& player)override;

		virtual void ReflectParry(Vec3 position)override;
	};
	class CrushAttack : public AttackCollision<CollisionSphere> {
		float m_BlowForce;
	public:
		CrushAttack(const shared_ptr<Stage>& stage,Vec3 size,AttackDate date,float force) :
			AttackCollision(stage,size,date),m_BlowForce(force){}
		virtual‾CrushAttack(){}

		virtual void ContactPlayer(shared_ptr<GameObject>& player)override;

		virtual void ReflectParry(Vec3 position)override;
	};
	class MachineGun : public Attack {
		shared_ptr<GameObject> m_Target;
		float m_LaunchNum;
		float m_ShotInterval;
	public:
		MachineGun(const shared_ptr<Stage>& stage, const shared_ptr<GameObject>& target,AttackDate date,float num) :
			Attack(stage, date), m_Target(target),m_LaunchNum(num),m_ShotInterval(0.0f){
		}
		virtual ‾MachineGun() {}
		virtual void OnUpdate()override;
	};
	class Missile : public Attack {
		float m_ExplodePower;
		shared_ptr<Transform> m_Target;
		int m_MissileCount;
		int m_MissileMaxCount;
		Timer m_MissileTimer;
		vector<Vec3> m_MuzzlePositions;
		int m_MuzzleIndex;
	public:
		Missile(const shared_ptr<Stage>& stage,const shared_ptr<Transform> target,AttackDate date,float power,int count,float time) :
			Attack(stage, date), m_Target(target), m_ExplodePower(power),
			m_MissileCount(count),m_MissileMaxCount(count),
			m_MuzzleIndex(0),
			m_MissileTimer(Timer(time,false)) { }
		virtual ‾Missile(){}
		virtual void OnUpdate()override;

		virtual void Play(Vec3 position, Vec3& direction = Vec3(0, 0, 1))override;

		void AddMuzzle(Vec3 position) {
			m_MuzzlePositions.push_back(position);
		}

	};

	class ShakeOffAttack : public ImpactAttack {
	public:
		ShakeOffAttack(const shared_ptr<Stage>& stage, Vec3 size, AttackDate date, float force) :
			ImpactAttack(stage, size, date,force){
		}
		virtual void ReflectParry(Vec3 position)override;
	};
}



//end basecross
