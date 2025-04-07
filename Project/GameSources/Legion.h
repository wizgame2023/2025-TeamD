/*!
@file Enemymaneger.h
@brief Enemy‚ÌŠÇ—‚È‚Ç
*/

#pragma once
#include "stdafx.h"


namespace basecross {
	class Enemy;
	class Legion
	{
		bool m_IntruderAlert;
		vector<weak_ptr<Enemy>> m_GruopEnemy;
	public:
		Legion();
		~Legion() {};

		void IntoEnemyGruop(const shared_ptr<Enemy>& enemy);
		void ReportAlert(const bool& alert);
		void ChangeEnemyMove(const shared_ptr<EnemyState>& state);
	};
}
//end basecross
