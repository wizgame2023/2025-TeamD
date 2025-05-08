
#include "stdafx.h"
#include "Project.h"

namespace basecross {


	flyobject::flyobject(const shared_ptr<Stage>& stage) : Object(stage){}
	flyobject::~flyobject() {}

	Vec3 flyobject::GetForward()
	{
		return m_Transform->GetForward();
	}

	void flyobject::flyPosison(shared_ptr<GameObject>& other)
	{		
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto gravity = GetComponent<Gravity>();
		Vec3 objPos = other->GetComponent<Transform>()->GetPosition();
		Vec3 flyPos = GetComponent<Transform>()->GetPosition();
		Vec3 spherePos = other->GetComponent<Transform>()->GetForward();
		Vec3 pos = objPos - flyPos;
		pos.normalize(); 

		gravity->StartJump(Vec3(-pos.x + spherePos.x / 2, 5.0f, -pos.z + spherePos.z / 2));
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
			flyPosison(other);
		}
	}

}