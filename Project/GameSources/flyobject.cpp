
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	flyobject::flyobject(const shared_ptr<Stage>& stage ,Vec3 position) : Object(stage) ,m_Position(position){}
	flyobject::~flyobject() {}

	void flyobject::flyPosison()
	{
		auto gravity = GetComponent<Gravity>();
		gravity->StartJump(Vec3(0.0f, 2.0f, 0.0f));
	}

	void flyobject::OnCreate() {
		Object::OnCreate();
		auto objectGravity = AddComponent<Gravity>();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);

		//ï`âÊê›íË
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		//ptrDraw->SetTextureResource(L"GROUND");

		AddTag(L"Fly");
	}

	void flyobject::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			flyPosison();
		}
	}

}