/*!
@file citizen.h
@brief Žs–¯‚È‚Ç
*/

#pragma once
#include "stdafx.h"
namespace basecross {
	class Citizen : public Character
	{
		Vec3 m_Position;
		Vec3 m_Scale;
	public:
		Citizen(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale
		);
		~Citizen();
		virtual void OnCreate();
		virtual void OnUpdate();

		void RunAway();
	};
}

//end basecross
