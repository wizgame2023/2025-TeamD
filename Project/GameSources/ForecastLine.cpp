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
	}
	void ForecastLine::OnCreate()
	{
		auto transform = GetComponent<Transform>();
		transform->SetPosition(Vec3(10.0f, 0.25f, 10.0f));
		m_BalletLine = GetStage()->AddGameObject<LineCube>();
		m_Forecast = GetStage()->AddGameObject<LineCube>();

		m_BalletLine->m_Transform->SetPosition(transform->GetPosition() - Vec3(2.0f, 0.0f, 0.0f));
		m_BalletLine->m_Draw->SetDiffuse(Col4(1.0f, 1.0f, 0.0f,1.0f));
		m_Forecast->m_Transform->SetPosition(transform->GetPosition() + Vec3(2.0f, 0.0f, 0.0f));
		m_Forecast->m_Draw->SetDiffuse(Col4(1.0f, 0.0f, 0.0f, 1.0f));
		
	}
	void ForecastLine::SetLine(const Vec3& direction, const Vec3& startPosition, const float maxLength) {
		m_Direction = direction;
		m_StartPosition = startPosition;
		m_Length = maxLength;
	}
	void ForecastLine::OnUpdate()
	{
		float forecastSize = m_Length;
		float balletDistance = 0.0f;

		auto ballet = m_Ballet.lock();
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
	void ForecastLine::Destroy() {
		GetStage()->RemoveGameObject<LineCube>(m_BalletLine);
		GetStage()->RemoveGameObject<LineCube>(m_Forecast);
		GetStage()->RemoveGameObject<ForecastLine>(GetThis<ForecastLine>());
	}
}
//end basecross
