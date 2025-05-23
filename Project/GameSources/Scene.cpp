
/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳滉ｽ
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Scene::CreateModelResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring modelPath = mediaPath + L"Models/";

		//モデル関係
		auto modelBuild = MeshResource::CreateStaticModelMesh(modelPath, L"kari.bmf");
		auto modelEnemy = MeshResource::CreateStaticModelMesh(modelPath, L"testtetet.bmf");
		auto modelMesh = MeshResource::CreateBoneModelMesh(modelPath, L"Player.bmf");
		app->RegisterResource(L"PLAYER", modelMesh);
		modelMesh = MeshResource::CreateBoneModelMesh(modelPath, L"Boss.bmf");
		app->RegisterResource(L"BOSS", modelMesh);
		app->RegisterResource(L"OBJECT", modelBuild);
		app->RegisterResource(L"MOB", modelEnemy);
	}
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	void Scene::OnCreate() {
		try {
			Col4 Col;
			Col.set(31.0f / 255.0f, 30.0f / 255.0f, 71.0f / 255.0f, 255.0f / 255.0f);
			SetClearColor(Col);

			//自分自身にイベントを送る
			//これにより各ステージやオブジェクトがCreate時にシーンにアクセスできる

			CreateModelResource();
			SoundManager::Instance().RegisterSounds();
      
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");
			m_MaxCount = 3;
    }
		catch (...) {
			throw;
		}
	}

	Scene::~Scene() {
	}

	void Scene::ChangeCountStage(int count) {
		switch (count) {
		case 0:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");
			break;
		case 1:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageM");
			break;
		case 2:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageKamata");
			break;
		case 3:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageSatou");
			break;
		}
		SetCount(count);
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"ToTitleStage") {
			ResetActiveStage<TitleStage>();
		}
		else if (event->m_MsgStr == L"ToSelectStage") {
			ResetActiveStage<SelectStage>();
		}
		else if (event->m_MsgStr == L"ToGameStage") {
			//次のアクティブステージの設定
			ResetActiveStage<GameStage>(L"level.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageM") {
			ResetActiveStage<GameStageM>(L"testStage01.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageKamata") {
			ResetActiveStage<GameStageK>(L"TestKamataMap.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageSatou") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStageS>(L"TestMapSatou.csv");
		}

	}

}
//end basecross
