
/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	繧ｲ繝ｼ繝繧ｷ繝ｼ繝ｳ
	//--------------------------------------------------------------------------------------
	void Scene::OnCreate() {
		try {
			//繧ｯ繝ｪ繧｢縺吶ｋ濶ｲ繧定ｨｭ螳
			Col4 Col;
			Col.set(31.0f / 255.0f, 30.0f / 255.0f, 71.0f / 255.0f, 255.0f / 255.0f);
			SetClearColor(Col);

			//自分自身にイベントを送る
			//これにより各ステージやオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStage");

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
			//譛蛻昴�繧｢繧ｯ繝�ぅ繝悶せ繝��繧ｸ縺ｮ險ｭ螳
			ResetActiveStage<TitleStage>();
		}
		else if (event->m_MsgStr == L"ToGameStage") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStage>();
    }
		else if (event->m_MsgStr == L"ToGameStageM") {
			ResetActiveStage<GameStageM>();
		}
		else if (event->m_MsgStr == L"ToGameStageKamata") {
			//譛蛻昴�繧｢繧ｯ繝�ぅ繝悶せ繝��繧ｸ縺ｮ險ｭ螳
			ResetActiveStage<GameStageK>();
		}
	}

}
//end basecross
