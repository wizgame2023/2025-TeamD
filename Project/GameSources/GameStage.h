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
	class NormalIcon;
	class UltIcon;
	class FollowCamera;
	class HpSprite;
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
		
		enum CameraState
		{
			OPENINGCAMERA,
			FOLLOWCAMERA,
			RESULTCAMERA
		};
		int m_cameraState;
		wstring m_MapFileName;
		bool m_IsPose;
		int m_IsGameStater;
		int m_MaxEnemyCount;
		int m_EnemyCount;
		float m_TotalTime;

		shared_ptr<Menu> m_PauseMenu;
		shared_ptr<Menu> m_SoundTestMenu;
		shared_ptr<Menu> m_ResultMenu;
		shared_ptr<Menu> m_GameOverMenu;

		shared_ptr<NormalIcon> m_NormalIcon;
		shared_ptr<NormalIcon> m_Icon;
		shared_ptr<UltIcon> m_UltIcon;
		shared_ptr<HpSprite> m_PlayerHpBar;
		//shared_ptr<Sprite> m_PlayerHpBarBackGround;

		shared_ptr<HpSprite> m_BossHpBar;
		//shared_ptr<Sprite> m_BossHpBarBackGround;
		shared_ptr<Sprite> m_BossText;


		//ÉrÉÖÅ[ÇÃçÏê¨
		void CreateViewLight();
		void RegisterObjects();

		void CreateGameOverMenu();
		void CreatePose();
		void CreateSoundTest();
		void CreateResult();
		void CreateUI();

		void GameOver();
		void GameOverCamera();
		void BossAppaerCamera();
		void GameClear();
		void ToOpeningCamera();
		shared_ptr<EffectManeger> m_Effect;
		shared_ptr<FollowCamera> m_Camera;

		shared_ptr<SingleView> m_ProductionCameraView; 
		shared_ptr<SingleView> m_MyCameraView;      
	public:
		//ç\ízÇ∆îjä¸
		GameStage(const wstring& file) : Stage(), m_MapFileName(file), m_IsPose(false), m_IsGameStater(false){}
		virtual ~GameStage() {}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;
		virtual void OnDestroy()override;
		void CreateResource();
		virtual void OnEvent(const shared_ptr<Event>& event)override;
		shared_ptr <EffectManeger> GetCreateEffect();
		shared_ptr<FollowCamera> SetCameraPause();
		void SetAllGameObjectActive(bool flag);
		void ToMainCamera();
	};

}
//end basecross

