/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class NumberSprite;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
		bool m_IsPose;
		int m_MaxEnemyCount;
		int m_EnemyCount;
		shared_ptr<NumberSprite> m_ProtoHpNumber;
		shared_ptr<NumberSprite> m_ProtoScoreNumber;

		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void RegisterObjects();
		void CreateBossEnemy();

		void CreatePose();
		void CreateSoundTest();

		void ClosePose();
		void OpenPose();
		void SetAllGameObjectActive(bool flag);
		float GetClearRate() {
			return 100.0f - (static_cast<float>(m_EnemyCount) / static_cast<float>(m_MaxEnemyCount)) * 100.0f;
		}

		
	public:
		//構築と破棄
		GameStage() :Stage(),m_IsPose(false) {}
		virtual ~GameStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDestroy()override;
		void SetMaxEnemyCount(int count) {
			m_MaxEnemyCount = count;
			m_EnemyCount = count;
		}
		void EliminateEnemy() {
			m_EnemyCount--;
		}
	};


}
//end basecross

