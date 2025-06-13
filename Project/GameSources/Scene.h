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
	struct StageFileData {
		wstring m_FileName;
		bool m_IsClear;
	};
	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	class Scene : public SceneBase{
		vector<vector<StageFileData>> m_StageFile;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		Scene() :SceneBase(){
			m_StageFile = {
				{{L"Debugstage.csv",false},{L"testStage_Y_normal.csv",false},{L"testStage_Y_Hard.csv",false}},
				{{L"testStage_Y_Easy.csv",false},{L"testStage_Y_normal.csv",false},{L"testStage_Y_Hard.csv",false}},
				{{L"testStage_Y_Easy.csv",false},{L"testStage_Y_normal.csv",false},{L"testStage_Y_Hard.csv",false}}
			};
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ‾Scene();
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

		wstring GetFileName(StageData data) {
			return m_StageFile[data.stageNum][data.level].m_FileName;
		}
		bool IsClear(StageData data) {
			return m_StageFile[data.stageNum][data.level].m_IsClear;
		}

		void Clear(StageData data) {
			m_StageFile[data.stageNum][data.level].m_IsClear = true;
		}

	};

}

//end basecross
