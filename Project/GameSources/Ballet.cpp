/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	
	Ballet::Ballet(const shared_ptr<Stage>& stage,Vec3 position, float speed, Vec3 direction,float range):
		GameObject(stage),m_Position(position),m_Speed(speed),m_Direction(direction),m_EffectiveRange(range)
	{
	}
	Ballet::~Ballet(){}

	void Ballet::OnCreate()
	{
		//初期位置の設定
		m_Transform = AddComponent<Transform>();
		m_Transform->SetPosition(m_Position);
		m_Transform->SetRotation(Vec3(0));
		m_Transform->SetScale(Vec3(0.1f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
	}
	void Ballet::OnUpdate() {
		float elapsed = App::GetApp()->GetElapsedTime();
		Vec3 position = m_Transform->GetPosition();
		position += m_Speed * m_Direction * elapsed;
		m_Transform->SetPosition(position);

		if ((m_Position - position).length() > m_EffectiveRange) {
			GetStage()->RemoveGameObject<Ballet>(GetThis<Ballet>());
		}
	}

	void Ballet::OnCollisionEnter(shared_ptr<GameObject>& other) {
		if (other->FindTag(L"Player")) {
			GetStage()->RemoveGameObject<Ballet>(GetThis<Ballet>());
		}
	}

}
//end basecross
