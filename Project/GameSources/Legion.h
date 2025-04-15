/*!
@file Enemymaneger.h
@brief EnemyÇÃä«óùÇ»Ç«
*/

#pragma once
#include "stdafx.h"


namespace basecross {
	class Enemy;
	template<typename> class EnemyState;
	class Legion : public Object
	{
		bool m_IntruderAlert;
		int m_LegionNumber;
		vector<weak_ptr<Enemy>> m_GruopEnemy;
	public:
		Legion(const shared_ptr<Stage>& stage) : Object(stage) {}
		~Legion() {};

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetLegionNumber(const int& number)
		{
			m_LegionNumber = number;
		}

		int GetLegionNumber()
		{
			return m_LegionNumber;
		}

		void IntoEnemyGruop(const shared_ptr<Enemy>& enemy)
		{
			m_GruopEnemy.push_back(enemy);
		}

		void ReportAlert(const bool& alert)
		{
			m_IntruderAlert = alert;
		}

		//template<typename T>
		//void ChangeEnemyMove( const shared_ptr<Enemy>& enemy);
	};
}
//end basecross
