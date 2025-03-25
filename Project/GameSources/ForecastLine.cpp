/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void LineCube::OnCreate() {
		m_Transform = GetComponent<Transform>();

		m_Draw = AddComponent<BcPNTStaticDraw>();
		m_Draw->SetMeshResource(L"DEFAULT_CUBE");
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 0.1f));
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
		m_BalletLine->m_Draw->SetDiffuse(Col4(1.0f, 1.0f, 0.0f, 1.0f));
		m_Forecast->m_Transform->SetPosition(transform->GetPosition() + Vec3(2.0f, 0.0f, 0.0f));
		m_Forecast->m_Draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));

	}
	/// <summary>
	/// ラインの設定
	/// </summary>
	/// <param name="direction">方向</param>
	/// <param name="startPosition">初期位置</param>
	/// <param name="maxLength">長さ</param>
	void ForecastLine::SetLine(const Vec3& direction, const Vec3& startPosition, const float maxLength) {
		m_Direction = direction;
		m_StartPosition = startPosition;
		m_Length = maxLength;
	}
	/// <summary>
	/// オブジェクトとの距離を基に判別するかどうか
	/// </summary>
	/// <param name="position">オブジェクトの位置</param>
	/// <returns></returns>
	bool ForecastLine::CheckDistanceToObject(Vec3 position) {
		float startDistanceSq = (position - m_StartPosition).lengthSqr();
		float endDistanceSq = (position - (m_StartPosition + m_Direction * m_Length)).lengthSqr();
		float lengthSq = m_Length * m_Length;
		if (startDistanceSq > lengthSq || endDistanceSq > lengthSq) {
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
		vector<wstring> excludeTags = { L"Bullet",L"Line" };
		RayCastHit hit = RayCastHit();
		for (auto& obj : GetStage()->GetGameObjectVec()) {
			if (obj == launcher) continue;
			Vec3 objPosition = obj->GetComponent<Transform>()->GetPosition();
			if (!CheckDistanceToObject(objPosition)) continue;
			RayCast::HitTest(hit,m_StartPosition, m_Direction, m_Length, obj, excludeTags);
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

		balletTransform->SetRotation(Vec3(0, rad, 0));
		forecastTransform->SetRotation(Vec3(0, rad, 0));

		balletTransform->SetScale(Vec3(balletDistance, 0.02f, 0.02f));
		forecastTransform->SetScale(Vec3(forecastSize, 0.02f, 0.02f));

		balletTransform->SetPosition(m_StartPosition + m_Direction * balletDistance / 2.0f);
		forecastTransform->SetPosition(m_StartPosition + m_Direction * (balletDistance + forecastSize / 2.0f));

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
	/// <summary>
	/// レイキャスト処理
	/// </summary>
	/// <param name="hit">結果</param>
	/// <param name="startPosition">発射位置</param>
	/// <param name="direction">発射方向</param>
	/// <param name="length">長さ</param>
	/// <param name="object">調べるオブジェクト</param>
	/// <param name="excludeTags">除外するタグ</param>
	/// <returns>当たったか</returns>
	bool RayCast::HitTest(RayCastHit& hit,const Vec3& startPosition, const Vec3& direction, float length, shared_ptr<GameObject>& object, const vector<wstring> excludeTags) {
		RayCastHit newResult = RayCastHit();
		if (object == nullptr) return false;

		bool isExclude = false;
		for (auto& tag : excludeTags) {
			if (object->FindTag(tag)) {
				isExclude = true;
				break;
			}
		}
		if (isExclude) return false;
		bool isHit = false;
		auto draw = object->GetComponent<SmBaseDraw>(false);
		if (draw != nullptr) {
			isHit = draw->HitTestStaticMeshSegmentTriangles(startPosition, startPosition + direction * length, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
		}
		else {
			auto bcDraw = object->GetComponent<BcBaseDraw>(false);
			if (bcDraw != nullptr) {
				isHit = bcDraw->HitTestStaticMeshSegmentTriangles(startPosition, startPosition + direction * length, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
			}
		}

		if (isHit) {
			if (hit.m_Object == nullptr) {
				hit.m_Object = object;
				hit = newResult;
			}
			else if ((hit.m_HitPosition - startPosition).length() > (newResult.m_HitPosition - startPosition).length()) {
				hit.m_Object = object;
				hit = newResult;
			}
			return true;
		}
		return false;
	}
}
//end basecross
