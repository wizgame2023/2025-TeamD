#pragma once
#include "stdafx.h"

namespace basecross {
	class Tube : public GameObject
	{
	protected :
		std::vector<VertexPositionColorTexture> m_vertices;
		std::vector<uint16_t> m_indices;

		std::shared_ptr<BcPCTStaticDraw> m_drawComp;
		float m_height;
		float m_topRadius;
		float m_bottomRadius;
		int m_VerticesSize;
		Vec2 m_loops;
		Col4 m_topColor;
		Col4 m_bottomColor;
		std::wstring m_textureKeyName;
		Vec2 m_scrollVelocity;
		vector<Vec3> m_axis;
		vector<float> m_spinAngle;
		float m_timer;
		weak_ptr<GameObject> m_obj;
		Vec3 m_diffTracking;

		bool m_isUpdate = false;
		bool m_isTimer = false;
		bool m_isTracking = false;
		bool m_isTrackingCamera = false;

		virtual void InitializeVertices();
		Vec3 RotatePosition(Vec3 position);

	public:
		Tube(const std::shared_ptr<Stage>& stage) :
			Tube(stage, 36)
		{
		}
		Tube(const std::shared_ptr<Stage>& stage, int angles) :
			Tube(stage,L"",angles)
		{
		}
		Tube(const std::shared_ptr<Stage>& stage, const std::wstring& textureKeyName, int angles = 36) :
			GameObject(stage),
			m_height(30.0f),
			m_topRadius(0.0f),
			m_bottomRadius(20.0f),
			m_VerticesSize(angles),
			m_loops(50.0f, 50.0f),
			m_topColor(0.0f, 0.3f, 1.0f, 1.0f),
			m_bottomColor(0.0f, 0.3f, 1.0f, 0.0f),
			m_textureKeyName(textureKeyName),
			m_scrollVelocity({ 1.0f,-10.0f }),
			m_timer(0.0f), m_diffTracking(0.0f)
		{
		}

		void OnCreate() override;
		void OnUpdate() override;


		void SetScrollPerSecond(const Vec2& velocity)
		{
			m_scrollVelocity = velocity;
			m_scrollVelocity.y *= -1.0f;
		}

		void SetScrollPerSecond(float u, float v)
		{
			SetScrollPerSecond({ u,v });
		}

		void SetHeight(float height)
		{
			m_isUpdate = true;
			m_height = height;
		}

		void SetRadius(float radius)
		{
			m_isUpdate = true;
			m_bottomRadius = m_topRadius = radius;
		}

		void SetTopRadius(float radius)
		{
			m_isUpdate = true;
			m_topRadius = radius;
		}

		void SetBottomRadius(float radius)
		{
			m_isUpdate = true;
			m_bottomRadius = radius;
		}

		void SetColor(const Col4& color)
		{
			m_isUpdate = true;
			m_bottomColor = m_topColor = color;
		}

		void SetColor(float red, float green, float blue, float alpha = 1.0f)
		{
			SetColor(Col4(red, green, blue, alpha));
		}

		void SetTopColor(const Col4& color)
		{
			m_isUpdate = true;
			m_topColor = color;
		}

		void SetTopColor(float red, float green, float blue, float alpha = 1.0f)
		{
			SetTopColor(Col4(red, green, blue, alpha));
		}

		void SetBottomColor(const Col4& color)
		{
			m_isUpdate = true;
			m_bottomColor = color;
		}

		void SetBottomColor(float red, float green, float blue, float alpha = 1.0f)
		{
			SetBottomColor(Col4(red, green, blue, alpha));
		}
		void SetAxis(const Vec3 axis)
		{
			m_axis.push_back(axis);
		}
		void SetAngle(float angle)
		{
			m_spinAngle.push_back(angle);
		}
		void SetAngleAndAxis(const Vec3 axis, float angle)
		{
			m_axis.push_back(axis);
			m_spinAngle.push_back(angle);
		}
		void SetDestroyTimer(float time)
		{
			m_timer = time;
			m_isTimer = true;
		}
		void SetTrackingCamera(bool flag)
		{
			m_isTrackingCamera = flag;
		}
		void SetTrackingDiff(Vec3& diff)
		{
			m_diffTracking = diff;
		}
		void SetTracking(const shared_ptr<GameObject>& obj)
		{
			m_obj = obj;
			m_isTracking = true;
		}
		void SetLoops(const Vec2& uv)
		{
			m_isUpdate = true;
			m_loops = uv;
		}

		void SetBlendState(BlendState state)
		{
			m_drawComp->SetBlendState(state);
		}
	};

}