#include "stdafx.h"
#include "Project.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	void SelectStage::CreateViewLight() {
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

	Vec2 SelectStage::GetInputState() const {
		Vec2 ret;
		//コントローラの取得
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		ret.x = 0.0f;
		ret.y = 0.0f;
		WORD wButtons = 0;
		if (cntlVec[0].bConnected) {
			ret.x = cntlVec[0].fThumbLX;
			ret.y = cntlVec[0].fThumbLY;
		}

		//キーボードの取得(キーボード優先)
		auto KeyState = App::GetApp()->GetInputDevice().GetKeyState();
		if (KeyState.m_bPushKeyTbl['W']) { ret.y = 1.0f; }
		if (KeyState.m_bPushKeyTbl['S']) { ret.y = -1.0f; }
		if (KeyState.m_bPushKeyTbl['A']) { ret.x = -1.0f; }
		if (KeyState.m_bPushKeyTbl['D']) { ret.x = 1.0f; }

		return ret;
	}

	Vec3 SelectStage::GetMoveVector(float& rot) const {

		Vec3 angle(0, 0, 0);
		//入力の取得
		float moveX = GetInputState().x;
		float moveZ = GetInputState().y;
		if (moveX != 0 || moveZ != 0) {
			float movemove = atan2f(-moveZ, moveX);
			float fRotate = movemove - XM_PIDIV2;

			angle = Vec3(cos(fRotate), 0.0f, -sin(fRotate));

			float rotate = fRotate + XM_PIDIV2;
			angle.normalize();
			rot = rotate;
		}
		return angle;
	}

	void SelectStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		app->RegisterTexture(L"TITLESPRITE", uiPath + L"Title.png");
		app->RegisterTexture(L"STRATA", uiPath + L"StartA.png");
		app->RegisterTexture(L"POSE_TITLE", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"POSE_SELECT", uiPath + L"ResultNextStage.png");
		app->RegisterTexture(L"POSE_START", uiPath + L"BackGame.png");
		app->RegisterTexture(L"POSE_SOUND", uiPath + L"Sound_Menu.png");

		//app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
	}

	void SelectStage::CreateTitle() {
		auto titleSprite = AddGameObject<Sprite>(L"TITLESPRITE", Vec3(0.0f, 100.0f, 0.0f), Vec2(700.0f, 600.0f), true);
		//auto stratASprite = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		//auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		////点滅設定
		//stratASprite->AddComponent<SpriteFlash>(0.8f);
		//m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		//m_Fade->FadeOut();
		//m_Fade->Stop();
	}

	void SelectStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			//OnUpdate();
			CreateResource();
			CreateTitle();
			Which = false;
		}
		catch (...) {
			throw;
		}
	}


	void SelectStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<SelectStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (cntlVec.bConnected) {
			if (cntlVec.wPressedButtons & XINPUT_GAMEPAD_A) {
				OnPushA();
			}
		}
		//if (m_Fade->IsFinish())
		//{
		//	PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		//}

		float rot;
		auto angle = GetMoveVector(rot);
		if (m_TotalTimer.UpdateTimer() && angle.length() > 0.0f) {
			count = (count + 1) % 4; // 0,1,2,3の範囲内ループ
			m_TotalTimer.Reset();
		}
		RemoveGameObject<Sprite>(mConut);

		switch (count) {
		case 0:
			mConut = AddGameObject<Sprite>(L"POSE_TITLE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			break;
		case 1:
			mConut = AddGameObject<Sprite>(L"POSE_SELECT", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			break;
		case 2:
			mConut = AddGameObject<Sprite>(L"POSE_START", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			break;
		case 3:
			mConut = AddGameObject<Sprite>(L"POSE_SOUND", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
			break;

		}

	}

	void SelectStage::OnPushA() {
		//ボタンを押されたらtrue
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		//m_Fade->Play();
		if (count == 0) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToTitleStage");
		}
		if (count == 1) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageM");
		}
		if (count == 2) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		}
		if (count == 3) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		}

	}

}
