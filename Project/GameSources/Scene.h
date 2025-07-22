/*!
@file Scene.h
@brief シーン
*/
#pragma once

#include "stdafx.h"
#include "ScoreManager.h"
namespace basecross {
	struct StageData {
		int stageNum;
		int level;
	};
	struct StageFileData {
		wstring m_FileName;
		bool m_IsClear;
	};
	struct ScoreData {
		ScoreBorder<float> m_TimeBorder;
		ScoreBorder<int> m_ParryBorder;
	};
	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	class Scene : public SceneBase {
		vector<vector<StageFileData>> m_StageFile;
		vector<vector<ScoreData>> m_Borders;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief コンストラクタ
		*/
		//--------------------------------------------------------------------------------------
		Scene() :SceneBase(), m_MaxCount(0), m_Count(0) {
			m_StageFile = {
				{{L"testStage_Y_Easy.csv",false},{L"testStage_Y_normal.csv",false},{L"testStage_Y_Hard.csv",false}},
				{{L"TestMap_Easy.csv",false},{L"TestMap_Normal.csv",false},{L"TestMap_Hard.csv",false}},
				{{L"Debugstage.csv"/*L"Map_S_Easy.csv"*/,false},{L"Map_S_Normal.csv",false},{L"Map_S_Hard.csv",false}}
			};

			m_Borders = {
//						時間				  パリイ
				{
					{ScoreBorder<float>({60.0f,90.0f,120.0f,150.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({5,4,3,2})},
					{ScoreBorder<float>({150.0f,180.0f,210.0f,240.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({15,12,9,6})},
					{ScoreBorder<float>({200.0f,230.0f,260.0f,290.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({17,14,10,7})}
				},

				{
					{ScoreBorder<float>({90.0f,120.0f,150.0f,180.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({5,4,3,2})},
					{ScoreBorder<float>({150.0f,180.0f,210.0f,240.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({15,12,9,6})},
					{ScoreBorder<float>({230.0f,260.0f,290.0f,320.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({17,14,10,7})},
				},
				{
					{ScoreBorder<float>({100.0f,130.0f,160.0f,190.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({5,4,3,2})},
					{ScoreBorder<float>({170.0f,200.0f,230.0f,260.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({15,12,9,6})},
					{ScoreBorder<float>({240.0f,270.0f,300.0f,330.0f}, JudgeMode::UpperOrder),ScoreBorder<int>({17,14,10,7})},
				}
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
		virtual void OnUpdate() override;

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
		void ResetClear();
		void ResetStage();
	};

}

//end basecross
