/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	bool CameraCollision::m_Hit = false;

	CameraCollision::CameraCollision(const shared_ptr<Stage>& StagePtr)
		:GameObject(StagePtr), m_GetPosition(Vec3(0.0f, 0.0f, 0.0f)), m_TargetPosition(Vec3(0.0f, 0.0f, 0.0f)), m_CameraPos(Vec3(0.0f, 0.0f, 0.0f)) {
	}

	void CameraCollision::OnCreate() {
		m_Collision = AddComponent<CollisionSphere>();
		m_Collision->SetAfterCollision(AfterCollision::None);
	}

	void CameraCollision::OnUpdate() {
		auto positionTransform = GetComponent<Transform>();
		//追尾システム
		GetComponent<Transform>()->SetPosition(m_GetPosition);
	}

	void CameraCollision::OnCollisionEnter(shared_ptr<GameObject>& other) {
		m_HitObject = other;
		if (other->FindTag(L"Object"))
		{
			m_Hit = true;
		}
		if (other->FindTag(L"Player"))
		{
			m_Point = true;
		}

	}
	void CameraCollision::OnCollisionExit(shared_ptr<GameObject>& other) {
		m_HitObject = nullptr;
		if (other->FindTag(L"Object"))
		{
			m_Hit = false;
		}
		if (other->FindTag(L"Player"))
		{
			m_Point = false;
		}

	}
	Vec3 CameraCollision::GetAfterPosition(Vec3 beforePosition, Vec3 tergetPosi) {
		m_TargetPosition = tergetPosi;
		m_GetPosition = beforePosition;
		if (m_Hit == false) {
			return m_GetPosition;
		}
		if (m_Hit == true) {
			auto collision = m_HitObject->GetComponent<Collision>();
			AABB aabb = collision->GetEnclosingAABB();
			aabb = AABB(aabb.m_Min - Vec3(0.5f, 0.5f, 0.5f), aabb.m_Max + Vec3(0.5f, 0.5f, 0.5f));
			//底面
			m_GetPosition = GetCompareVertex(Vec2(aabb.m_Min.x, aabb.m_Min.z), Vec2(aabb.m_Max.x, aabb.m_Min.z));
			m_GetPosition = GetCompareVertex(Vec2(aabb.m_Min.x, aabb.m_Min.z), Vec2(aabb.m_Min.x, aabb.m_Max.z));
			m_GetPosition = GetCompareVertex(Vec2(aabb.m_Min.x, aabb.m_Max.z), Vec2(aabb.m_Max.x, aabb.m_Max.z));
			m_GetPosition = GetCompareVertex(Vec2(aabb.m_Max.x, aabb.m_Min.z), Vec2(aabb.m_Max.x, aabb.m_Max.z));

			if (m_CameraPos == Vec3(0.0f)) {
				m_CameraPos = m_GetPosition;
			}
			if ((m_CameraPos - m_GetPosition).length() < 12.5f)
			{
				m_CameraPos = m_GetPosition;
				return m_GetPosition;
			}
			else{ 
				return m_CameraPos; 
			}

			return m_CameraPos;
		}

	}

	Vec3 CameraCollision::GetCompareVertex(Vec2 verx, Vec2 very) {
		float m_Side, m_Beside;
		Vec2 target = Vec2(m_TargetPosition.x, m_TargetPosition.z);
		Vec2 get = Vec2(m_GetPosition.x, m_GetPosition.z);
		Vec2 afterget = Vec2(0.0f,0.0f);
		float dar = Cross(very - verx, target - get);
		if (dar == 0.0f) return m_GetPosition;
		m_Side = Cross(get - verx, target - get) / dar;
		m_Beside = Cross(very - verx, verx - get) / dar;
		if (m_Side < 0.0f || m_Side > 1.0f || m_Beside < 0.0f || m_Beside > 1.0f) {
			return m_GetPosition;
		}
		Vec3 anser(verx.x + m_Side * (very - verx).x, m_GetPosition.y, verx.y + m_Side * (very - verx).y);
		return anser;

	}

	float CameraCollision::Cross(Vec2 crox, Vec2 croy) {
		return crox.x * croy.y - crox.y * croy.x;
	}

	FollowCamera::FollowCamera(const shared_ptr<Stage>& StagePtr) :
		Camera(),
		m_Direction(Vec3(0)),
		m_Eye(Vec3(0)),
		m_Position(Vec3(0)),
		m_Angle(-XM_PIDIV2),
		m_RotateSpeed(XMConvertToRadians(180)),
		m_Stage(StagePtr)
	{
	}

	void FollowCamera::OnCreate()
	{
		auto& app = App::GetApp();

		m_Width = app->GetGameWidth();
		m_Height = app->GetGameHeight();
		// ゲーム開始時にマウスカーソルを画面の中央に移動させる
		int m_centerX = app->GetGameWidth() / 2;
		int m_centerY = app->GetGameHeight() / 2;
		::SetCursorPos(m_centerX, m_centerY);
		m_HitCollision = false;

		//m_CameraCollision = m_Stage->AddGameObject<CameraCollision>();
	}

	void FollowCamera::OnUpdate() {
		//コントローラの取得
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		float elapsed = App::GetApp()->GetElapsedTime();
		if (cntlVec.bConnected) {
			float stickX = cntlVec.fThumbRX;
			m_Angle -= m_RotateSpeed * elapsed * stickX;
		}


		//方向
		m_Direction = Vec3(cos(m_Angle), 0.0f, sin(m_Angle));
		//位置
		m_Position = m_PlayerTransform->GetPosition();

		m_Eye = m_Position + m_Direction * 2.0f;
		m_Eye.y = m_Position.y + 0.5f;
		RayCastHit hit;
		vector<wstring> excludeTags = { L"Bullet",L"Line",L"Enemy",L"Player"};
		for (auto& obj : m_Stage->GetGameObjectVec()) {

			auto transform = obj->GetComponent<Transform>();
			Vec3 position = transform->GetPosition();
			Vec3 scale = transform->GetScale();
			float length = RayCast::CalcDistanceXZPointToLine(position,Line(m_PlayerTransform->GetPosition(),m_Eye));
			Vec3 h = Vec3(scale.x, 0, scale.z) / 2.0f;
			if (length < h.length()) {
				RayCast::HitTest(hit, Line(m_PlayerTransform->GetPosition(), m_Eye), obj, excludeTags);
			}
		}
		if (hit.m_Object != nullptr) {
			m_Eye = hit.m_HitPosition - m_Direction * 0.5f;
		}
		//m_Eye = m_CameraCollision->GetAfterPosition(m_Eye, m_Position);
		//自分の位置
		SetEye(m_Eye);
		//見ているところ
		SetAt(m_PlayerTransform->GetPosition());
		LogCamera();

		Camera::OnUpdate();

	}

	void FollowCamera::LogCamera() {

		auto scene = App::GetApp()->GetScene<Scene>();

		wstringstream Debug(L"\n");
		Debug << L"\nForward.x : " << m_Direction.x
			<< L"\nForward.y : " << m_Direction.y
			<< L"\nForward.z : " << m_Direction.z << endl;

		scene->SetDebugString(Debug.str());

	}

}