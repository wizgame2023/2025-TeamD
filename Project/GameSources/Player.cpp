/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Player::Player(const shared_ptr<Stage>& stage) : Player(stage, Vec3(), Vec3(), Vec3(1.0f)) {}

	Player::Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
		Character(stage, position, rotation, scale),
		m_MoveSpeed(6.0f),
		m_EnergyCharge(0.0f),
		m_PlayerStateNum(PlayerState::NORMAL),
		m_ZoneTime(0.0f),
		m_ParryTime(30.0f),
		m_ParryJudge(false),
		m_BoostTime(1.0f),
		m_BulletDire(Vec3(0))
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

	Vec3 Player::GetMoveVector(float& rot) {
		Vec3 angle(0, 0, 0);
		//入力の取得
		float moveX = GetInputState().x;
		float moveZ = GetInputState().y;

		if (moveX + moveZ != 0) {
			auto ptrCamera = OnGetDrawCamera();

			float angleY = dynamic_pointer_cast<FollowCamera>(ptrCamera)->GetAngle();
			float movemove = atan2f(-moveZ, moveX);
			float fRotate = movemove - angleY - XM_PIDIV2;

			angle = Vec3(cos(fRotate), 0.0f, -sin(fRotate));

			float rotate = fRotate + XM_PIDIV2;
			angle.normalize();
			rot = rotate;
		}
		return angle;
	}

	void Player::MovePlayer(const float Speed) {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		float rot;
		auto angle = GetMoveVector(rot);
		if (angle.length() > 0.0f) {
			auto pos = GetPosition();
			pos += angle * elapsedTime * Speed;
			SetPosition(pos);
		}
		//回転の計算
		if (angle.length() > 0.0f) {
			//auto utilPtr = GetBehavior<UtilBehavior>();
			//utilPtr->RotToHead(angle, 1.0f);
			SetRotation(Vec3( 0,XMConvertToDegrees(rot),0));
			m_BulletDire = GetForward();

		}
	}

	void Player::BoostMove(const float Speed, const Vec3 Angle) {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		if (Angle.length() > 0.0f) {
			auto pos = GetPosition();
			pos += Angle * elapsedTime * Speed;
			SetPosition(pos);
		}
	}

	void Player::ZoneActivation()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (m_EnergyCharge >= 1.0)
		{
			if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_B)
			{
				if ((m_PlayerStateNum & PlayerState::ZONE) == 0)
				{
					m_PlayerStateNum += PlayerState::ZONE;
				}
			}
			m_EnergyCharge = 1.0f;
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
		Vec3 forward = m_Transform->GetForward();
		Vec3 position = m_Transform->GetPosition();
		float searchDistance = 10.0f;
		auto bulletGroup = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		auto enemyGroup = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		auto targetBulletVector = ObjectSearch(bulletGroup);
		auto targetEnemyVector = ObjectSearch(enemyGroup);
		if (targetEnemyVector != nullptr)
		{
			Vec3 targetEnemy = targetEnemyVector->GetComponent<Transform>()->GetPosition();
			if ((position - targetEnemy).length() < searchDistance / 3.0f)
			{
				if (IsWithinDetectionRange(position, targetEnemy, 90.0)) {
					//この方向に少し動く、動いている間はコントローラで移動できない
					Vec3 rot = RotateTowardsTarget(position, targetEnemy);
					float rotate = atan2f(rot.x, rot.z) ;
					m_Transform->SetRotation(Vec3(0.0f, rotate, 0.0f));
				}
			}
			else if (targetBulletVector != nullptr)
			{
				Vec3 targetbullert = targetBulletVector->GetComponent<Transform>()->GetPosition();
				if ((position - targetbullert).length() < searchDistance)
				{
					if (IsWithinDetectionRange(position, targetbullert, 90.0)) {
						//この方向に少し動く、動いている間はコントローラで移動できない
						Vec3 rot = RotateTowardsTarget(position, targetbullert);
						float rotate = atan2f(rot.x, rot.z);
						m_Transform->SetRotation(Vec3(0.0f, rotate, 0.0f));
					}
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

	shared_ptr<GameObject> Player::ObjectSearch(const shared_ptr<GameObjectGroup>& group)
	{
		auto target = group->GetGroupVector();
		shared_ptr<GameObject> nearObject = nullptr;
		for (auto vec : target)
		{
			auto sharedObject = vec.lock();
			if (nearObject == nullptr)
			{
				nearObject = sharedObject;
			}
			else if (nearObject != nullptr)
			{

				if (sharedObject != nullptr)
				{
					Vec3 position = m_Transform->GetPosition();
					Vec3 vec0 = nearObject->GetComponent<Transform>()->GetPosition();
					Vec3 vec1 = sharedObject->GetComponent<Transform>()->GetPosition();
					if ((vec1 - position).length() < (vec0 - position).length())
					{
						nearObject = sharedObject;
					}
				}
			}
		}
		return nearObject;
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
		return m_Transform->GetForward();
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
		Character::OnCreate();
		m_HP = 20;

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"01");
		
		//auto ptrDraw = AddComponent<PNTBoneModelDraw>();
		//Mat4x4 meshMat;
		//meshMat.affineTransformation(
		//	Vec3(.4f, .4f, .4f), //(.1f, .1f, .1f),
		//	Vec3(0.0f, 0.0f, 0.0f),
		//	Vec3(0.0f, XM_PI, 0.0f),
		//	Vec3(0.0f, -XM_PIDIV2, 0.0f)
		//);

		//ptrDraw->SetMeshResource(L"PLAYER");
		//ptrDraw->SetMeshToTransformMatrix(meshMat);
		//ptrDraw->SetBlendState(BlendState::AlphaBlend);
		//ptrDraw->SetOwnShadowActive(true);

		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		AddTag(L"Player");


		m_Stage->SetSharedGameObject(L"Player", GetThis<Player>());
	}

	void Player::OnUpdate()
	{
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		float spped = 0.0f;
		//コントローラチェックして入力があればコマンド呼び出し
		//m_InputHandler.PushHandle(GetThis<Player>());
		ZoneActivation();
		Debug();

		if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_ParryJudge = true;
			SearchRange();
			m_Position = GetPosition();
			//Vec3 forward = Vec3(cos(m_Rotation.y), 0, sin(m_Rotation.y));
			Vec3 forward = GetForward();
			m_Stage->AddGameObject<HitSphere>(Vec3(m_Position.x + forward.x /2, m_Position.y + 0.25f, m_Position.z + forward.z /2), forward, GetThis<GameObject>());
		}

		if (m_ParryJudge == true)
		{
			m_ParryTime--;
			if (m_ParryTime <= 0.0f)
			{
				m_ParryJudge = false;
			}
		}
		float rot;

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
			m_BoostTime = 0.2f;
			MovePlayer(6.0f);
			if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X)
			{
				m_BoostAngle = GetForward();;
				m_PlayerStateNum -= PlayerState::NORMAL;
				m_PlayerStateNum += PlayerState::DASH;
			}
		}

	}

	void Player::OnDraw()
	{
		Character::OnDraw();
	}
	void Player::Dead() {
		SetPosition(Vec3(0, 2, 0));
		m_HP = 5;
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
					m_EnergyCharge += 0.2;
				}
				else if (m_ParryTime <= 15 && m_ParryTime > 0)
				{
					m_HP -= 1;
					m_EnergyCharge += 0.1;
				}
			}
			else {
				m_HP -= 2;
				m_EnergyCharge += 0.2;
			}
			m_HP = max(m_HP, 0);
			if (m_HP <= 0) {
				Dead();
			}
			m_ParryJudge = false;
			m_ParryTime = 30.0f;
		}
		if (other->FindTag(L"Enemy"))
		{
			m_EnergyCharge += 0.1;
		}
	}

	HitSphere::HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward, const shared_ptr<GameObject> player) :
		GameObject(stage),
		m_HitPosition(position),
		m_HitRotation(forward),
		m_Player(player),
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
		if (other->FindTag(L"Bullet") || other->FindTag(L"Object"))
		{
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			player->OnCollisionEnter(other);
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
		if (other->FindTag(L"Enemy"))
		{
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			player->OnCollisionEnter(other);
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
	}

}
//end basecross

