/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Player::Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
		Character(stage),
		m_Position(position),
		m_Rotation(rotation),
		m_Scale(scale),
		m_EnergyCharge(0.0f),
		m_PlayerStateNum(PlayerState::NORMAL),
		m_ZoneTime(0.0f)
	{
	}
	Player::~Player()
	{
	}

	Vec2 Player::GetInputState() const {
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


	Vec3 Player::GetMoveVector() {
		Vec3 angle(0, 0, 0);
		//入力の取得
		float moveX = GetInputState().x;
		float moveZ = GetInputState().y;

		if (moveX + moveZ != 0) {
			auto ptrTransform = GetComponent<Transform>();
			auto ptrCamera = OnGetDrawCamera();

			//進行方向の向きを計算
			auto front = ptrTransform->GetPosition() - ptrCamera->GetEye();
			front.y = 0;
			front.normalize();
			//進行方向向きからの角度を算出
			float frontAngle = atan2(front.z, front.x);

			//コントローラの向き計算
			Vec2 moveVec(moveX, moveZ);
			float moveSize = moveVec.length();
			//コントローラの向きから角度を計算
			float cntlAngle = atan2(-moveX, moveZ);
			//トータルの角度を算出
			float totalAngle = frontAngle + cntlAngle;

			//角度からベクトルを作成
			angle = Vec3(cos(totalAngle), 0, sin(totalAngle));
			//正規化する
			angle.normalize();
			//移動サイズを設定。
			angle *= moveSize;
			//Y軸は変化させない
			angle.y = 0;
		}
		return angle;
	}

	void Player::MovePlayer() {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto angle = GetMoveVector();
		if (angle.length() > 0.0f) {
			auto pos = GetComponent<Transform>()->GetPosition();
			pos += angle * elapsedTime * 6.0f;
			GetComponent<Transform>()->SetPosition(pos);
		}
		//回転の計算
		if (angle.length() > 0.0f) {
			auto utilPtr = GetBehavior<UtilBehavior>();
			utilPtr->RotToHead(angle, 1.0f);
		}
	}

	void Player::ZoneActivation()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (m_EnergyCharge > 1.0)
		{
			if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_B)
			{
				if ((m_PlayerStateNum & PlayerState::ZONE) == 0)
				{
					m_PlayerStateNum += PlayerState::ZONE;
				}
			}
		}
		else {
			m_EnergyCharge += 0.01f;
		}

		if ((m_PlayerStateNum & PlayerState::ZONE) != 0)
		{
			m_ZoneTime += elapsedTime;
			if (m_ZoneTime > 5.0f)
			{
				m_PlayerStateNum -= PlayerState::ZONE;
				m_PlayerStateNum += PlayerState::NORMAL;
				m_ZoneTime = 0;
				m_EnergyCharge = 0;
			}
		}

	}

	void Player::Debug()
	{
		//auto scene = App::GetApp()->GetScene<Scene>();
		//wstringstream wss(L"");
		//wss << L"\nZoneCharge : "
		//	<< m_EnergyCharge
		//	<< endl;
		//scene->SetDebugString(wss.str());
	}

	void Player::PlayerHit()
	{
		if ((m_PlayerStateNum & PlayerState::GUARD)	!= 0)
		{
			m_HP -= 0;
		}
		else {
			m_HP -= 1;
		}
	}

	Vec3 Player::GetForward()
	{
		return GetComponent<Transform>()->GetForward();
	}

	int Player::GetStates()
	{
		return m_PlayerStateNum;
	}

	int Player::GetPlayerHP()
	{
		return m_HP;
	}

	void Player::OnCreate()
	{
		m_HP = 5;
		//初期位置の設定
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(m_Position);
		ptr->SetRotation(m_Rotation);
		ptr->SetScale(m_Scale);

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//重力をつける
		auto ptrGra = AddComponent<Gravity>();


		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		AddTag(L"Player");

	}
	void Player::OnUpdate()
	{
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		auto ptrDraw = GetComponent<BcPNTStaticDraw>();
		//コントローラチェックして入力があればコマンド呼び出し
		//m_InputHandler.PushHandle(GetThis<Player>());
		MovePlayer();
		ZoneActivation();
		Debug();
		if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			m_PlayerStateNum += PlayerState::GUARD;
			ptrDraw->SetDiffuse(Col4(0, 0, 0, 0));
		}
		if (cntlVec[0].wReleasedButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			m_PlayerStateNum -= PlayerState::GUARD;
			ptrDraw->SetDiffuse(Col4(1, 1, 1, 1));
		}
		if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_Position = GetComponent<Transform>()->GetPosition();
			Vec3 forward = GetComponent<Transform>()->GetForward();
			GetStage()->AddGameObject<HitSphere>(m_Position + forward / 2, forward);
		}
	}
	void Player::OnDraw()
	{
		Character::OnDraw();
	}

	HitSphere::HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward) :
		GameObject(stage),
		m_HitPosition(position),
		m_HitRotation(forward),
		m_HitScale(Vec3(0.5f, 0.5f, 0.5f)),
		m_FlyingTime(1.0f),
		m_TotalTime(0.0f),
		m_Speed(12.0f)
	{
	}

	void HitSphere::OnCreate()
	{
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(m_HitPosition);
		ptr->SetRotation(m_HitRotation);
		ptr->SetScale(m_HitScale);

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);
		ptrColl->SetAfterCollision(AfterCollision::None);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");
		AddTag(L"HitJudge");
	}

	void HitSphere::OnUpdate()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();
		if ((state & Player::PlayerState::ZONE) == 0) {
			m_FlyingTime = 0.1f;
			m_Speed = 12.0f;
		}
		else {
			m_FlyingTime = 0.5f;
			m_Speed = 24.0f;
		}
		Vec3 hitPosition = GetComponent<Transform>()->GetPosition();
		if (m_FlyingTime > m_TotalTime)
		{
			hitPosition += m_Speed * m_HitRotation * elapsedTime;
		}
		else {
			SetDrawActive(false);
			SetUpdateActive(false);
		}
		GetComponent<Transform>()->SetPosition(hitPosition);
		m_TotalTime += elapsedTime;
	}
}
//end basecross

