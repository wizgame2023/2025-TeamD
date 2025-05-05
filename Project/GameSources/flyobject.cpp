
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	flyobject::flyobject(const shared_ptr<Stage>& stage) : Object(stage){}
	flyobject::~flyobject() {}

	void flyobject::flyPosison()
	{
		auto gravity = GetComponent<Gravity>();
		Vec3 hitPos = GetComponent<Transform>()->GetPosition();
		Vec3 pos = GetPosition();
		Vec3 vec = hitPos - pos;

		gravity->StartJump(Vec3(0.0f + vec.x, 5.0f, 0.0f + vec.z));
	}

	void flyobject::OnCreate() {
		Object::OnCreate();
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		auto objectGravity = AddComponent<Gravity>();

		//ï`âÊê›íË
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		//ptrDraw->SetTextureResource(L"GROUND");
	}

	void flyobject::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			flyPosison();
		}
	}

}