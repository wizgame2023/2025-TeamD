/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void LineCube::OnCreate() {
		m_Transform = GetComponent<Transform>();

		m_BoneDraw = AddComponent<BcPNTStaticDraw>();
		m_BoneDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_BoneDraw->SetSamplerState(SamplerState::LinearWrap);
		m_BoneDraw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 0.1f));
		SetAlphaActive(true);

		AddTag(L"Line");
	}
	void ForecastLine::OnCreate()
	{
		auto transform = GetComponent<Transform>();
		transform->SetPosition(Vec3(10.0f, 0.25f, 10.0f));
		m_BalletLine = GetStage()->AddGameObject<LineCube>();
		m_Forecast = GetStage()->AddGameObject<LineCube>();

		m_BalletLine->m_Transform->SetPosition(transform->GetPosition() - Vec3(2.0f, 0.0f, 0.0f));
		m_BalletLine->m_BoneDraw->SetDiffuse(Col4(1.0f, 1.0f, 0.0f, 1.0f));
		m_Forecast->m_Transform->SetPosition(transform->GetPosition() + Vec3(2.0f, 0.0f, 0.0f));
		m_Forecast->m_BoneDraw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));

		//InitializeCriticalSection(&m_CriticalSection);
	}
	/// <summary>
	/// ラインの設定
	/// </summary>
	/// <param name="direction">方向</param>
	/// <param name="startPosition">初期位置</param>
	/// <param name="maxLength">長さ</param>
	void ForecastLine::SetLine(const Vec3& direction, const Vec3& startPosition, const float maxLength, const Col4& color) {
		m_Direction = direction;
		m_Direction.y = 0.0f;
		m_StartPosition = startPosition;
		m_Length = maxLength;
		m_DefaultColor = color;
	}
	/// <summary>
	/// オブジェクトとの距離を基に判別するかどうか
	/// </summary>
	/// <param name="position">オブジェクトの位置</param>
	/// <returns></returns>
	bool ForecastLine::CheckDistanceToObject(const shared_ptr<GameObject>& obj) {
		auto transform = obj->GetComponent<Transform>();
		Vec3 position = transform->GetPosition();
		Vec3 scale = transform->GetScale();
		float length = RayCast::CalcDistancePointToLine(position,Line(m_StartPosition, (m_StartPosition + m_Direction * m_Length)));
		Vec3 halfScale = scale / 2.0f;
		if(length > halfScale.length()){
			return false;
		}
		return true;
	}
	/// <summary>
	/// レイキャスト処理
	/// </summary>
	/// <param name="hitPoint">衝突地点</param>
	/// <returns>衝突したかどうか</returns>
	bool ForecastLine::CheckRayCast(Vec3& hitPoint) {
		shared_ptr<GameObject> launcher = m_Launcher.lock();
		m_NearestHitObject.reset();
		vector<wstring> excludeTags = { L"Bullet",L"Line",L"Enemy"};
		RayCastHit hit = RayCastHit();
		for (auto& obj : GetStage()->GetGameObjectVec()) {
			if (obj == launcher) continue;
			if (!CheckDistanceToObject(obj)) continue;
			RayCast::HitTest(hit, Line(m_StartPosition,(m_StartPosition + m_Direction * m_Length)), obj, excludeTags, true);
		}

		if (hit.m_Object != nullptr) {
			m_NearestHitObject = hit.m_Object;
			hitPoint = hit.m_HitPosition;
			return true;
		}
		return false;
	}
	void ForecastLine::OnUpdate()
	{
		float forecastSize = m_Length;
		Vec3 intersectPosition;
		m_NearestHitObject.reset();
		if (GetDrawActive() && m_IsRay) {
			if (CheckRayCast(intersectPosition)) {
				forecastSize = (intersectPosition - m_StartPosition).length();
			}
		}
		float balletDistance = 0.0f;

		auto ballet = m_Bullet.lock();
		if (ballet != nullptr) {
			m_IsLaunched = true;
			Vec3 direction = ballet->GetComponent<Transform>()->GetPosition() - m_StartPosition;
			balletDistance = direction.length();
			forecastSize -= balletDistance;
		}
		else {
			if (m_IsLaunched) {
				Destroy();
			}
		}
		float rad = atan2f(-m_Direction.z, m_Direction.x);

		auto& balletTransform = m_BalletLine->m_Transform;
		auto& forecastTransform = m_Forecast->m_Transform;
		auto& balletDraw = m_BalletLine->m_BoneDraw;
		auto& forecastDraw = m_Forecast->m_BoneDraw;

		balletTransform->SetRotation(Vec3(0, rad, 0));
		forecastTransform->SetRotation(Vec3(0, rad, 0));

		balletTransform->SetScale(Vec3(balletDistance, 0.02f, 0.02f));
		forecastTransform->SetScale(Vec3(forecastSize, 0.02f, 0.02f));

		balletTransform->SetPosition(m_StartPosition + m_Direction * balletDistance / 2.0f);
		forecastTransform->SetPosition(m_StartPosition + m_Direction * (balletDistance + forecastSize / 2.0f));

		forecastDraw->SetDiffuse(m_DefaultColor);

		m_BalletLine->SetDrawActive(GetDrawActive());
		m_Forecast->SetDrawActive(GetDrawActive());

	}
	/// <summary>
	/// ラインの削除
	/// </summary>
	void ForecastLine::Destroy() {
		GetStage()->RemoveGameObject<LineCube>(m_BalletLine);
		GetStage()->RemoveGameObject<LineCube>(m_Forecast);
		GetStage()->RemoveGameObject<ForecastLine>(GetThis<ForecastLine>());
	}
}
//end basecross
