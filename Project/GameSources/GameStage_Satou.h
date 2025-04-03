/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStageS : public GameStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreatePlayer();
		void CreateEnemy();
		void CreateBossEnemy();
		void RegisterObjects();
		void SetAllGameObjectActive(bool flag);
		bool m_IsPose;

		int m_MaxEnemyCount;
		int m_EnemyCount;
		shared_ptr<NumberSprite> m_ProtoHpNumber;

		float GetClearRate() {
			return 100.0f - (static_cast<float>(m_EnemyCount) / static_cast<float>(m_MaxEnemyCount)) * 100.0f;
		}

	public:
		//構築と破棄
		GameStageS(const wstring& file) : GameStage(file), m_IsPose(false) {}
		virtual ~GameStageS() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;


		void ClosePose();
		void OpenPose();

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

