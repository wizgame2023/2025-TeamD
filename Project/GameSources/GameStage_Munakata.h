/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Sprite;
	class NumberSprite;
	class SharpFan;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStageM : public GameStage {

		shared_ptr<NumberSprite> m_Fps;
		int m_MaxEnemyCount;
		int m_EnemyCount;

		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void RegisterObjects();
		bool m_IsPose;

		float GetClearRate() {
			return 100.0f - (static_cast<float>(m_EnemyCount) / static_cast<float>(m_MaxEnemyCount)) * 100.0f;
		}
	public:
		//構築と破棄
		GameStageM(const wstring& file) :GameStage(file), m_IsPose(false) {}
		virtual ~GameStageM() {}
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

