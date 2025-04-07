/*!
@file Legion.cpp
@brief Enemyの管理
*/

#include "stdafx.h"
#include "Project.h"
#include "EnemyManeger.h"

namespace basecross {
	Legion::Legion() :
		m_IntruderAlert(false)
	{
	}
	void Legion::IntoEnemyGruop(const shared_ptr<Enemy>& enemy)
	{
		m_GruopEnemy.push_back(enemy);
	}
	void Legion::ReportAlert(const bool& alert)
	{
		m_IntruderAlert = alert;
	}

	void Legion::ChangeEnemyMove(const shared_ptr<EnemyState>& state)
	{

	}

}
//end basecross