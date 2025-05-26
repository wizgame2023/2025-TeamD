#include "stdafx.h"
#include "Project.h"

namespace basecross {
	AreaOfEffect::AreaOfEffect(const shared_ptr<Stage>& stage, Vec3 position, float size, int vertexCount, float time) : 
		Object(stage,position,Vec3(),Vec3(1)),m_AreaSize(size),m_AreaVertexCount(vertexCount),m_ExitTime(Timer(time)) {}

	void AreaOfEffect::OnCreate() {
		Object::OnCreate();
		m_AreaSizeEffect = m_Stage->AddGameObject<SharpFan>(L"SEARCH_RANGE", m_AreaVertexCount, 380.0f, m_AreaSize);
		m_AreaEffect = m_Stage->AddGameObject<SharpFan>(L"AOF", m_AreaVertexCount, 380.0f, 0);
	}
	void AreaOfEffect::OnUpdate() {
		m_AreaEffect->SetPosition(GetPosition());
		m_AreaSizeEffect->SetPosition(GetPosition());

		float elapsed = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
		float radius = m_AreaEffect->GetRadius();
		radius += m_AreaSize * elapsed / m_ExitTime.GetMaxTime();
		m_AreaEffect->SetRadius(m_AreaSize * (m_ExitTime.GetTime() / m_ExitTime.GetMaxTime()));

		if (m_ExitTime.UpdateTimer()) {
			m_Stage->RemoveGameObject<SharpFan>(m_AreaEffect);
			m_Stage->RemoveGameObject<SharpFan>(m_AreaSizeEffect);
			m_Stage->RemoveGameObject<AreaOfEffect>(GetThis<AreaOfEffect>());
		}
	}
}