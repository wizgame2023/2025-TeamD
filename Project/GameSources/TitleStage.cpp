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

	void TitleStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			OnUpdate();
			Which = false;
		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<TitleStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (Which) OnPushA();
	}

	void TitleStage::OnPushA() {
		//ボタンを押されたらtrue
		PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStage");
	}

}
