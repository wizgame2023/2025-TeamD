#include "stdafx.h"
#include "Project.h"

namespace basecross {
	AreaOfEffect::AreaOfEffect(const shared_ptr<Stage>& stage, Vec3 position, Vec3 forward, CIRCLE circle, float time) :
		Object(stage, position, Vec3(), Vec3(1)), m_Forward(forward), m_ExitTime(Timer(time)),m_AreaType(TYPE::CIRCLE),m_Circle(circle),m_Rect{}
	{}
	AreaOfEffect::AreaOfEffect(const shared_ptr<Stage>& stage, Vec3 position, Vec3 forward, RECTANGLE rect, float time) :
		Object(stage, position, Vec3(), Vec3(1)),m_Forward(forward), m_ExitTime(Timer(time)), m_AreaType(TYPE::RECT), m_Rect(rect), m_Circle{}
	{}


	void AreaOfEffect::OnCreate() {
		Object::OnCreate();
		switch (m_AreaType) {
		case TYPE::CIRCLE:
			CIRCLE circle = m_Circle;
			circle.m_Radius = 0.0f;

			m_AreaSizeEffect = m_Stage->AddGameObject<GroundEffect>(L"SEARCH_RANGE", m_Circle);
			m_AreaEffect = m_Stage->AddGameObject<GroundEffect>(L"AOF", circle);
			break;
		case TYPE::RECT:
			RECTANGLE rect = {};

			m_AreaSizeEffect = m_Stage->AddGameObject<GroundEffect>(L"SEARCH_RANGE", m_Rect);
			m_AreaEffect = m_Stage->AddGameObject<GroundEffect>(L"AOF", rect);
			break;
		}
	}
	void AreaOfEffect::OnUpdate() {
		m_AreaEffect->SetPosition(GetPosition());
		m_AreaSizeEffect->SetPosition(GetPosition());
		m_AreaEffect->SetForward(m_Forward);
		m_AreaSizeEffect->SetForward(m_Forward);

		float elapsed = GetGameElapsed() / m_ExitTime.GetMaxTime();

		switch (m_AreaType) {
		case TYPE::CIRCLE: {
			CIRCLE circle = m_AreaEffect->GetCircle();

			circle.m_Radius += m_Circle.m_Radius * elapsed;
			m_AreaEffect->SetCircle(circle);
			break;
		}
		case TYPE::RECT: {
			RECTANGLE rect = m_AreaEffect->GetRect();

			rect.left += m_Rect.left * elapsed;
			rect.right += m_Rect.right * elapsed;
			rect.top += m_Rect.top * elapsed;
			rect.bottom += m_Rect.bottom * elapsed;

			m_AreaEffect->SetRect(rect);
			break;
		}
		}

		/*float radius = m_AreaEffect->GetRadius();
		radius += m_AreaSize * elapsed / m_ExitTime.GetMaxTime();
		m_AreaEffect->SetRadius(m_AreaSize * (m_ExitTime.GetTime() / m_ExitTime.GetMaxTime()));*/

		if (m_ExitTime.UpdateTimer(GameManager::Instance()->GetTimeRate())) {
			m_Stage->RemoveGameObject<GroundEffect>(m_AreaEffect);
			m_Stage->RemoveGameObject<GroundEffect>(m_AreaSizeEffect);
			m_Stage->RemoveGameObject<AreaOfEffect>(GetThis<AreaOfEffect>());
		}
	}



	void GroundEffect::OnCreate() {
		Object::OnCreate();

		vector<Vec3> positions;

		switch (m_AreaType) {
		case TYPE::CIRCLE:
			positions = GetVertexPositionsCircle();
			break;

		case TYPE::RECT:
			positions = GetVertexPositionsRect();
			break;
		}
		Init(positions, 0.0f);
		m_Draw = AddComponent<PCTStaticDraw>();
		m_Draw->SetOriginalMeshUse(true);
		m_Draw->CreateOriginalMesh(m_Vertices, m_Indices);
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDepthStencilState(DepthStencilState::Read);
		m_Draw->SetBlendState(BlendState::Additive);
		SetAlphaActive(true);
		if (m_TexKey != L"") {
			m_Draw->SetTextureResource(m_TexKey);
		}

		
	}
	void GroundEffect::OnUpdate() {
		vector<Vec3> positions;

		switch (m_AreaType) {
		case TYPE::CIRCLE:
			positions = GetVertexPositionsCircle();
			break;

		case TYPE::RECT:
			positions = GetVertexPositionsRect();
			break;
		}
		if (positions.size() > 0) {
			Init(positions, m_ForwardRad);
			m_Draw->UpdateVertices(m_Vertices);
		}
	}

	void GroundEffect::Init(vector<Vec3>& positions, float rad) {

		auto rotate = XMMatrixRotationY(rad);
		
		vector<VertexPositionColorTexture> newVertices = {};
		size_t vertexSize = positions.size();

		newVertices.reserve(vertexSize * 2);

		for (size_t i = 0; i < vertexSize; i++) {
			float u = 1.0f / static_cast<float>(vertexSize) * static_cast<float>(i);
			float v = 1.0f;

			Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
			if (m_Vertices.size() != 0) {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), m_Vertices[2 * i].textureCoordinate));
			}
			else {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), Vec2(u, 0)));
			}

			XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&positions[i]), rotate));

			if (m_Vertices.size() != 0) {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), m_Vertices[2 * i + 1].textureCoordinate));
			}
			else {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), Vec2(u, v)));
			}

		}
		m_Vertices.clear();
		m_Vertices = newVertices;
		const vector<uint16_t> baseIndices = {
				2,1,0,
				3,1,2,
		};

		m_Indices.clear();
		m_Indices.reserve((vertexSize - 1) * baseIndices.size());
		for (int i = 0; i < vertexSize - 1; i++)
		{
			for (auto baseIndex : baseIndices)
			{
				m_Indices.push_back(baseIndex + (2 * i));
			}
		}
	}


	vector<Vec3> GroundEffect::GetVertexPositionsCircle() {
		vector<Vec3> positions;
		positions.reserve(m_Circle.m_VertexCount);
		for (int i = 0; i <= m_Circle.m_VertexCount; i++) {
			float rad = XMConvertToRadians(m_Circle.m_Angle / m_Circle.m_VertexCount * i);

			positions.push_back(Vec3(cos(rad), 0.0f, sin(rad)) * m_Circle.m_Radius);
		}

		return positions;
	}

	vector<Vec3> GroundEffect::GetVertexPositionsRect() {
		vector<Vec3> positions;
		positions.resize(5);
		float width = m_Rect.right - m_Rect.left;
		float height = m_Rect.bottom - m_Rect.top;

		positions[0] = Vec3(-width * 0.5f, 0.0f, -height * 0.5f);
		positions[1] = Vec3(width * 0.5f, 0.0f, -height * 0.5f);
		positions[2] = Vec3(width * 0.5f, 0.0f, height * 0.5f);
		positions[3] = Vec3(-width * 0.5f, 0.0f, height * 0.5f);
		positions[4] = positions[0];

		return positions;
	}
}
