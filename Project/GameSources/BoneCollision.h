/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
namespace basecross {
	class Bone : public GameObject {
		shared_ptr<GameObject> m_Object;
		Vec3 m_Position;
		int m_BoneNumber;

		shared_ptr<Transform> m_Transform;
	public:
		Bone(shared_ptr<Stage>& stage, shared_ptr<GameObject>& object, Vec3 pos, int number) :
			GameObject(stage), m_Position(pos), m_Object(object), m_BoneNumber(number) {
		}

		virtual void OnCreate()override;

		shared_ptr<GameObject>& GetGameObject() {
			return m_Object;
		}
		Vec3 GetInitPosition() {
			return m_Position;
		}
		int GetBoneNumber() {
			return m_BoneNumber;
		}
		shared_ptr<Transform>& GetTransform() {
			return m_Transform;
		}
	};
	class BonePosition : public Component {
		wstring m_FileName;
		vector<shared_ptr<Bone>> m_Bones;

		shared_ptr<BcPNTBoneModelDraw> m_ObjectDraw;
		shared_ptr<Transform> m_ObjectTransform;

	public:
		BonePosition(shared_ptr<GameObject>& object,const wstring& name) : Component(object),m_FileName(name){}
		virtual ~BonePosition(){}

		virtual void OnUpdate()override;
		virtual void OnDraw()override{}

		void CreateBone();
		
		shared_ptr<Bone>& GetBone(int index) {
			return m_Bones[index];
		}
		Vec3 GetBonePosition(int index) {
			return m_Bones[index]->GetTransform()->GetPosition();
		}
	};
	
}

//end basecross
