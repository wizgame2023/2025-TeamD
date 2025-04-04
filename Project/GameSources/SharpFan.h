#pragma once
#include "stdafx.h"

namespace basecross {

	class SharpFan : public GameObject {
		shared_ptr<PCTStaticDraw> m_BoneDraw;
		shared_ptr<Transform> m_BoneTransform;

		wstring m_TexKey;
		float m_DrawAngle;
		int m_VerticesSize;
		float m_Radius;

		Vec3 m_Forward;

		vector<VertexPositionColorTexture> m_Vertices;
		vector<uint16_t> m_Indices;
		vector<Vec2> m_Uv;
	public:
		SharpFan(const shared_ptr<Stage>& stage, const wstring& texKey, int vertexSize, float drawAngle, float radius);
		virtual ~SharpFan() {}

		void InitializeVertex();

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetDrawAngle(float angle) {
			m_DrawAngle = angle;
		}
		void SetRadius(float radius) {
			m_Radius = radius;
		}
		void SetForward(Vec3 forward) {
			m_Forward = forward;
		}
		void SetPosition(Vec3 position) {
			m_BoneTransform->SetPosition(position);
		}
	};
}