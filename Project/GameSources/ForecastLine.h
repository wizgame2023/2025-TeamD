/*!
@file Character.h
@brief ƒLƒƒƒ‰ƒNƒ^[‚È‚Ç
*/

#pragma once
#include "stdafx.h"
#include "RayCast.h"
namespace basecross {
	class Bullet;
	class LineCube : public GameObject {
		Col4 m_Color;
		float m_LineSize;
		Line m_Line;
	public:
		LineCube(shared_ptr<Stage>& stage, float size, Col4 color) : GameObject(stage), m_Color(color), m_Line(Line()), m_LineSize(size) {}
		virtual ~LineCube() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void SetLine(const Line& line) {
			m_Line = line;
		}
		shared_ptr<BcPNTStaticDraw> m_Draw;
		shared_ptr<Transform> m_Transform;
	};
}
//end basecross
