/*!
@file Sprite.cpp
@brief スプライト
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Sprite::OnCreate() {
		for (int y = 0; y < m_cutUV.y; y++) {
			for (int x = 0; x < m_cutUV.x; x++) {
				m_AnimationUV.push_back({
					{(1.0f / m_cutUV.x) * x,(1.0f / m_cutUV.y) * y},
					{(1.0f / m_cutUV.x) * (x + 1),(1.0f / m_cutUV.y) * y},
					{(1.0f / m_cutUV.x) * x,(1.0f / m_cutUV.y) * (y + 1)},
					{(1.0f / m_cutUV.x) * (x + 1),(1.0f / m_cutUV.y) * (y + 1)}
					}
				);
			}
		}
		if (m_UseIndex == -1 || m_UseIndex >= m_AnimationUV.size()) {
			m_UseIndex = static_cast<int>(m_AnimationUV.size()) - 1;
		}
		CreateVertex(m_Size, m_AnimationUV[0]);
		vector<uint16_t> indices = {
			0, 1, 2,
			2, 1, 3
		};


		m_BoneDraw = AddComponent<PCTSpriteDraw>(m_Vertices, indices);
		if (m_TexKey != L"") {
			m_BoneDraw->SetTextureResource(m_TexKey);
		}
		SetAlphaActive(true);
		m_BoneDraw->SetSamplerState(SamplerState::LinearWrap);
		m_BoneDraw->SetDiffuse(Col4(1, 1, 1, 1));

		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(m_Pos);

		m_ScreenSize = Vec2(1280, 800);

		SetDrawLayer(3);
	}
	void Sprite::OnUpdate() {
		if (m_IsAnimation) {
			Animation();
		}
		//情報更新
		m_Pos = m_Transform->GetPosition();
	}
	void Sprite::Animation() {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		m_AnimationTimer += elapsedTime;

		if (m_AnimationTimer > m_AnimationChangeTime) {
			m_Index++;
			if (m_Index >= m_UseIndex || m_Index >= m_AnimationUV.size()) {
				m_Index = 0;
			}
			UpdateUV(m_AnimationUV[m_Index]);

			m_AnimationTimer = 0.0f;
		}
	}
	void Sprite::NewAnimation() {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		m_AnimationTimer += elapsedTime;

		if (m_AnimationTimer > m_CurrentAnimation.m_UpdateInterval) {
			if (m_CurrentAnimation.m_IsReverse) {
				m_CurrentAnimation.m_OrderCount--;
				if (m_CurrentAnimation.m_OrderCount < 0) {
					m_CurrentAnimation.m_OrderCount = m_CurrentAnimation.m_Order.size() - 1;
					if (!m_CurrentAnimation.m_IsLoop) {
						//デフォルトに戻る
						return;
					}
				}
			}
			else {
				m_CurrentAnimation.m_OrderCount++;
				if (m_CurrentAnimation.m_OrderCount >= m_CurrentAnimation.m_Order.size() || m_CurrentAnimation.m_OrderCount >= m_AnimationUV.size()) {
					m_CurrentAnimation.m_OrderCount = 0;
					if (!m_CurrentAnimation.m_IsLoop) {
						//デフォルトに戻る
						return;
					}
				}
			}

			UpdateUV(m_AnimationUV[m_CurrentAnimation.m_OrderCount]);

			m_AnimationTimer = 0.0f;
		}
	}
	void Sprite::CreateVertex(Vec2 size, vector<Vec2> uv) {
		if (m_IsUseCenterSprite) {
			m_Vertices = {
				{Vec3(-size.x / 2.0f, size.y / 2.0f, 0),Col4(1,1,1,1), uv[0]},
				{Vec3(size.x / 2.0f, size.y / 2.0f, 0),Col4(1,1,1,1), uv[1]},
				{Vec3(-size.x / 2.0f, -size.y / 2.0f, 0),Col4(1,1,1,1), uv[2]},
				{Vec3(size.x / 2.0f, -size.y / 2.0f, 0),Col4(1,1,1,1), uv[3]}
			};
		}
		else {
			m_Vertices = {
				{Vec3(0, 0, 0),Col4(1,1,1,1), uv[0]},
				{Vec3(size.x, 0, 0),Col4(1,1,1,1), uv[1]},
				{Vec3(0, -size.y, 0),Col4(1,1,1,1), uv[2]},
				{Vec3(size.x, -size.y, 0),Col4(1,1,1,1), uv[3]}
			};
		}
	}
	void Sprite::UpdateUV(vector<Vec2> uv) {
		if (uv.empty()) return;
		if (m_BoneDraw) {
			for (int i = 0; i < m_Vertices.size(); i++)
			{
				m_Vertices[i].textureCoordinate = uv[i];
			}

			m_BoneDraw->UpdateVertices(m_Vertices);
		}
	}
	void Sprite::CutAnimationUv(Vec2 cut) {
		for (int i = 0; i < cut.y; i++) {
			for (int j = 0; j < cut.x; j++) {
				vector<Vec2> uv = {
					{(1.0f / cut.x) * j,(1.0f / cut.y) * i},
					{(1.0f / cut.x) * (j + 1),(1.0f / cut.y) * i},
					{(1.0f / cut.x) * j,(1.0f / cut.y) * (i + 1)},
					{(1.0f / cut.x) * (j + 1),(1.0f / cut.y) * (i + 1)}
				};

				m_AnimationUV.push_back(uv);
			}
		}
		m_IsAnimation = true;
	}
	void Sprite::UpdateSize(Vec3 size) {
		m_Transform->SetScale(size);
	}
	void Sprite::UpdateSize(Vec2 size) {
		if (m_BoneDraw) {
			m_Size = size;
			if (m_IsAnimation) {
				CreateVertex(m_Size, m_AnimationUV[m_CurrentAnimation.m_OrderCount]);
			}
			else {
				CreateVertex(m_Size, m_AnimationUV[0]);
			}
			m_BoneDraw->UpdateVertices(m_Vertices);
		}
	}

	void Sprite::SetPos(Vec3 pos) {
		m_Transform->SetPosition(pos);
	}
	void Sprite::SetDiffuse(Col4 color) {
		m_BoneDraw->SetDiffuse(color);
	}
	Col4 Sprite::GetDiffuse() {
		return m_BoneDraw->GetDiffuse();
	}
	void Sprite::ScreenCenter(const Vec2 diff) {
		Vec3 newPos = Vec3();
		if (m_IsUseCenterSprite) {

		}
		else {

		}
		m_Transform->SetPosition(newPos);
	}
	void Sprite::ScreenTop(const Vec2 diff) {

	}
	void Sprite::ScreenTopRight(const Vec2 diff) {

	}
	void Sprite::ScreenTopLeft(const Vec2 diff) {

	}
	void Sprite::ScreenBottom(const Vec2 diff) {

	}
	void Sprite::ScreenBottomRight(const Vec2 diff) {

	}
	void Sprite::ScreenBottomLeft(const Vec2 diff) {

	}
	void Sprite::ScreenLeft(const Vec2 diff) {

	}
	void Sprite::ScreenRight(const Vec2 diff) {

	}

	void NumberSprite::OnCreate() {
		int digits = static_cast<int>(pow(10, m_DisplayDigit - 1));
		float sizeX = m_Size.x / m_DisplayDigit;
		m_Numbers.reserve(m_DisplayDigit);
		for (int i = 0; i < m_DisplayDigit; i++) {
			shared_ptr<Sprite> number = GetStage()->AddGameObject<Sprite>(m_TexKey, Vec3(m_Pos.x + i * sizeX, m_Pos.y, m_Pos.z), Vec2(sizeX, m_Size.y));//ObjectFactory::Create<Sprite>(GetStage(), m_TexKey, Vec3(0,0,0)/*m_Pos + i * sizeX*/, Vec2(sizeX, m_Size.y));
			int singleDigit = m_DisplayNumber / digits % 10;

			number->UpdateUV(GetUV(singleDigit));
			digits /= 10;
			m_Numbers.push_back(number);
		}
		auto trans = GetComponent<Transform>();
		trans->SetPosition(0, 0, 0);
	}

	void NumberSprite::OnUpdate() {
		for (auto& number : m_Numbers) {
			number->SetDrawActive(GetDrawActive());
		}
	}

	vector<Vec2> NumberSprite::GetUV(int displayDigit) {
		float uvX = 1.0f / m_CutNum;

		vector<Vec2> uv = {
				{uvX * displayDigit,0.0f},
				{uvX * (displayDigit + 1),0.0f},
				{uvX * displayDigit,1.0f},
				{uvX * (displayDigit + 1),1.0f}
		};

		return uv;
	}

	void NumberSprite::UpdateNumber(int number) {
		m_DisplayNumber = number;
		int digits = static_cast<int>(pow(10, m_DisplayDigit - 1));
		for (auto& sprite : m_Numbers) {
			int singleDigit = m_DisplayNumber / digits % 10;

			sprite->UpdateUV(GetUV(singleDigit));
			digits /= 10;
		}
	}


	void SpriteAction::OnCreate() {
		m_BoneDraw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_Trans = GetGameObject()->GetComponent<Transform>();
	}

	void SpriteFlash::OnUpdate() {
		if (m_BoneDraw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			Col4 color = m_BoneDraw->GetDiffuse();
			color.w += m_FlashSpeed * elapsed;
			if (color.w < 0 || color.w > 1) {
				m_FlashSpeed *= -1;
				if (color.w < 0) {
					color.w = 0;
				}
				if (color.w > 1) {
					color.w = 1;
				}
			}
			m_BoneDraw->SetDiffuse(color);
		}
	}
	void SpriteScaling::OnCreate() {
		SpriteAction::OnCreate();

		defaultSize = m_Trans->GetScale();
	}
	void SpriteScaling::OnUpdate() {
		if (m_Trans != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			m_Ratio += m_ScalingSpeed * elapsed;
			if (m_Ratio < m_MinRatio || m_Ratio > m_MaxRatio) {
				m_ScalingSpeed *= -1;
			}
			m_Trans->SetScale(defaultSize * m_Ratio);
		}
	}

	void SpriteFade::OnUpdate() {
		if (m_BoneDraw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			if (!m_IsFadeOut) {
				elapsed *= -1;
			}
			Col4 color = m_BoneDraw->GetDiffuse();
			if (!IsFinish()) {
				color.w += elapsed * m_FadeSpeed;
			}
			if (color.w < 0 || color.w > 1)
			{
				m_IsFinished = true;
			}
			else {
				m_IsFinished = false;
			}
			m_BoneDraw->SetDiffuse(color);
		}
	}
	void SpriteButton::OnCreate() {
		m_SpriteDraw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_UnSelectColor = m_SpriteDraw->GetDiffuse();
		ButtonManager::instance->Register(m_BelongGroup, GetThis<SpriteButton>());
	}
	void SpriteButton::OnUpdate() {

		if (m_AddFunction != nullptr) {
			m_AddFunction(GetThis<SpriteButton>());
		}
		if (m_IsSelect) {
			if (m_SelectedTexture != L"") {
				m_SpriteDraw->SetTextureResource(m_SelectedTexture);
			}
			if (m_SelectedColor != Col4(0, 0, 0, 0)) {
				m_SpriteDraw->SetDiffuse(m_SelectedColor);
			}
		}
		else {
			m_SpriteDraw->SetTextureResource(m_UnSelectTexture);
			m_SpriteDraw->SetDiffuse(m_UnSelectColor);
		}
	}


	shared_ptr<ButtonManager> ButtonManager::instance = nullptr;

	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size,nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size,nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size,object, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size,object, func);
	}

	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		auto sprite = stage->AddGameObject<Sprite>(defaultTex, pos, size, true);
		sprite->AddTag(L"Button");
		shared_ptr<SpriteButton> button = nullptr;
		if (selectedTex != L"" && button == nullptr) {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedTex);
		}
		if (selectedColor != Col4() && button == nullptr) {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedColor);
		}

		button->SetFunction(func,object);
		return sprite;
	}
	void ButtonManager::OnCreate() {
		AddTag(L"Manager");
		instance = GetThis<ButtonManager>();
	}
	void ButtonManager::OnUpdate() {
		if (!m_IsActive) return;
		if (!ExistOpenGroup()) {
			m_IsActive = false;
			return;
		}
		if (m_SelectIndexes.size() == 0 && m_ButtonGroup.size() == 0) return;
		if (!m_ButtonGroup[m_UsingGroup][m_SelectIndexes[m_UsingGroup]]->GetActive()) {
			if (m_SelectIndexes[m_UsingGroup] > 0) {
				m_SelectIndexes[m_UsingGroup]--;
			}
			else {
				m_SelectIndexes[m_UsingGroup]++;
			}
		}
		auto& inputState = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (m_InputDates.find(m_UsingGroup) != end(m_InputDates)) {
			for (auto& inputData : m_InputDates[m_UsingGroup]) {
				bool checkInput = false;
				if (inputData.m_Mode == InputMode::Button) {
					checkInput = inputData.CheckInput(inputState.wPressedButtons);
				}
				else if (inputData.m_Mode == InputMode::Stick) {
					float inputThumb = 0.0f;
					switch (inputData.m_StickMode) {
					case StickMode::LX:
						inputThumb = inputState.fThumbLX;
						break;
					case StickMode::LY:
						inputThumb = -inputState.fThumbLY;
						break;
					case StickMode::RX:
						inputThumb = inputState.fThumbRX;
						break;
					case StickMode::RY:
						inputThumb = -inputState.fThumbRY;
						break;
					}
					checkInput = inputData.CheckInput(inputThumb);
				}
				if (checkInput) {
					if (abs(inputData.m_MoveAmount) > 1) {
						if (!CheckOverIndex(inputData.m_MoveAmount)) continue;
					}
					int checkButton = m_SelectIndexes[m_UsingGroup] + inputData.m_MoveAmount;
					checkButton = min(m_ButtonGroup[m_UsingGroup].size() - 1, checkButton);
					checkButton = max(0, checkButton);
					if (m_ButtonGroup[m_UsingGroup][checkButton]->GetActive()) {
						m_SelectIndexes[m_UsingGroup] += inputData.m_MoveAmount;
					}
				}
			}
		}
		LimitIndex();

		for (int i = 0; i < m_ButtonGroup[m_UsingGroup].size(); i++) {
			if (i == m_SelectIndexes[m_UsingGroup]) {
				m_ButtonGroup[m_UsingGroup][i]->Select();
			}
			else {
				m_ButtonGroup[m_UsingGroup][i]->UnSelect();
			}
		}
		if (m_AcceptButtons.find(m_UsingGroup) != end(m_AcceptButtons)) {
			m_PressedAccept[m_UsingGroup] = 0;
			for (auto& acceptButton : m_AcceptButtons[m_UsingGroup]) {
				if (inputState.wPressedButtons & acceptButton) {
					if (m_ClickSound != L"") {
						SoundManager::Instance().PlaySE(m_ClickSound);
					}
					m_PressedAccept[m_UsingGroup] = acceptButton;
					m_ButtonGroup[m_UsingGroup][m_SelectIndexes[m_UsingGroup]]->Func();
				}
			}
		}
		for (auto& groupMovementAmount : m_GroupMovementAmount) {
			Vec3 movementAmount = groupMovementAmount.second;
			if (movementAmount.length() != 0) {
				movementAmount = movementAmount.normalize();
				movementAmount *= 30.0f;//移動速度
				if (groupMovementAmount.second.length() < movementAmount.length()) {
					movementAmount = groupMovementAmount.second;
				}
				for (auto& button : m_ButtonGroup[groupMovementAmount.first]) {
					auto trans = button->GetGameObject()->GetComponent<Transform>();
					Vec3 pos = trans->GetPosition();
					pos += movementAmount;
					trans->SetPosition(pos);
				}
				groupMovementAmount.second -= movementAmount;
			}
		}

	}

	void ButtonManager::OnDestroy() {
		instance = nullptr;
	}

	void ButtonManager::SetSound(const wstring& sound) {
		m_ClickSound = sound;
	}
	void ButtonManager::SetMoveAmount(const wstring& group, Vec3 target) {
		if (m_GroupMovementAmount.find(group) != end(m_GroupMovementAmount)) {
			if (m_GroupMovementAmount[group].length() == 0) {
				m_GroupMovementAmount[group] = target;
			}
		}
	}
	void Board::OnCreate() {
		m_BoneDraw = AddComponent<PNTStaticDraw>();
		m_BoneDraw->SetOriginalMeshUse(true);
		vector<uint16_t> indices = {};
		MeshUtill::CreateSquare(1.0f, m_Vertices, indices);
		m_BoneDraw->CreateOriginalMesh(m_Vertices, indices);
		if (m_TexKey != L"") {
			m_BoneDraw->SetTextureResource(m_TexKey);
		}
		SetAlphaActive(true);

		m_Trans = GetComponent<Transform>();
		m_Trans->SetPosition(m_StartPos);
		m_Trans->SetScale(m_Size);
	}
	void Board::OnUpdate() {
		if (!m_IsBillBoard) return;
		
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
		
		m_Trans->SetQuaternion(Qt);

	}


}
//end basecross
