#include "stdafx.h"
#include "Project.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	�Q�[���X�e�[�W�N���X����
	//--------------------------------------------------------------------------------------
	void SelectStage::CreateViewLight() {
		const Vec3 eye(0.0f, 5.0f, -5.0f);
		const Vec3 at(0.0f);
		auto PtrView = CreateView<SingleView>();
		//�r���[�̃J�����̐ݒ�
		auto PtrCamera = ObjectFactory::Create<Camera>();
		PtrView->SetCamera(PtrCamera);
		PtrCamera->SetEye(eye);
		PtrCamera->SetAt(at);
		//�}���`���C�g�̍쐬
		auto PtrMultiLight = CreateLight<MultiLight>();
		//�f�t�H���g�̃��C�e�B���O���w��
		PtrMultiLight->SetDefaultLighting();
	}


	void SelectStage::CreateResource() {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring uiPath = mediaPath + L"UI/";
		app->RegisterTexture(L"SELECTSPRITE", uiPath + L"SelectStageToTitle.png");
		app->RegisterTexture(L"SELECT_TITLE", uiPath + L"ResultToTitle.png");
		app->RegisterTexture(L"SELECT_STAGE", uiPath + L"SelectStage.png");
		app->RegisterTexture(L"SELECT_NUMBER", uiPath + L"Number.png");

		//app->RegisterTexture(L"FADE", uiPath + L"TitelFade.png");
	}

	void SelectStage::CreateSelect() {
		auto titleSprite = AddGameObject<Sprite>(L"SELECTSPRITE", Vec3(0.0f, 100.0f, 0.0f), Vec2(700.0f, 600.0f), true);
		m_Number = AddGameObject<NumberSprite>(L"SELECT_NUMBER", Vec3(120.0f, -140.0f, 0.0f), Vec2(33, 100), 1);
		m_TitleSprite = AddGameObject<Sprite>(L"SELECT_TITLE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		m_StageSprite = AddGameObject<Sprite>(L"SELECT_STAGE", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		//auto stratASprite = AddGameObject<Sprite>(L"STRATA", Vec3(0.0f, -200.0f, 0.0f), Vec2(300.0f, 200.0f), true);
		//auto fadeSprite = AddGameObject<Sprite>(L"FADE", Vec3(0.0f, 0.0f, 0.0f), Vec2(1480.0f, 880.0f), true);
		////�_�Őݒ�
		//stratASprite->AddComponent<SpriteFlash>(0.8f);
		//m_Fade = fadeSprite->AddComponent<SpriteFade>(1.0f);
		//m_Fade->FadeOut();
		//m_Fade->Stop();
	}

	void SelectStage::OnCreate() {
		try {
			//�r���[�ƃ��C�g�̍쐬
			CreateViewLight();
			//OnUpdate();
			CreateResource();
			CreateSelect();
			SoundManager::Instance().PlayBGM(L"BGM_SELECT");

		}
		catch (...) {
			throw;
		}
	}


	void SelectStage::OnUpdate() {
		m_InputHandler.PushHandle(GetThis<SelectStage>());
		auto& app = App::GetApp();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		//if (m_Fade->IsFinish())
		//{
		//	PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageSatou");
		//}

		float rot;
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX > 0.5f) {
			m_Count = (m_Count + 1) % 4; // 0,1,2,3�͈͓̔����[�v
			m_TotalTimer.Reset();
		}
		if (m_TotalTimer.UpdateTimer() && cntlVec.fThumbLX < -0.5f) {
			m_Count = (m_Count + 3) % 4; // 0,1,2,3�͈͓̔����[�v
			m_TotalTimer.Reset();
		}

		switch (m_Count) {
		case 0:		
			m_Number->UpdateNumber(1);
			m_TitleSprite->SetDrawActive(true);
			m_StageSprite->SetDrawActive(false);
			break;
		case 1:
			m_Number->UpdateNumber(2);
			m_TitleSprite->SetDrawActive(false);
			m_StageSprite->SetDrawActive(true);
			break;
		case 2:
			m_Number->UpdateNumber(3);
			m_TitleSprite->SetDrawActive(false);
			m_StageSprite->SetDrawActive(true);
			break;
		case 3:
			m_Number->UpdateNumber(4);
			m_TitleSprite->SetDrawActive(false);
			m_StageSprite->SetDrawActive(true);
			break;

		}

	}

	void SelectStage::OnPushA() {
		//�{�^���������ꂽ��true
		//PostEvent(0.0f, GetThis<ObjectInterface>(), App::GetApp()->GetScene<Scene>(), L"ToGameStageKamata");
		//m_Fade->Play();
		OnDestroy();
		auto scene = App::GetApp()->GetScene<Scene>();
		scene->ChangeCountStage(m_Count);
		SoundManager::Instance().PlaySE(L"SE_HIT_ENEMY");

	}

}
