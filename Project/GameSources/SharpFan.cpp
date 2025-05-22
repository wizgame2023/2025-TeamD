#include "stdafx.h"
#include "Project.h"

namespace basecross {

	SharpFan::SharpFan(const shared_ptr<Stage>& stage, const wstring& texKey, int vertexSize, float drawAngle, float radius) : GameObject(stage),
		m_TexKey(texKey), m_DrawAngle(drawAngle), m_VerticesSize(vertexSize), m_Radius(radius), m_Forward(Vec3())
	{
	}

	void SharpFan::InitializeVertex() {
		float forwardRadian = atan2f(m_Forward.z, m_Forward.x);
		float startRadius = forwardRadian - XMConvertToRadians(m_DrawAngle / 2.0f);
		bool isFirst = true;
		if (m_Uv.size() != 0) {
			isFirst = false;
		}
		vector<VertexPositionColorTexture> newVertices = {};
		for (size_t i = 0; i < m_VerticesSize; i++) {
			float rad = startRadius + XMConvertToRadians(m_DrawAngle / m_VerticesSize * i);

			float u = 1.0f / static_cast<float>(m_VerticesSize) * static_cast<float>(i);
			float v = 1.0f;

			Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
			if (m_Vertices.size() != 0) {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), m_Vertices[2 * i].textureCoordinate));
			}
			else {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), Vec2(u, 0)));
			}

			position = Vec3(cos(rad), 0.0f, sin(rad)) * m_Radius;
			if (m_Vertices.size() != 0) {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), m_Vertices[2 * i + 1].textureCoordinate));
			}
			else {
				newVertices.push_back(VertexPositionColorTexture(position, Col4(1.0f, 0.0f, 0.0f, 1.0f), Vec2(u, v)));
			}

		}
		m_Vertices.clear();
		for (size_t i = 0; i < newVertices.size(); i++) {
			m_Vertices.push_back(newVertices[i]);
		}
		const vector<uint16_t> baseIndices = {
				2,1,0,
				3,1,2,
		};

		m_Indices.clear();
		m_Indices.reserve(m_VerticesSize - 1 * baseIndices.size());
		for (int i = 0; i < m_VerticesSize - 1; i++)
		{
			//baseIndices‚Ì”ƒ‹[ƒv
			for (auto baseIndex : baseIndices)
			{
				m_Indices.push_back(baseIndex + (2 * i));
			}
		}
	}
	void SharpFan::OnCreate() {
		InitializeVertex();
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
		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(Vec3(-10.0f, 0.1f, 2.0f));
	}

	void SharpFan::OnUpdate() {
		InitializeVertex();
		float elapsed = App::GetApp()->GetElapsedTime();
		for (auto& vertex : m_Vertices) {
			vertex.textureCoordinate -= m_Speed * elapsed;
		}
		m_Draw->UpdateVertices(m_Vertices);
	}
}
