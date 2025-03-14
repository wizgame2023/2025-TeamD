#include "stdafx.h"
#include "Project.h"
#include "Tube.h"

namespace basecross {
	void Tube::InitializeVertices()
	{
		//頂点データ
		m_vertices.clear();
		m_vertices.reserve((m_angle/* / 2*/ + 1) * 2);
		for (int i = 0; i <= m_angle/* / 2*/; i++)
		{
			Vec3 position; //頂点座標
			VertexPositionColorTexture vtx; //1頂点のデータ

			//上下の頂点の共通データ
			float rad = XMConvertToRadians(360.0f * i / m_angle);
			
			float u = m_loops.x * static_cast<float>(i) / static_cast<float>(m_angle);
			float v = m_loops.y;

			
			//上の円の頂点
			position = Vec3(cosf(rad), 0.0f, sinf(rad)) * m_topRadius + Vec3(0.0f,m_height,0.0f);//Y座標をm_height分ずらす
			position = RotatePosition(position);
			vtx = VertexPositionColorTexture(position, m_topColor, Vec2(u, 0.0f));
			m_vertices.push_back(vtx);

			position = Vec3(cosf(rad), 0.0f, sinf(rad)) * m_bottomRadius;
			position = RotatePosition(position);
			vtx = VertexPositionColorTexture(position, m_bottomColor, Vec2(u, v));//v座標を最大値にする
			m_vertices.push_back(vtx);

			
		}

		//頂点インデックス(頂点を繋げる順番)
		const vector<uint16_t> baseIndices = {
			2,1,0,
			3,1,2,
		};

		m_indices.clear();
		m_indices.reserve(m_angle/* / 2*/ * baseIndices.size() );
		for (int i = 0; i < m_angle/* / 2*/; i++)
		{
			//baseIndicesの数ループ
			for (auto baseIndex : baseIndices)
			{
				m_indices.push_back(baseIndex + (2 * i));
			}
		}
	}
	Vec3 Tube::RotatePosition(Vec3 position)
	{
		for (int i = 0; i < m_axis.size(); i++)
		{
			Vec3 axis = m_axis[i];
			float spin = m_spinAngle[i];
			float inner = position.x * axis.x + position.y * axis.y + position.z * axis.z;
			Vec3 cross = Vec3(
				position.y * axis.z - position.z * axis.y,
				position.z * axis.x - position.x * axis.z,
				position.x * axis.y - position.y * axis.x
			);
			/*a2b3−a3b2,a3b1−a1b3,a1b2−a2b1*/
			position = cos(spin) * position + (1 - cos(spin)) * (inner * axis) + (sin(spin) * cross);
		}
		

		return position;
	}
	void Tube::OnCreate()
	{
		InitializeVertices();

		m_drawComp = AddComponent<BcPCTStaticDraw>();
		m_drawComp->SetOriginalMeshUse(true);
		m_drawComp->CreateOriginalMesh(m_vertices, m_indices);
		m_drawComp->SetSamplerState(SamplerState::LinearWrap);
		m_drawComp->SetDepthStencilState(DepthStencilState::Read);
		m_drawComp->SetBlendState(BlendState::Additive);
		if (m_textureKeyName.length() > 0)
		{
			m_drawComp->SetTextureResource(m_textureKeyName);
		}
		//アルファ値の反映
		SetAlphaActive(true);
	}
	
	void Tube::OnUpdate()
	{
		float delta = App::GetApp()->GetElapsedTime();
		if (m_scrollVelocity.length() > 0.0f && m_textureKeyName != L"")
		{
			for (auto& vertex : m_vertices)
			{
				vertex.textureCoordinate += m_scrollVelocity * delta;
			}
		}

		if (m_isUpdate)
		{
			m_isUpdate = false;
			InitializeVertices();
		}
		if (m_isTimer)
		{
			m_timer -= delta;
			if (m_timer < 0.0f)
			{
				GetStage()->RemoveGameObject<Tube>(GetThis<Tube>());
				if (m_isTracking)
				{
					PostEvent(0.0f, GetThis<ObjectInterface>(), m_obj.lock(), L"DESTROY");
				}
			}
		}
		if (m_isTracking && !m_isTrackingCamera)
		{
			auto obj = m_obj.lock();
			auto trans = GetComponent<Transform>();
			if (obj != nullptr) {
				auto forward = obj->GetComponent<Transform>()->GetForward();
				forward.y = 0.0f;

				auto side = forward;
				trans->SetPosition(obj->GetComponent<Transform>()->GetPosition() + forward * m_diffTracking.x + Vec3(0, m_diffTracking.y, 0));
			}
		}

		if (m_isTrackingCamera)
		{

		}
		m_drawComp->UpdateVertices(m_vertices);
	}
}