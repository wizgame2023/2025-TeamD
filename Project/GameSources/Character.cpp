/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Character::OnCreate()
	{
		Object::OnCreate();
	}

	double Character::AngleBetweenVectors(const Vec3& v1, const Vec3& v2)
	{
		double dot_prod = DotProduct(v1, v2);
		double mag_v1 = Magnitude(v1);
		double mag_v2 = Magnitude(v2);
		return std::acos(dot_prod / (mag_v1 * mag_v2));
	}

	bool Character::IsWithinDetectionRange(const Vec3& direction, const Vec3& target, double angle)
	{
		float angleresult = angleBetweenNormals(direction,target);
		//AngleBetweenVectors(direction, target);
		float detection_angle_radians = angle * XM_PI / 180.0;
		float minas_detection_angle_radians = -angle * XM_PI / 180.0;
		return angleresult <= detection_angle_radians && angleresult >= minas_detection_angle_radians;
	}

	double Character::DotProduct(const Vec3& v1, const Vec3& v2)
	{
		return v1.x * v2.x + v1.z * v2.z;
	}

	double Character::Magnitude(const Vec3& v)
	{
		return std::sqrt(v.x * v.x + v.z * v.z);
	}

	void Character::ZoneSpeedSet()
	{
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();
		if ((state & Player::PlayerState::ZONE) == 0) {
			m_ZoneElapsedTime = 1.0f;
		}
		else {
			m_ZoneElapsedTime = 0.2f;
		}
	}
  
	inline float Character::GetElpasedTime() {
		return GetGameElapsed() * m_ZoneElapsedTime;
	}

	void Character::Move(const Vec3& direction,const bool& isGameSpeed) {
		float elapsed;
		if (isGameSpeed) {
			elapsed = GetGameElapsed();
		}
		else {
			elapsed = GetElapsed();
		}
		Vec3 position = GetPosition();
		position += direction * m_Speed * elapsed * m_ZoneElapsedTime;
		SetPosition(position);
	}

	FixedBox::FixedBox(const shared_ptr<Stage>& stage) :
		Object(stage)
	{
	}
	FixedBox::~FixedBox() {}

	void FixedBox::OnCreate()
	{
		//ここでm_Transformの中身取得してくれる
		Object::OnCreate();
		//操作系
		SetPosition(Vec3());
		SetScale(Vec3(1.0f));
		SetRotation(Vec3());
		Wicth_FixedBox = true;

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(Wicth_FixedBox);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
	}

	BraekBox::BraekBox(const shared_ptr<Stage>& stage) :
		Object(stage)
	{
	}
	BraekBox::~BraekBox() {}

	void BraekBox::OnCreate()
	{
		//ここでm_Transformの中身取得してくれる
		Object::OnCreate();
		//操作系
		SetPosition(Vec3(0, 1, -20));
		SetScale(Vec3(1.0f));
		SetRotation(Vec3(0));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);

		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		AddTag(L"BreakObject");
	}

	Wall::Wall(const shared_ptr<Stage>& stage) :
		GameObject(stage)
	{
	}
	Wall::~Wall() {}

	void Wall::OnCreate()
	{
		Wicth_Wall = true;
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(Vec3(0.0f, 0.0f, 5.0f));
		ptr->SetRotation(Vec3(0));
		ptr->SetScale(Vec3(5.0f, 3.0f, 0.5f));

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(Wicth_Wall);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");

		ptrDraw->SetOwnShadowActive(true);
	}

}
//end basecross
