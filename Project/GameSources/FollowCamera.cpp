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
			else {
				return m_CameraPos;
			}

			return m_GetPosition;
		}

	}

	Vec3 CameraCollision::GetCompareVertex(Vec2 verx, Vec2 very) {
		float m_Side, m_Beside;
		Vec2 target = Vec2(m_TargetPosition.x, m_TargetPosition.z);
		Vec2 get = Vec2(m_GetPosition.x, m_GetPosition.z);
		Vec2 afterget = /*Vec2(0.0f, 0.0f);*/ Vec2(m_CameraPos.x, m_CameraPos.z);
		if (afterget == Vec2(0.0f)) {
			afterget = get;
		}
		if ((afterget - get).length() < 12.5f)
		{
			afterget = get;
			//return m_GetPosition;
		}
		Vec2 closeget = afterget;
		float crossget = Cross(very - verx, target - closeget);
		//else {
		//	return m_CameraPos;
		//}

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
		m_Stage(StagePtr),
		m_StopCamera(false),
		m_Width(0),
		m_Height(0),
		m_HitCollision(false),
		m_IsShaking(false),
		m_Duration(0.0f),
		m_InitialDuration(0.0f),
		m_Magnification(4.0f),
		m_Up(1.5f),
		m_CenterPt({ 0, 0 }), 
		m_CurrntTime(0.0f), 
		m_Magnitude(0.0f), 
		m_MouseSensitivityX(0.001f),
		m_MouseUpdateInterval(5),
		m_FrameCounter(0)
	{
	}
	void FollowCamera::OnCreate()
	{
		auto& app = App::GetApp();
		m_Width = app->GetGameWidth();
		m_Height = app->GetGameHeight();

		// 画面中央を保持
		m_CenterPt.x = m_Width / 2;
		m_CenterPt.y = m_Height / 2;

		// カーソルを中央に移動／非表示
		::SetCursorPos(m_CenterPt.x, m_CenterPt.y);
		ShowCursor(FALSE);

		// ウィンドウ内にカーソルをクリップ
		RECT rc{ 0, 0, m_Width, m_Height };
		::ClientToScreen(app->GetHWnd(), (POINT*)&rc.left);
		::ClientToScreen(app->GetHWnd(), (POINT*)&rc.right);
		ClipCursor(&rc);

		m_HitCollision = false;

	}

	void FollowCamera::OnUpdate() {
		//コントローラの取得
		auto& app = App::GetApp();
		auto& cntlVec = app->GetInputDevice().GetControlerVec()[0];
		float elapsed = app->GetElapsedTime();
		if (m_StopCamera) return;
		//コントローラ入力(つながっていないならマウス操作)
		if (cntlVec.bConnected) {
			m_Angle -= m_RotateSpeed * elapsed * cntlVec.fThumbRX;

			// Pitch
			m_Pitch += m_RotateSpeed * elapsed * cntlVec.fThumbRY;
		}
		else {
			++m_FrameCounter;
			if (m_FrameCounter >= m_MouseUpdateInterval)
			{
				m_FrameCounter = 0;
				// 中央に戻す
				::SetCursorPos(m_CenterPt.x, m_CenterPt.y);
			}
			// マウスフレーム制御は省略
			POINT now; ::GetCursorPos(&now);
			float dx = float(now.x - m_CenterPt.x);
			float dy = float(now.y - m_CenterPt.y);

			// Yaw
			m_Angle -= dx * m_MouseSensitivityX;
			// Pitch（符号は好みで反転可）
			m_Pitch += dy * m_MouseSensitivityX * 0.1f;
			//m_CenterPt = now;
		}

		m_Pitch = max(m_MinPitch, min(m_MaxPitch, m_Pitch));
		//方向
		m_Direction = Vec3(cos(m_Angle), 0.0f, sin(m_Angle));
		m_Direction.y = sinf(m_Pitch);
		//位置
		m_Position = m_PlayerTransform->GetPosition();
		Vec2 dire = CameraApproach(0.75f);
		m_Eye = m_Position + m_Direction * dire.x;
		m_Eye.y = m_Position.y + dire.y;

		RayCastHit hit;
		vector<wstring> excludeTags = { L"Bullet",L"Line",L"Enemy",L"Player",L"LimitArea", L"Ground" };
		RayCast::HitTestVec(hit, Line(m_PlayerTransform->GetPosition(), m_Eye), m_Stage->GetGameObjectVec(), excludeTags);
		if (hit.m_Object != nullptr) {
			m_Eye = hit.m_HitPosition - m_Direction * 0.5f;
		}

		Vec3 m_addEye = ShakeCameraMove();
		m_Eye += m_addEye; // カメラの振動を適用

		if (m_IsSmoothLook) {
			m_LerpElapsed += elapsed;

			m_Angle = Lerp::CalculateLerp(m_StartAngle, m_TargetAngle,0.0f, m_LerpDuration,m_LerpElapsed, Lerp::rate::Linear);

			// 補完完了 or デッドゾーン内到達を判定
			float yawDiff = fabsf(CalcAngleDiff(m_Angle, m_TargetAngle));
			bool reachedYaw = (yawDiff < m_YawDeadZone);

			if (m_LerpElapsed >= m_LerpDuration || (reachedYaw)) {
				m_IsSmoothLook = false;
				m_Angle = m_TargetAngle;
			}
		}
		//m_Eye = m_CameraCollision->GetAfterPosition(m_Eye, m_Position);
		if (m_StopCamera == false) {
			//自分の位置
			SetEye(m_Eye);
			//見ているところ
			SetAt(m_PlayerTransform->GetPosition() - m_Direction * 1.0f);
		}
		//LogCamera();
		Camera::OnUpdate();
	}

	void FollowCamera::SetCameraPause(const bool& StopCamera)
	{
		m_StopCamera = StopCamera;
	}

	shared_ptr<GameObject> FollowCamera::ObjectSearch(const shared_ptr<GameObjectGroup>& group) {
		const auto& list = group->GetGroupVectors();
		Vec3 position = m_PlayerTransform->GetPosition();

		shared_ptr<GameObject> nearest = nullptr;
		float bestSqrDist = std::numeric_limits<float>::infinity();

		for (auto& weakObj : list) {
			if (auto obj = weakObj.lock()) {
				Vec3 objPos = obj->GetComponent<Transform>()->GetPosition();
				Vec3 diff = objPos - position;
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				if (sqrDist < bestSqrDist) {
					bestSqrDist = sqrDist;
					nearest = obj;
				}
			}
		}
		return nearest;
	}

	float FollowCamera::CalcAngleDiff(float from, float to) {
		// 差分を [-π, +π] に丸める
		float diff = fmodf(to - from + XM_PI, XM_2PI) - XM_PI;
		return diff;
	}

	void FollowCamera::LookAtNearestEnemy() {
		auto enemyGroup = m_Stage->GetSharedObjectGroup(L"EnemyGroup");
		auto nearestEnemy = ObjectSearch(enemyGroup);
		if (!nearestEnemy) return;

		Vec3 playerPos = m_PlayerTransform->GetPosition();
		Vec3 enemyPos = nearestEnemy->GetComponent<Transform>()->GetPosition();

		// 方向ベクトルを正規化
		Vec3 dir = enemyPos - playerPos;
		dir.normalize();

		// 開始角度を保存
		m_StartAngle = m_Angle;
		m_StartPitch = m_Pitch;

		// 目標角度を計算
		m_TargetAngle = std::atan2(dir.z, dir.x);
		m_TargetPitch = std::asin(dir.y);

		// デッドゾーン内ならそのままセットして終了
		float yawDiff = fabsf(CalcAngleDiff(m_Angle, m_TargetAngle));
		float pitchDiff = fabsf(m_Pitch - m_TargetPitch);

		if (yawDiff < m_YawDeadZone) {
			// 角度差が小さいので即時適用し、補完も開始しない
			m_Angle = m_TargetAngle;
			m_Pitch = m_TargetPitch;
			m_IsSmoothLook = false;
			return;
		}

		// 補完開始
		m_LerpElapsed = 0.0f;
		m_IsSmoothLook = true;
	}
	Vec2 FollowCamera::CameraApproach(float totaltime)
	{
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		float up = m_Up;
		float interpHeight = m_Magnification;
		if (m_IsShaking)
		{
			m_CurrntTime += ElapsedTime;

			if(m_CurrntTime <= totaltime / 3)
			{
				interpHeight = Lerp::CalculateLerp(m_Magnification, m_Magnification * 0.65f, 0.0f, totaltime / 5, m_CurrntTime, Lerp::rate::EaseOut);
				up = Lerp::CalculateLerp(m_Up, 0.5f, 0.0f, totaltime / 4, m_CurrntTime, Lerp::rate::Easein);
			}
			else if(m_CurrntTime <= totaltime)
			{
				ShakeStart(0.1f, 0.1f);
				interpHeight = Lerp::CalculateLerp(m_Magnification * 0.65f, m_Magnification , totaltime / 1.5, totaltime, m_CurrntTime, Lerp::rate::EaseOut);
				up = Lerp::CalculateLerp(0.5f,m_Up, totaltime / 1.5, totaltime, m_CurrntTime, Lerp::rate::Easein);
			}
			if (m_CurrntTime > totaltime) {
				m_IsShaking = false;
				m_CurrntTime = 0.0f;
				return Vec2(m_Magnification, m_Up);
			}
			return Vec2(interpHeight, up);
		}
		return Vec2(m_Magnification, m_Up);
	}

	void FollowCamera::ResetCursorPosition()
	{
		::SetCursorPos(m_CenterPt.x, m_CenterPt.y);
	}

	void FollowCamera::LogCamera() {

		auto scene = App::GetApp()->GetScene<Scene>();

		wstringstream Debug(L"¥n");
		Debug << L"¥nForward.x : " << m_Direction.x
			<< L"¥nForward.y : " << m_Direction.y
			<< L"¥nForward.z : " << m_Direction.z << endl;

		scene->SetDebugString(Debug.str());

	}
}
