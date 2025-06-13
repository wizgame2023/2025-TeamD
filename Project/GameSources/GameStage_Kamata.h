/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class EffectManeger;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStageK : public GameStage {
		//ビューの作成
		void CreateViewLight();
		void CreateResource();
		void CreatePose();
		void CreateSoundTest();
		void CreateBossEnemy();
		void RegisterObjects();
		void SetAllGameObjectActive(bool flag);
		bool m_IsPose;

		shared_ptr<NumberSprite> m_ProtoHpNumber;
		shared_ptr<EffectManeger> m_Effect;
	public:
		//構築と破棄
		GameStageK(const wstring& file) : GameStage(file, {}), m_IsPose(false) {}
		virtual ‾GameStageK() {}
		//初期化
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;

		void ClosePose();
		void OpenPose();
	};
}
//end basecross

