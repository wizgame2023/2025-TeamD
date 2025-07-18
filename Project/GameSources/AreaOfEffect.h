#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class GroundEffect;

	enum class TYPE {
		CIRCLE,RECT
	};
	struct CIRCLE {
		float m_Radius;			//半径
		size_t m_VertexCount;	//頂点数
		float m_Angle;			//表示角度
	};
	struct RECTANGLE {
		float left;
		float top;
		float right;
		float bottom;
	};

	class AreaOfEffect : public Object {
		TYPE m_AreaType;
		CIRCLE m_Circle;
		RECTANGLE m_Rect;
		Vec3 m_Forward;

		//int m_AreaVertexCount;
		//float m_AreaSize;
		Timer m_ExitTime;

		shared_ptr<GroundEffect> m_AreaEffect;
		shared_ptr<GroundEffect> m_AreaSizeEffect;
	public:
		//AreaOfEffect(const shared_ptr<Stage>& stage,Vec3 position, float size,int vertexCount,float time);
		AreaOfEffect(const shared_ptr<Stage>& stage,Vec3 position,Vec3 forward, CIRCLE circle,float time);
		AreaOfEffect(const shared_ptr<Stage>& stage,Vec3 position, Vec3 forward, RECTANGLE rect,float time);
		virtual ‾AreaOfEffect(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};

	class GroundEffect : public Object {
		TYPE m_AreaType;
		CIRCLE m_Circle;
		RECTANGLE m_Rect;
		float m_ForwardRad;

		wstring m_TexKey;

		vector<VertexPositionColorTexture> m_Vertices;
		vector<uint16_t> m_Indices;
		shared_ptr<PCTStaticDraw> m_Draw;

		vector<Vec3> GetVertexPositionsCircle();
		vector<Vec3> GetVertexPositionsRect();

		//float a = 1.0f;
	public:
		GroundEffect(const shared_ptr<Stage>& ptr, const wstring& key, RECTANGLE rect) : Object(ptr), m_AreaType(TYPE::RECT), m_TexKey(key),m_ForwardRad(0),m_Circle{}, m_Rect(rect) {}
		GroundEffect(const shared_ptr<Stage>& ptr, const wstring& key, CIRCLE circle) : Object(ptr), m_AreaType(TYPE::CIRCLE), m_TexKey(key), m_ForwardRad(0), m_Circle(circle), m_Rect{} {}
		virtual ‾GroundEffect(){}

		void Init(vector<Vec3>& positions,float rad);
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		CIRCLE GetCircle() {
			return m_Circle;
		}
		void SetCircle(CIRCLE circle) {
			if (circle.m_VertexCount * 2 != m_Indices.size()) return;
			m_Circle = circle;
		}

		RECTANGLE GetRect() {
			return m_Rect;
		}
		void SetRect(RECTANGLE rect) {
			m_Rect = rect;
		}

		void SetForward(Vec3 forward) {
			m_ForwardRad = atan2f(forward.x, forward.z);
		}
	};
	
}