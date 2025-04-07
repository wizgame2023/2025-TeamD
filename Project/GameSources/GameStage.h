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

		wstring m_MapFileName;
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
			if (m_MaxEnemyCount <= 0) return 100.0f;
			return 100.0f - (static_cast<float>(m_EnemyCount) / static_cast<float>(m_MaxEnemyCount)) * 100.0f;
		}
		void GameOver();
		void GameClear();

		shared_ptr<EffectManeger> m_Effect;


	public:
		//構築と破棄
		GameStage(const wstring& file) :Stage(),m_MapFileName(file), m_IsPose(false), m_MaxEnemyCount(0), m_EnemyCount(0) {}
		virtual ~GameStage() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;
		virtual void OnDestroy()override;
		virtual void OnEvent(const shared_ptr<Event>& event)override;
		void SetMaxEnemyCount(int count) {
			m_MaxEnemyCount = count;
			m_EnemyCount = count;
		}
		void EliminateEnemy() {
			m_EnemyCount--;
		}
		int GetDefeatEnemyCount() {
			return m_MaxEnemyCount - m_EnemyCount;
		}
	};


}
//end basecross

