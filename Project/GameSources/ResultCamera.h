/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Player;
	class ResultCamera : public Camera {
		Vec3 m_Direction;
		Vec3 m_Eye;
		Vec3 m_At;

		shared_ptr<Player> m_Player;
	public:
		ResultCamera(Vec3 eye,Vec3 at,shared_ptr<Player>& player) : Camera(),m_Eye(eye),m_At(at),m_Player(player){}
		virtual ~ResultCamera() {}
		virtual void OnCreate()override;
		virtual void OnUpdate();
	};


}