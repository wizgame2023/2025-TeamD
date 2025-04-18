/*!
@file GameStage.h
@brief 
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class NumberSprite;
	class Sprite;
	class Menu;
	class UltIcon;
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {

		wstring m_MapFileName;
		bool m_IsPose;
		int m_MaxEnemyCount;
		int m_EnemyCount;

		shared_ptr<Menu> m_PauseMenu;
		shared_ptr<Menu> m_SoundTestMenu;
		shared_ptr<Menu> m_ResultMenu;

		shared_ptr<UltIcon> m_UltIcon;
		shared_ptr<Sprite> m_PlayerHpBar;
		shared_ptr<Sprite> m_PlayerHpBarBackGround;

		shared_ptr<Sprite> m_BossHpBar;
		shared_ptr<Sprite> m_BossHpBarBackGround;
		shared_ptr<Sprite> m_BossText;

		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void RegisterObjects();
		void CreateBossEnemy();

		void CreatePose();
		void CreateSoundTest();
		void CreateResult();
		void CreateUI();
		void SetAllGameObjectActive(bool flag);
		float GetClearRate() {
			if (m_MaxEnemyCount <= 0) return 100.0f;
			return 100.0f - (static_cast<float>(m_EnemyCount) / static_cast<float>(m_MaxEnemyCount)) * 100.0f;
		}

		void GameOver() {};
		void GameClear() {};

		shared_ptr<EffectManeger> m_Effect;


	public:
		//構築と破棄
		GameStage(const wstring& file) : Stage(), m_MapFileName(file), m_IsPose(false) {}
		virtual ~GameStage() {}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;
		virtual void OnDestroy()override;
		virtual void OnEvent(const shared_ptr<Event>& event)override;
		void ClosePose();
		void OpenPose();
		shared_ptr <EffectManeger> GetCreateEffect();

	};


}
//end basecross

