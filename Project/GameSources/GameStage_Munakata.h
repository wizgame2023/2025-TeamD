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
		shared_ptr<Sprite> m_PlayerHpBar;
		shared_ptr<Sprite> m_PlayerHpBarBackGround;
		shared_ptr<Sprite> m_PlayerEnergyBar;
		shared_ptr<Sprite> m_PlayerEnergyBarBackGround;

		shared_ptr<NumberSprite> m_Fps;
		int m_MaxEnemyCount;
		int m_EnemyCount;

		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreatePlayer();
		void CreateEnemy();
		void RegisterObjects();
		void SetAllGameObjectActive(bool flag);
		bool m_IsPose;

		shared_ptr<NumberSprite> m_ProtoHpNumber;

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

