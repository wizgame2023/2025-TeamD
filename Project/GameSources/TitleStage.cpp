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

	void TitleStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		wstring texPath = mediaPath + L"Textures/";
		wstring effectPath = mediaPath + L"Effekt/";

		app->RegisterTexture(L"TITLESPRITE", uiPath + L"rogo.png");
		app->RegisterTexture(L"STRATA", uiPath + L"Start_AS.png");

		//igc用UI
		app->RegisterTexture(L"STARTA_KEY", uiPath + L"Start_A_Key.png");

		app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
		app->RegisterTexture(L"BACKGROUND", texPath + L"TitleBackGround.png");
		m_Effect = AddGameObject<EffectManager>();
		m_Effect->SetDrawLayer(4);
		m_Effect->RegisterResource(L"Panchi", effectPath + L"panchi.efk");

	}

	void TitleStage::CreateTitle() {
		m_BackGround = AddGameObject<Sprite>(L"BACKGROUND", Vec3(0.0f, 0.0f, 5.0f), Vec2(1280.0f, 800.0f), Vec2(0.5f));
		m_BackGround->SetDrawLayer(4);
		//auto borad = AddGameObject<Board>(L"BACKGROUND", Vec3(0.0f, 2.5f, 0.0f), Vec3(6.75f, 4.25f,1));
		//borad->SetColor(Col4(0, 0, 0, 1));
		m_Title = AddGameObject<Sprite>(L"TITLESPRITE", Vec3(0.0f, 150.0f, 0.0f), Vec2(1000.0f, 800.0f), Vec2(0.5f));

		m_Title->SetDrawLayer(6);
		m_Start = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -270.0f, 0.0f), Vec2(250.0f, 120.0f), Vec2(0.5f));
		m_Start->SetDrawLayer(6);
		m_StartKey = AddGameObject<Sprite>(L"STARTA_KEY", Vec3(0.0f, -270.0f, 0.0f), Vec2(375.0f, 180.0f), Vec2(0.5f));
		m_StartKey->SetDrawLayer(6);
		auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), Vec2(0.5f));
		fadeSprite->SetDrawLayer(6);
		m_Titlemodel = AddGameObject<TirleStageModel>();
		m_Titlemodel->SetDrawActive(false);
		//点滅設定
		m_Start->AddComponent<SpriteFlash>(1.0f);
		m_StartKey->AddComponent<SpriteFlash>(0.8f);
		m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		m_Fade->FadeOut();
		m_Fade->Stop();
	}

	void TitleStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			//OnUpdate();
			CreateResource();
			CreateTitle();
			m_TotalTime = 0.0f;
			SoundManager::GetInstance().PlayBGM(L"BGM_TITLE");
		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<TitleStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (m_EffectPos != Vec3())
		{
			m_EffectPos = m_EffectPos + Vec3(0, 0, -0.1f);
			m_Effect->SetLocation(m_Handle, m_EffectPos);
			m_Titlemodel->SetFlag(true);
			m_Titlemodel->SetDrawActive(true);
			m_BackGround->SetDrawActive(true);
		}
		else {
			DrawIcon();
			m_TotalTime += app->GetElapsedTime();
			if (m_TotalTime > 30.0f)
			{
				PostEvent(0.0f, GetThis<ObjectInterface>(), app->GetScene<Scene>(), L"ToMoveStage");
			}
		}
		if (m_Titlemodel->GetEndFlag())
		{
			m_Fade->Play();
			m_Effect->SetEffectPause(true);
		}
		if (m_Fade->IsFinish())
		{
			PostEvent(0.0f, GetThis<ObjectInterface>(),app->GetScene<Scene>(), L"ToSelectStage");
		}
	}

	void TitleStage::DrawIcon()
	{
		auto& device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected)
		{
			m_Start->SetDrawActive(true);
			m_StartKey->SetDrawActive(false);
		}
		else
		{
			m_Start->SetDrawActive(false);
			m_StartKey->SetDrawActive(true);
		}
	}

	void TitleStage::OnPushA() {
		if (m_Titlemodel->GetDrawActive()) return;
		auto camera = GetView()->GetTargetCamera();
		auto forward = camera->GetEye() - camera->GetAt();
		float rotate = atan2f(forward.x, forward.z);
		m_EffectPos = Vec3(0.01f, 2.05f, -3.0f);
		m_Effect->PlayEffect(m_Handle, L"Panchi", m_EffectPos, 0.0f);
		m_Effect->SetRotation(m_Handle, Vec3(0, 1, 0), rotate);
		m_Effect->SetEffectSpeed(m_Handle, 0.1f);
		m_Start->SetDrawActive(false);
		m_StartKey->SetDrawActive(false);
		m_Title->SetDrawActive(false);
		SoundManager::GetInstance().PlaySE(L"SE_CRACK", 1.0f);
	}

	void TirleStageModel::OnCreate() {
		Object::OnCreate();

		m_Transform = GetComponent<Transform>();
		SetPosition(Vec3(0));
		SetScale(Vec3(1));
		SetRotation(Vec3(0));


		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(1.0f), //(.1f, .1f, .1f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0, 0.0f),
			Vec3(0.0f, 0.25f, 0.0f)
		);

		ptrDraw->SetMeshResource(L"TITLEBREAK");
		ptrDraw->SetTextureResource(L"BACKTIRLEBRAKE");
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::Additive);

		ptrDraw->SetDepthStencilState(DepthStencilState::Read);
		ptrDraw->AddAnimation(L"TITLEBREAK", 0.0f, 60.0f, false, 120.0f);
		ptrDraw->AddAnimation(L"TITLEFALL", 60.0f, 120.0f, false, 60.0f);
		SetAnim(L"TITLEBREAK");
		SetAlphaActive(true);
		SetDrawLayer(5);
	}

	void TirleStageModel::OnUpdate() {

		Vec3 defUp = Vec3(0, 1, 0);
		auto camera = OnGetDrawCamera();
		Vec3 eye = camera->GetEye();
		Vec3 at = camera->GetAt();

		Vec3 temp = at - eye;
		Vec2 tempVec2(temp.x, temp.z);
		if (length(tempVec2) < 0.1f) {
			defUp = bsm::Vec3(0, 0, 1.0f);
		}
		temp.normalize();
		Mat4x4 rotMatrix = static_cast<Mat4x4>(XMMatrixLookAtLH(Vec3(0, 0, 0), temp, defUp));
		rotMatrix = inverse(rotMatrix);
		Quat Qt = rotMatrix.quatInMatrix();
		Qt.normalize();

		GetComponent<Transform>()->SetQuaternion(Qt);
		auto ptrDraw = GetComponent<BcPNTBoneModelDraw>();
		auto elapsedTime = App::GetApp()->GetElapsedTime();
		if (m_Flag)
		{
			ptrDraw->UpdateAnimation(elapsedTime);
			m_EndFlag = ptrDraw->IsTargetAnimeEnd();
		}
	}
}
