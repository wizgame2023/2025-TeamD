
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
		auto playerModel = MeshResource::CreateBoneModelMesh(modelPath, L"Player.bmf");
		auto modelBuild = MeshResource::CreateStaticModelMesh(modelPath, L"kari.bmf");
		auto modelEnemy = MeshResource::CreateStaticModelMesh(modelPath, L"testtetet.bmf");
		auto modelGround = MeshResource::CreateStaticModelMesh(modelPath, L"Ground.bmf");

		app->RegisterResource(L"OBJECT", modelBuild);
		app->RegisterResource(L"MOB", modelEnemy);
		app->RegisterResource(L"PLAYER", playerModel);
		app->RegisterResource(L"GROUND", modelGround);
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

			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageM");
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
			ResetActiveStage<GameStage>(L"level.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageM") {
			ResetActiveStage<GameStageM>(L"level.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageKamata") {
			ResetActiveStage<GameStageK>(L"TestKamataMap.csv");
		}
		else if (event->m_MsgStr == L"ToGameStageSatou") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStageS>(L"levelMap.csv");
		}

	}

}
//end basecross
