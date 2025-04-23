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
	void GameStageM::CreateViewLight() {
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
	void GameStageM::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring modelPath = mediaPath + L"Models/";

		
	}
	void GameStageM::RegisterObjects() {
		auto& builder = AddGameObject<StageBuilder>(L"level.csv", 1.0f);
		builder->Register<FixedBox>(L"cube");
		builder->Register<Player>(L"player");
		builder->Register<Mob>(L"mob");
		builder->Register<RootPointer>(L"pointer");

		builder->LoadCsv();
	}
	void GameStageM::OnCreate() {
		try {
			CreateResource();
			GameStage::OnCreate();
			m_Fps = AddGameObject<NumberSprite>(L"NUMBER", Vec3(530.0f, 350.0f, 0.0f), Vec2(109.0f, 96.0f), 3);

			SoundManager::Instance().PlayBGM(L"BGM_GAME_PINCH");
		}
		catch (...) {
			throw;
		}
	}

	void GameStageM::OnUpdate() {
		auto& app = App::GetApp();

		auto& device = app->GetInputDevice().GetControlerVec()[0];
		GameStage::OnUpdate();

		float elpased = app->GetStepTimer().GetFramesPerSecond();
		m_Fps->UpdateNumber(elpased);

		
	}

}
//end basecross
