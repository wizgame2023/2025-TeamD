/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Citizen.h"

namespace basecross {
	Citizen::Citizen(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Character(stage, position, Vec3(), scale) {
	}

	Citizen::~Citizen(){}

	void Citizen::OnCreate()
	{
		Character::OnCreate();
		InitHP(10);
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);

		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.1f), //(.1f, .1f, .1f),
			Vec3(0.0f, 90.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -0.5f, 0.0f)
		);
		ptrDraw->SetMeshResource(L"PLAYER");
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::AlphaBlend);
		ptrDraw->SetOwnShadowActive(true);

		ptrDraw->SetDiffuse(Col4(1, 0, 0, 1));

		auto ptrGra = AddComponent<Gravity>();
		auto shadowPtr = AddComponent<Shadowmap>();
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");
		AddTag(L"Citizen");

		auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
		group->IntoGroup(GetThis<Citizen>());

	}

	void Citizen::OnUpdate()
	{
		ZoneSpeedSet();
		RunAway();
		if (m_HP <= 0)
		{
			Dead();
		}
	}

	void Citizen::Dead()
	{
		auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
		auto& groupVec = group->GetGroupVectors();
		for (int i = 0; i < groupVec.size(); i++) {
			auto obj = groupVec[i].lock();
			if (obj != nullptr) {
				if (obj == GetThis<GameObject>()) {
					groupVec.erase(groupVec.begin() + i);
					break;
				}
			}
		}

		m_Stage->RemoveGameObject<Citizen>(GetThis<Citizen>());
	}

	void Citizen::RunAway()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto obj = m_Stage->GetSharedGameObject<Object>(L"BreakObject", false);
		if (obj != nullptr)
		{
			Vec3 position = GetPosition();
			Vec3 objDirection = obj->GetComponent<Transform>()->GetPosition() - position;
			float objRotate = atan2f(objDirection.x, objDirection.z);
			m_Transform->SetRotation(Vec3(0, objRotate, 0));
			float objRenge = objDirection.length();
			position += objDirection.normalize() * 3.0f * elapsedTime * m_ZoneElapsedTime;
			SetPosition(position);
		}
	}

	void Citizen::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"BreakObject"))
		{
			Dead();
		}
	}

}
//end basecross
