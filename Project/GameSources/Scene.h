/*!
@file Scene.h
@brief シーン
*/
#pragma once

#include "stdafx.h"

namespace basecross{
	struct StageData {
		int stageNum;
		int level;
	};
	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	class Scene : public SceneBase{
		vector<vector<wstring>> m_StageFile;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		Scene() :SceneBase(){
			m_StageFile = {
				{L"testStage_Y_Easy.csv",L"testStage_Y_normal.csv",L"testStage_Y_Hard.csv"},
				{L"testStage_Y_Easy.csv",L"testStage_Y_normal.csv",L"testStage_Y_Hard.csv"},
				{L"testStage_Y_Easy.csv",L"testStage_Y_normal.csv",L"testStage_Y_Hard.csv"}
			};
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Scene();
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief イベント取得
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnEvent(const shared_ptr<Event>& event) override;

		int m_MaxCount;
		int GetMaxCount() { return m_MaxCount; }
		int m_Count;
		void ChangeCountStage(int count);
		void CreateModelResource();
		int GetCount() { return m_Count; }
		void SetCount(int count) { m_Count = count; }
	};

}

//end basecross
