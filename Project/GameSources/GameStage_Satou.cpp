/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void GameStageS::CreateViewLight() {
		const Vec3 eye(0.0f, 30.0f, -30.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		//ビューのカメラの設定
		auto PtrCamera = ObjectFactory::Create<Camera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
	}
	/// <summary>
	/// リソースの作成
	/// </summary>
	void GameStageS::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";


	}
	void GameStageS::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		//builder->Register<Mob>(L"mob");
		builder->Register<RootPointer>(L"pointer");

		builder->LoadCsv();
	}
	void GameStageS::OnCreate() {
		try {
			CreateResource();
			GameStage::OnCreate();

		}
		catch (...) {
			throw;
		}
	}

	void GameStageS::OnUpdate() {
		auto& app = App::GetApp();

		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();

		float elpased = app->GetStepTimer().GetFramesPerSecond();


	}

}
//end basecross
