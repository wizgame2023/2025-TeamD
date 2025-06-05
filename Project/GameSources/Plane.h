#pragma once
#include "stdafx.h"

namespace basecross {
	class Plane : public GameObject
	{
	public:
		Plane(const std::shared_ptr<Stage>& stage)
			: GameObject(stage)
		{
		}

		void OnCreate() override;
	};
}
