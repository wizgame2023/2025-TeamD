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
	void TitleStage::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
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

	void TitleStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		app->RegisterTexture(L"TITLESPRITE", uiPath + L"Title.png");

	}

	void TitleStage::CreateTitle() {
		auto m_TitleSprite = AddGameObject<Sprite>(L"TITLESPRITE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1028.0f, 800.0f), Vec2(1.0f, 1.0f), true);
	}

	void TitleStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			OnUpdate();
			CreateResource();
			CreateTitle();
			Which = false;
		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<TitleStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (cntlVec.bConnected) {
			if (cntlVec.wPressedButtons & XINPUT_GAMEPAD_A) {
				OnPushA();
			}
		}

	}

	void TitleStage::OnPushA() {
		//ボタンを押されたらtrue
		PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
	}

}
