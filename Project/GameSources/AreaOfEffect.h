#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {

	class AreaOfEffect : public Object {
		int m_AreaVertexCount;
		float m_AreaSize;
		Timer m_ExitTime;

		shared_ptr<SharpFan> m_AreaEffect;
		shared_ptr<SharpFan> m_AreaSizeEffect;
	public:
		AreaOfEffect(const shared_ptr<Stage>& stage,Vec3 position, float size,int vertexCount,float time);
		virtual ~AreaOfEffect(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}