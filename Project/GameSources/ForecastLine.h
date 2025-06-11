/*!
@file Character.h
@brief 
*/

#pragma once
#include "stdafx.h"
#include "RayCast.h"

namespace basecross {
	class Bullet;
	class LineCube : public GameObject {
	protected:
		Col4 m_Color;
		float m_LineSize;
		Line m_Line;
	public:
		LineCube(shared_ptr<Stage>& stage, float size, Col4 color) : GameObject(stage), m_Color(color), m_Line(Line()), m_LineSize(size) {}
		virtual ~LineCube() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		void Destroy();
		void SetLine(const Line& line) {
			m_Line = line;
		}
		shared_ptr<BcPNTStaticDraw> m_Draw;
		shared_ptr<Transform> m_Transform;
	};
	class BulletLine : public LineCube {
	public:
		BulletLine(shared_ptr<Stage>& stage, float size, Col4 color) : LineCube(stage,size,color){}
		virtual ~BulletLine(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}
//end basecross
