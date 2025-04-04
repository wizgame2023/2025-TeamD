/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class AttackCollision : public GameObject {
		Vec3 m_StartPosition;
		Vec3 m_Size;
	protected:
		float m_Damage;
		float m_ExistenceTime;
		shared_ptr<CollisionObb> m_Collision;
		shared_ptr<Transform> m_BoneTransform;
	public:
		AttackCollision(const shared_ptr<Stage>& stage,Vec3 position,Vec3 size,float damage,float time) : 
			GameObject(stage),
			m_StartPosition(position),m_Size(size),
			m_Damage(damage),m_ExistenceTime(time) {}
		virtual ~AttackCollision() {}

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
		virtual void LoopEffect(){}
		virtual void ContactObjectEffect(){}
		virtual void ContactPlayerEffect(){}
		virtual void ContactPlayer(shared_ptr<GameObject>& player){}
		virtual void ContactStage(shared_ptr<GameObject>& object){}

		virtual float GetDamage() {
			return m_Damage;
		}
	};

	class CrushAttack : public AttackCollision {
		float m_BlowForce;
	public:
		CrushAttack(const shared_ptr<Stage>& stage,Vec3 position,Vec3 size,float damage,float time,float force) : 
			AttackCollision(stage,position,size,damage,time),m_BlowForce(force){}
		virtual~CrushAttack(){}

		virtual void ContactPlayer(shared_ptr<GameObject>& player);
	};

	class Missile : public AttackCollision {
		float m_ExplodePower;
		Vec3 m_Target;
	public:
		Missile(const shared_ptr<Stage>& stage,const Vec3 target, Vec3 position, Vec3 size, float damage, float time,float power) : 
			AttackCollision(stage,position,size,damage,time),m_Target(target),m_ExplodePower(power){ }
		virtual ~Missile(){}
		virtual void OnUpdate()override;
		virtual void ContactStage(shared_ptr<GameObject>& object)override;
	};
}



//end basecross
