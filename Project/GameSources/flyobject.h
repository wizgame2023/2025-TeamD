
#pragma once
#include "stdafx.h"
#include "StageBuilder.h"

namespace basecross {
	class flyobject : public Object
	{
		Vec3 m_Position;
		void flyPosison();
	public:
		flyobject(const shared_ptr<Stage>& stage ,Vec3 position);
		~flyobject();
		virtual void OnCreate() override;
		void OnCollisionEnter(shared_ptr<GameObject>& other) override;

	};

}