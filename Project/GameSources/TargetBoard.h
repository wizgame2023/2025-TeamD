/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
namespace basecross {
	class Player;
	class Board;
	class TargetBoard : public GameObject {
		shared_ptr<Player> m_Player;
		shared_ptr<GameObject> m_Target;
		shared_ptr<Board> m_Board;

	public:
		TargetBoard(const shared_ptr<Stage>& stage,shared_ptr<Player>& player) : GameObject(stage),m_Player(player){}
		virtual ‾TargetBoard() {}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetTarget(const shared_ptr<GameObject>& target) {
			m_Target = target;
		}

	};
}

//end basecross
