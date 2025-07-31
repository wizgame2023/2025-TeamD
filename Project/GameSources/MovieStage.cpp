/*!
@file TitleScne.cpp
@brief タイトルシーン実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void Movie::CreateViewLight() {
		const Vec3 eye(0.0f, 2.5f, -5.0f);
		const Vec3 at(0.0f, 2.5f, 0.0f);
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

	void Movie::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring effectPath = mediaPath + L"Effekt/";


	}

	void Movie::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			CreateResource();

			Play(L"/movies/PV.mp4");
		}
		catch (...) {
			throw;
		}
	}

	void Movie::OnUpdate() {
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];

		if (cntlVec.bConnected && cntlVec.wPressedButtons) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), app->GetScene<Scene>(), L"ToTitleStage");
		}
	}

	void Movie::Play(const wstring& filename) {
		if (filename == L"") return;

		App::GetApp()->ClearMovie();

		SetMovieFileName(App::GetApp()->GetDataDirWString() + filename);

		MovieStage::Play();
	}

}
