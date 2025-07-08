/*!
@file Sprite.cpp
@brief スプライト
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void Sprite::OnCreate() {
		/*for (int y = 0; y < m_cutUV.y; y++) {
			for (int x = 0; x < m_cutUV.x; x++) {
				m_AnimationUV.push_back({
					{(1.0f / m_cutUV.x) * x,(1.0f / m_cutUV.y) * y},
					{(1.0f / m_cutUV.x) * (x + 1),(1.0f / m_cutUV.y) * y},
					{(1.0f / m_cutUV.x) * x,(1.0f / m_cutUV.y) * (y + 1)},
					{(1.0f / m_cutUV.x) * (x + 1),(1.0f / m_cutUV.y) * (y + 1)}
					}
				);
			}
		}*/
		if (m_UseIndex != -1) {
			m_AnimationUV = CreateAnimationUV(m_cutUV, m_UseIndex);
		}
		else {
			m_AnimationUV = CreateAnimationUV(m_cutUV);
			m_UseIndex = m_AnimationUV.size() - 1;
		}
		//if (m_UseIndex == -1 || m_UseIndex >= m_AnimationUV.size()) {
		//	m_UseIndex = static_cast<int>(m_AnimationUV.size()) - 1;
		//}
		CreateVertex(m_Size, m_AnimationUV[0]);
		vector<uint16_t> indices = {
			0, 1, 2,
			2, 1, 3
		};


		m_Draw = AddComponent<PCTSpriteDraw>(m_Vertices, indices);
		if (m_TexKey != L"") {
			m_Draw->SetTextureResource(m_TexKey);
		}
		SetAlphaActive(true);
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDiffuse(Col4(1, 1, 1, 1));
		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(m_Pos);

		m_ScreenSize = Vec2(1280, 800);

		//SetDrawLayer(3);
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

		if (m_AnimationTimer > m_CurrentAnimation.m_UpdateInterval) {
			if (m_CurrentAnimation.m_IsReverse) {
				m_CurrentAnimation.m_CurrentOrder--;
				if (m_CurrentAnimation.m_CurrentOrder < 0) {
					m_CurrentAnimation.EndAnimation();
					/*if (m_CurrentAnimation.m_IsLoop) {
						m_CurrentAnimation.m_CurrentOrder = m_CurrentAnimation.m_Order.size() - 1;
					}
					else {
						m_CurrentAnimation.m_CurrentOrder = 0;
					}*/
				}
			}
			else {
				m_CurrentAnimation.m_CurrentOrder++;
				if (m_CurrentAnimation.m_CurrentOrder >= m_CurrentAnimation.m_Order.size() || m_CurrentAnimation.m_CurrentOrder >= m_AnimationUV.size()) {
					m_CurrentAnimation.EndAnimation();
					/*if (m_CurrentAnimation.m_IsLoop) {
						m_CurrentAnimation.m_CurrentOrder = 0;
					}
					else {
						m_CurrentAnimation.m_CurrentOrder = m_CurrentAnimation.m_Order.size() - 1;
					}*/
				}
			}

			UpdateUV(m_AnimationUV[m_CurrentAnimation.m_CurrentOrder]);

			m_AnimationTimer = 0.0f;
		}
	}
	vector<vector<Vec2>> Sprite::CreateAnimationUV(Vec2 cut, const size_t& maxIndex) {
		vector<vector<Vec2>> uv;
		for (size_t y = 0; y < cut.y; y++) {
			for (size_t x = 0; x < cut.x; x++) {
				//最大値に達したらもう作らない
				if (cut.x * y + x > maxIndex) return uv;
				uv.push_back({
					{(1.0f / cut.x) * x,(1.0f / cut.y) * y},
					{(1.0f / cut.x) * (x + 1),(1.0f / cut.y) * y},
					{(1.0f / cut.x) * x,(1.0f / cut.y) * (y + 1)},
					{(1.0f / cut.x) * (x + 1),(1.0f / cut.y) * (y + 1)}
					}
				);
			}
		}
		return uv;
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
	void Sprite::SetVertex(vector<Vec3> positions) {
		for (int i = 0; i < m_Vertices.size(); i++) {
			if (positions.size() <= i) break;
			m_Vertices[i].position = positions[i];
		}
		m_Draw->UpdateVertices(m_Vertices);
	}
	void Sprite::UpdateUV(vector<Vec2> uv) {
		if (uv.empty()) return;
		if (m_Draw) {
			for (int i = 0; i < m_Vertices.size(); i++)
			{
				m_Vertices[i].textureCoordinate = uv[i];
			}

			m_Draw->UpdateVertices(m_Vertices);
		}
	}
	void Sprite::UpdateSize(Vec3 size) {
		m_Transform->SetScale(size);
	}
	void Sprite::UpdateSize(Vec2 size) {
		if (m_Draw) {
			m_Size = size;
			if (m_IsAnimation) {
				CreateVertex(m_Size, m_AnimationUV[m_CurrentAnimation.m_CurrentOrder]);
			}
			else {
				CreateVertex(m_Size, m_AnimationUV[0]);
			}
			m_Draw->UpdateVertices(m_Vertices);
		}
	}

	void Sprite::SetPos(Vec3 pos) {
		m_Transform->SetPosition(pos);
	}
	void Sprite::SetDiffuse(Col4 color) {
		m_Draw->SetDiffuse(color);
	}
	Col4 Sprite::GetDiffuse() {
		return m_Draw->GetDiffuse();
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
			auto trans = number->GetComponent<Transform>();
			trans->SetParent(GetThis<NumberSprite>());
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
	void NumberSprite::Destroy() {
		for (int i = 0; i < m_Numbers.size(); i++) {
			GetStage()->RemoveGameObject<Sprite>(m_Numbers[i]);
		}
		GetStage()->RemoveGameObject<Sprite>(GetThis<NumberSprite>());
	}

	void SpriteAction::OnCreate() {
		m_Draw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_Trans = GetGameObject()->GetComponent<Transform>();
	}

	void SpriteFlash::Reset() {
		if (m_Draw != nullptr) {
			Col4 color = m_Draw->GetDiffuse();
			color.w = 1.0f;
			m_Draw->SetDiffuse(color);
		}
	}
	void SpriteFlash::OnUpdate() {
		if (m_Draw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			Col4 color = m_Draw->GetDiffuse();
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
			m_Draw->SetDiffuse(color);
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
		if (m_Draw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			if (!m_IsFadeOut) {
				elapsed *= -1;
			}
			Col4 color = m_Draw->GetDiffuse();
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
			m_Draw->SetDiffuse(color);
		}
	}
	void SpriteButton::OnCreate() {
		m_SpriteDraw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_UnSelectColor = m_SpriteDraw->GetDiffuse();
		if (m_SelectedColor.w == 1.0f) {
			m_UnSelectColor.w = 0.0f;
		}
		
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
			if (m_Effect != nullptr) {
				m_Effect->Play();
			}
		}
		else {
			m_SpriteDraw->SetTextureResource(m_UnSelectTexture);
			m_SpriteDraw->SetDiffuse(m_UnSelectColor);
			if (m_Effect != nullptr) {
				m_Effect->Reset();
				m_Effect->Stop();
			}
		}
	}


	shared_ptr<ButtonManager> ButtonManager::instance = nullptr;

	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size, nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size, nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size, object, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size, object, func);
	}



	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		auto sprite = stage->AddGameObject<Sprite>(defaultTex, pos, size, true);
		sprite->AddTag(L"Button");
		shared_ptr<SpriteButton> button = nullptr;

		if (selectedTex != L"") {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedTex);
		}
		else if (selectedColor != Col4()) {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedColor);
		}

		button->SetFunction(func, object);
		return sprite;
	}

	void ButtonManager::InitGroup(const wstring& group) {
		if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
			for (auto& button : m_ButtonGroup[group]) {
				button->UnSelect();
				//m_SelectIndexes[group] = 0;
			}
		}
	}

	void ButtonManager::OnCreate() {
		AddTag(L"Manager");
		instance = GetThis<ButtonManager>();
	}
	void ButtonManager::OnUpdate() {
		if (!CheckUpdate()) return;

		if (!m_ButtonGroup[m_UsingGroup][m_SelectIndexes[m_UsingGroup]]->GetActive()) {
			if (m_SelectIndexes[m_UsingGroup] > 0) {
				m_SelectIndexes[m_UsingGroup]--;
			}
			else {
				m_SelectIndexes[m_UsingGroup]++;
			}
		}
		auto& inputState = App::GetApp()->GetInputDevice().GetControlerVec()[0];

		InputData data(0, 0);
		if (PressSelect(m_UsingGroup,data)) {
			int checkButton = static_cast<int>(m_SelectIndexes[m_UsingGroup]) + data.m_MoveAmount;
			checkButton = min(static_cast<int>(m_ButtonGroup[m_UsingGroup].size()) - 1, checkButton);
			checkButton = max(0, checkButton);
			if (m_ButtonGroup[m_UsingGroup][checkButton]->GetActive()) {
				m_SelectIndexes[m_UsingGroup] += data.m_MoveAmount;
			}
		}
		/*if (m_InputDates.find(m_UsingGroup) != end(m_InputDates)) {
			for (auto& inputData : m_InputDates[m_UsingGroup]) {
				if (CheckMoveInput(inputData)) {
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
		}*/
		LimitIndex();

		for (int i = 0; i < m_ButtonGroup[m_UsingGroup].size(); i++) {
			if (i == m_SelectIndexes[m_UsingGroup]) {
				m_ButtonGroup[m_UsingGroup][i]->Select();
			}
			else {
				m_ButtonGroup[m_UsingGroup][i]->UnSelect();
			}
		}
		WORD accept = 0;

		if (PressAccept(m_UsingGroup, m_PressedAccept[m_UsingGroup])) {
			if (m_ClickSound != L"") {
				SoundManager::Instance().PlaySE(m_ClickSound);
			}
			m_PressedAccept[m_UsingGroup] = accept;
			m_ButtonGroup[m_UsingGroup][m_SelectIndexes[m_UsingGroup]]->Func();
		}
		for (auto& acceptButton : m_AcceptButtons[m_UsingGroup]) {
			if (inputState.wPressedButtons & acceptButton) {
				
			}
		}
		/*if (m_AcceptButtons.find(m_UsingGroup) != end(m_AcceptButtons)) {
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
		}*/
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
	bool ButtonManager::CheckUpdate() {
		if (!m_IsActive) return false;
		if (!ExistOpenGroup()) {
			m_IsActive = false;
			return false;
		}
		if (m_SelectIndexes.size() == 0 && m_ButtonGroup.size() == 0) return false;

		return true;
	}
	bool ButtonManager::CheckMoveInput(InputData& input) {

		auto& inputState = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (input.m_Mode == InputMode::Button) {
			return input.CheckInput(inputState.wPressedButtons);
		}
		float inputThumb = 0.0f;
		switch (input.m_StickMode) {
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
		return input.CheckInput(inputThumb);

	}
	void ButtonManager::SetSound(const wstring& sound) {
		m_ClickSound = sound;
	}
	void ButtonManager::SetSelectSound(const wstring& sound) {
		m_SelectSound = sound;
	}
	void ButtonManager::SetMoveAmount(const wstring& group, Vec3 target) {
		if (m_GroupMovementAmount.find(group) != end(m_GroupMovementAmount)) {
			if (m_GroupMovementAmount[group].length() == 0) {
				m_GroupMovementAmount[group] = target;
			}
		}
	}
	void Board::OnCreate() {
		m_Draw = AddComponent<PNTStaticDraw>();
		m_Draw->SetDepthStencilState(DepthStencilState::Read);
		m_Draw->SetOriginalMeshUse(true);
		m_Draw->SetModelDiffusePriority(true);

		m_Draw->SetDiffuse(Col4(1, 1, 1, 1));
		m_Draw->SetEmissive(Col4(1, 1, 1, 1));
		m_Draw->SetSpecular(Col4(1, 1, 1, 1));

		vector<uint16_t> indices = {};
		MeshUtill::CreateSquare(1.0f, m_Vertices, indices);
		m_Draw->CreateOriginalMesh(m_Vertices, indices);
		if (m_TexKey != L"") {
			m_Draw->SetTextureResource(m_TexKey);
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


	SlideInSprite::SlideInSprite(
		const shared_ptr<Stage>& StagePtr, 
		const wstring& TextureKey, 
		bool Trace,
		const Vec2& StartScale, 
		const Vec3& StartPos,
		const float& Maxtime
	) :
		GameObject(StagePtr),
		m_TextureKey(TextureKey),
		m_Trace(Trace),
		m_StartScale(StartScale),
		m_StartPos(StartPos),
		m_time(0),
		m_maxtime(Maxtime)
	{}

	void SlideInSprite::OnCreate() {
		UVCharge = 1.0f;

		Col4 color(1, 1, 1, 1);

		float halfW = 1.0f;
		float halfH = 1.0f;
		//頂点配列(縦横5個ずつ表示)
		m_Vertices = {
			{Vec3(-halfW, halfH, 0.0f), color,Vec2(0.0f, 0.0f)},
			{Vec3(halfW, halfH,   0.0f), color, Vec2(1.0f, 0.0f)},
			{Vec3(-halfW, -halfH, 0.0f), color, Vec2(0.0f, 1.0f)},
			{Vec3(halfW, -halfH, 0.0f), color,	Vec2(1.0f, 1.0f)},
		};
		//インデックス配列
		vector<uint16_t> indices = { 0, 1, 2, 1, 3, 2 };
		SetAlphaActive(m_Trace);
		auto ptrTrans = GetComponent<Transform>();
		ptrTrans->SetScale(m_StartScale.x, m_StartScale.y, 0.2f);
		ptrTrans->SetRotation(0, 0, 0);
		ptrTrans->SetPosition(m_StartPos);
		//頂点とインデックスを指定してスプライト作成
		auto ptrDraw = AddComponent<PCTSpriteDraw>(m_Vertices, indices);
		ptrDraw->SetTextureResource(m_TextureKey);
		ptrDraw->SetDepthStencilState(DepthStencilState::Read);
	}
	void SlideInSprite::OnUpdate()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		m_time += elapsedTime * 5.0f;
		ChargeUV(m_time);
	}
	void SlideInSprite::ChargeUV(const float& time)
	{
		float alpha = m_maxtime;
		if (time < alpha) {
			UpdateProgress(time);
		}
	}

	void SlideInSprite::UpdateProgress(float time) {
		float progress = time;
		m_Vertices[1].position.x = (progress * 2) - 1.0f;
		m_Vertices[3].position.x = (progress * 2) - 1.0f;
		m_Vertices[1].textureCoordinate.x = progress;
		m_Vertices[3].textureCoordinate.x = progress;

		GetComponent<PCTSpriteDraw>()->UpdateVertices(m_Vertices);
	}

	void SlideInSprite::SetDiffuse(Col4 rgba)
	{
		auto ptrDraw = GetComponent<PCTSpriteDraw>();
		ptrDraw->SetDiffuse(rgba);
	}

}
//end basecross
