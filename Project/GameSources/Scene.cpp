
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
		auto modelBuild = MeshResource::CreateStaticModelMesh(modelPath, L"build.bmf");
		app->RegisterTexture(L"BUILD_TEX", modelPath + L"T_Building.png");
		app->RegisterResource(L"OBJECT", modelBuild);

		modelBuild = MeshResource::CreateStaticModelMesh(modelPath, L"Break_Build.bmf");
		app->RegisterTexture(L"BUILD_BREAK_TEX", modelPath + L"Build.png");
		app->RegisterResource(L"OBJECT_BREAK", modelBuild);

		auto modelEnemy = MeshResource::CreateStaticModelMesh(modelPath, L"testtetet.bmf");
		auto modelMesh = MeshResource::CreateBoneModelMesh(modelPath, L"Player.bmf");
		app->RegisterResource(L"PLAYER", modelMesh);
		auto mobMesh = MeshResource::CreateBoneModelMesh(modelPath, L"Enemy.bmf");
		app->RegisterResource(L"MOB", mobMesh);
		modelMesh = MeshResource::CreateBoneModelMesh(modelPath, L"Boss.bmf");
		app->RegisterResource(L"BOSS", modelMesh);
		auto bulletModelMesh = MeshResource::CreateStaticModelMesh(modelPath, L"Tama.bmf");
		app->RegisterResource(L"BULLET", bulletModelMesh);

		auto rocketModel = MeshResource::CreateStaticModelMesh(modelPath, L"Rocket.bmf");
		app->RegisterResource(L"ROCKET", rocketModel);

		auto titlemodel = MeshResource::CreateBoneModelMesh(modelPath, L"Title_break 1.bmf");
		app->RegisterResource(L"TITLEBREAK", titlemodel);
		app->RegisterTexture(L"BACKTIRLEBRAKE", modelPath + L"Title_break.png");

		//app->RegisterResource(L"MOB", modelEnemy);
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

			//App::GetApp()->GetStepTimer().SetFixedTimeStep(true);
			//App::GetApp()->GetStepTimer().SetTargetElapsedSeconds(1.0 / 60.0);
    }
		catch (...) {
			throw;
		}
	}
	void Scene::OnUpdate() {
		RayCast::InitRay(10);
		SceneBase::OnUpdate();
	}

	void Scene::OnUpdate() {
		SceneBase::OnUpdate();
		RayCast::InitRay(10);
	}

	Scene::‾Scene() {
	}

	void Scene::ChangeCountStage(int count) {
		switch (count) {
		case 0:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageM");
			break;
		case 1:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageKamata");
			break;
		case 2:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStageSatou");
			break;
		case 3:
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");
			break;

		}
		SetCount(count);
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
				SoundManager::Instance().StopAll();
		if (event->m_MsgStr == L"ToTitleStage") {
			ResetActiveStage<TitleStage>();
		}
		else if (event->m_MsgStr == L"ToSelectStage") {
			ResetActiveStage<SelectStage>();
		}
		else if (event->m_MsgStr == L"ToTutorialGameStage") {
			ResetActiveStage<TutorialStage>(L"testStage_Y_Easy.csv");
		}
		else if (event->m_MsgStr == L"ToGameStage") {
			auto count = static_pointer_cast<StageData>(event->m_Info).get();
			if (count->stageNum >= m_StageFile.size() && count->level >= m_StageFile[count->stageNum].size()) {
				ResetActiveStage<TitleStage>();
				return;
			}
			else if (count->stageNum >= m_StageFile.size()) {
				count->stageNum = 0;
				count->level += 1;
				ResetActiveStage<GameStage>(GetFileName(*count), *count);
			}
			//次のアクティブステージの設定
			ResetActiveStage<GameStage>(GetFileName(*count),*count);
		}

	}

}
//end basecross
