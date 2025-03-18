/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Player::Player(const shared_ptr<Stage>& stage) : Player(stage,Vec3(),Vec3(),Vec3(1.0f)){}

	Player::Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
		Character(stage),
		m_Position(position),
		m_Rotation(rotation),
		m_Scale(scale),
		m_MoveSpeed(6.0f),
		m_EnergyCharge(0.0f),
		m_PlayerStateNum(PlayerState::NORMAL),
		m_ZoneTime(0.0f),
		m_ParryTime(30.0f),
		m_ParryJudge(false),
		m_BoostTime(1.0f)
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

	void Player::MovePlayer(const float Speed) {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto angle = GetMoveVector();
		if (angle.length() > 0.0f) {
			auto pos = GetComponent<Transform>()->GetPosition();
			pos += angle * elapsedTime * Speed;
			GetComponent<Transform>()->SetPosition(pos);
		}
		//回転の計算
		if (angle.length() > 0.0f) {
			auto utilPtr = GetBehavior<UtilBehavior>();
			utilPtr->RotToHead(angle, 1.0f);
		}
	}

	void Player::BoostMove(const float Speed, const Vec3 Angle) {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		if (Angle.length() > 0.0f) {
			auto pos = GetComponent<Transform>()->GetPosition();
			pos += Angle * elapsedTime * Speed;
			GetComponent<Transform>()->SetPosition(pos);
		}
		//回転の計算
		if (Angle.length() > 0.0f) {
			auto utilPtr = GetBehavior<UtilBehavior>();
			utilPtr->RotToHead(Angle, 1.0f);
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

	void Player::SearchRange()
	{
		Vec3 forward = GetComponent<Transform>()->GetForward();
		Vec3 position = GetComponent<Transform>()->GetPosition();
		float searchDistance = 10.0f;
		auto targetVector = BulletSearch();
		if (targetVector != nullptr)
		{
			Vec3 target = targetVector->GetComponent<Transform>()->GetPosition();
			if ((position - target).length() < searchDistance)
			{
				if (IsWithinDetectionRange(position, target, 90.0)) {
					Vec3 rot = RotateTowardsTarget(position, target);
					float rad = atan2f(rot.x, rot.z);
					GetComponent<Transform>()->SetRotation(0, rad, 0);
				}
			}
		}
	}

	Vec3 Player::RotateTowardsTarget(const Vec3& object, const Vec3& target) {
		// 目標方向ベクトルを計算
		Vec3 direction = {
			target.x - object.x,
			target.y - object.y,
			target.z - object.z
		};

		// ベクトルを正規化
		Vec3 normalizedDirection = direction.normalize();

		return normalizedDirection; // 向きベクトルを返却
	}

	shared_ptr<GameObject> Player::BulletSearch()
	{
		auto bulletGroup = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		auto target = bulletGroup->GetGroupVector();
		shared_ptr<GameObject> nearBullet = nullptr;
		for (auto vec : target)
		{
			auto sharedObject = vec.lock();
			if (nearBullet == nullptr)
			{
				nearBullet = sharedObject;
			}
			else if (nearBullet != nullptr)
			{

				if (sharedObject != nullptr)
				{
					Vec3 position = GetComponent<Transform>()->GetPosition();
					Vec3 vec0 = nearBullet->GetComponent<Transform>()->GetPosition();
					Vec3 vec1 = sharedObject->GetComponent<Transform>()->GetPosition();
					if ((vec1 - position).length() < (vec0 - position).length())
					{
						nearBullet = sharedObject;
					}
				}
			}
		}
		return nearBullet;
	}


	void Player::Debug()
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		wstringstream wss(L"");
		wss << L"\nZoneCharge : "
			<< m_EnergyCharge
			<< L"\nHP"
			<< m_HP
			<< L"\nx"
			<< m_Rotation.x
			<< L"\ny"
			<< m_Rotation.y
			<< L"\nz"
			<< m_Rotation.z
			<< endl;
		scene->SetDebugString(wss.str());
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
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto ptrDraw = GetComponent<BcPNTStaticDraw>();
		float spped = 0.0f;
		//コントローラチェックして入力があればコマンド呼び出し
		//m_InputHandler.PushHandle(GetThis<Player>());
		ZoneActivation();
		Debug();

		m_Rotation = GetComponent<Transform>()->GetRotation();

		if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_ParryJudge = true;
			SearchRange();
			m_Position = GetComponent<Transform>()->GetPosition();
			Vec3 forward = GetComponent<Transform>()->GetForward();
			GetStage()->AddGameObject<HitSphere>(m_Position + forward / 2, forward, GetThis<GameObject>());
		}

		if (m_ParryJudge == true)
		{
			m_ParryTime--;
			if (m_ParryTime <= 0.0f)
			{
				m_ParryJudge = false;
			}
		}

		if ((m_PlayerStateNum & PlayerState::DASH) != 0)
		{
			m_BoostTime -= elapsedTime;
			if (m_BoostTime >= 0.0f)
			{
				BoostMove(6.0f * 3.0f, m_BoostAngle);
			}
			else {
				m_PlayerStateNum += PlayerState::NORMAL;
				m_PlayerStateNum -= PlayerState::DASH;
			}
		}
		else {
			m_BoostTime = 1.0f;
			MovePlayer(6.0f);
			if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X)
			{
				m_BoostAngle = GetMoveVector();
				m_PlayerStateNum -= PlayerState::NORMAL;
				m_PlayerStateNum += PlayerState::DASH;
			}
		}

	}

	void Player::OnDraw()
	{
		Character::OnDraw();
	}
	shared_ptr<Object> Player::Create() {
		return GetStage()->AddGameObject<Player>();
	}

	void Player::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"Bullet"))
		{
			if (m_ParryJudge == true)
			{
				if (m_ParryTime <= 30 && m_ParryTime > 15)
				{
					m_HP -= 0;
				}
				else if (m_ParryTime <= 15 && m_ParryTime > 0)
				{
					m_HP -= 1;
				}
			}
			else {
				m_HP -= 2;
			}
			m_ParryJudge = false;
			m_ParryTime = 30.0f;
		}
	}

	HitSphere::HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward, const shared_ptr<GameObject> player) :
		GameObject(stage),
		m_HitPosition(position),
		m_HitRotation(forward),
		m_Player(player),
		m_HitScale(Vec3(1.0f, 1.0f, 1.0f)),
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
		ptrColl->SetAfterCollision(AfterCollision::Auto);

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
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
		GetComponent<Transform>()->SetPosition(hitPosition);
		m_TotalTime += elapsedTime;
	}

	void HitSphere::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"Bullet"))
		{
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			player->OnCollisionEnter(other);
		}
	}

}
//end basecross

