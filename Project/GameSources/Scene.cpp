
/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	void Scene::OnCreate() {
		try {
			Col4 Col;
			Col.set(31.0f / 255.0f, 30.0f / 255.0f, 71.0f / 255.0f, 255.0f / 255.0f);
			SetClearColor(Col);

			//自分自身にイベントを送る
			//これにより各ステージやオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");

			SoundManager::Instance().RegisterSounds();
		}
		catch (...) {
			throw;
		}
	}

	Scene::~Scene() {
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"ToTitleStage") {
			ResetActiveStage<TitleStage>();
		}
		else if (event->m_MsgStr == L"ToGameStage") {
			//次のアクティブステージの設定
			ResetActiveStage<GameStage>();
    }
		else if (event->m_MsgStr == L"ToGameStageM") {
			ResetActiveStage<GameStageM>();
		}
		else if (event->m_MsgStr == L"ToGameStageKamata") {
			ResetActiveStage<GameStageK>();
		}
		else if (event->m_MsgStr == L"ToGameStageSatou") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStageS>();
		}

	}

}
//end basecross
