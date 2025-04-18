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
		m_BulletDire(Vec3(0)),
		m_Attacktime(1.0f),
		m_DamageInterval(0.5f),
		m_BoostInterval(1.0F),
		m_IsGoal(false)
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
			Move(angle);
		}
		//回転の計算
		if (angle.length() > 0.0f) {
			//auto utilPtr = GetBehavior<UtilBehavior>();
			//utilPtr->RotToHead(angle, 1.0f);
			SetRotation(Vec3(0, rot, 0));
			m_BulletDire = GetForward();

		}
	}

	void Player::BoostMove(const float Speed, const Vec3 Angle) {
		float elapsedTime = App::GetApp()->GetElapsedTime();
		m_TotalTime += elapsedTime;
		if (Angle.length() > 0.0f) {
			auto pos = GetPosition();
			pos += Angle * Speed;
			auto boost = Lerp::CalculateLerp(GetPosition(), pos, 0, 1.0f, m_TotalTime, Lerp::rate::Linear);
			SetPosition(boost);
		}
		m_TotalTime = 0;
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
					m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));
					SoundManager::Instance().PlaySE(L"SE_USE_ULT");
				}
			}
			m_EnergyCharge = 1.0f;
		}

		if ((m_PlayerStateNum & PlayerState::ZONE) != 0)
		{
			SetAttackDamage(3.0f);
			m_ZoneTime += elapsedTime;
			if (m_ZoneTime > 5.0f)
			{
				m_PlayerStateNum -= PlayerState::ZONE;
				m_PlayerStateNum += PlayerState::NORMAL;
				m_ZoneTime = 0;
				SetAttackDamage(1.0f);
				m_EnergyCharge = 0;
				m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));
			}
		}

	}

	Vec3 Player::SearchRange()
	{
		Vec3 forward = m_Transform->GetForward();
		Vec3 position = m_Transform->GetPosition();
		float searchDistance = 10.0f;
		auto bulletGroup = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		auto enemyGroup = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		auto targetBulletVector = ObjectSearch(bulletGroup);
		auto targetEnemyVector = ObjectSearch(enemyGroup);
		m_TargetBoard->SetTarget(nullptr);
		if (targetEnemyVector != nullptr)
		{
			Vec3 targetEnemy = targetEnemyVector->GetComponent<Transform>()->GetPosition();
			if ((position - targetEnemy).length() < searchDistance / 3.0f)
			{
				if (IsWithinDetectionRange(forward, targetEnemy - position, 90.0)) {
					//この方向に少し動く、動いている間はコントローラで移動できない
					Vec3 rot = RotateTowardsTarget(position, targetEnemy);
					m_TargetBoard->SetTarget(targetEnemyVector);
					return rot;
				}
				else {
					return Vec3();
				}
			}
			else if (targetBulletVector != nullptr)
			{
				Vec3 targetbullert = targetBulletVector->GetComponent<Transform>()->GetPosition();
				if ((position - targetbullert).length() < searchDistance)
				{
					if (IsWithinDetectionRange(forward, targetbullert - position, 45.0)) {
						//この方向に少し動く、動いている間はコントローラで移動できない
						Vec3 rot = RotateTowardsTarget(position, targetbullert);
						return rot;
					}
					else {
						return Vec3();
					}
				}
				else {
					return Vec3();
				}
			}
			else {
				return Vec3();
			}
		}
		else {
			return Vec3();
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

	void Player::AimRock(Vec3 rot)
	{
		if (rot != Vec3())
		{
			float rotate = atan2f(rot.x, rot.z);
			SetRotation(Vec3(0.0f, rotate, 0.0f));
		}
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
	float Player::GetEnergy() {
		return m_EnergyCharge;
	}
	void Player::SetIsGaol(const bool& goal)
	{
		m_IsGoal = goal;
	}

	void Player::OnCreate()
	{
		Character::OnCreate();
		InitHP(20);
		SetAttackDamage(1.0f);
		SetSpeed(2.0f);
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		//描画設定
		/*auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");
		ptrDraw->SetTextureResource(L"01");*/


		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.1f), //(.1f, .1f, .1f),
			Vec3(0.0f, 90.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -0.5f, 0.0f)
		);
		ptrDraw->SetMeshResource(L"PLAYER");
		ptrDraw->SetTextureResource(L"01");
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::AlphaBlend);
		ptrDraw->SetOwnShadowActive(true);

		ptrDraw->AddAnimation(L"DEFAULT", 0, 120, true, 60);
		ptrDraw->ChangeCurrentAnimation(L"DEFAULT");
		ptrDraw->SetDiffuse(Col4(1, 0, 0, 1));
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		AddTag(L"Player");

		m_TargetBoard = m_Stage->AddGameObject<TargetBoard>(GetThis<Player>());
		auto stage = static_pointer_cast<GameStage>(m_Stage);

		if (stage != nullptr) {
			m_Effect = stage->GetCreateEffect();
		}
		else {
			m_Effect = nullptr;
		}

		m_Stage->SetSharedGameObject(L"Player", GetThis<Player>());
	}

	void Player::OnUpdate()
	{
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto draw = GetComponent<BcBaseDraw>();
		draw->UpdateAnimation(elapsedTime);
		float spped = 0.0f;
		if (m_IsGoal == false)
		{
			ZoneActivation();
			Debug();


			if (m_ParryJudge == true)
			{
				m_ParryTime--;
				if (m_ParryTime <= 0.0f)
				{
					m_ParryJudge = false;
				}
			}

			if (m_DamageIntervalStart)
			{
				m_DamageInterval -= elapsedTime;
				draw->SetDiffuse(Col4(1, 0, 0, 1));
				if (m_DamageInterval <= 0.0f)
				{
					draw->SetDiffuse(Col4(1, 1, 1, 1));
					m_DamageIntervalStart = false;
					m_DamageInterval = 0.5f;
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
					m_BoostInterval = 1.0f;
				}
			}
			else if ((m_PlayerStateNum & PlayerState::ATTACK) != 0)
			{
				m_Attacktime -= elapsedTime;
				if (m_Attacktime >= 0.0f)
				{
				}
				else {
					m_PlayerStateNum -= PlayerState::ATTACK;
					m_PlayerStateNum += PlayerState::NORMAL;
				}

			}
			else {
				m_BoostTime = 0.2f;
				m_Attacktime = 0.2f;

				m_BoostInterval -= elapsedTime;
				m_AttackInterval -= elapsedTime;
				MovePlayer(6.0f);
				Vec3 rot = SearchRange();

				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X)
				{
					m_BoostAngle = GetForward();
					if (m_BoostInterval <= 0.0f)
					{
						m_PlayerStateNum -= PlayerState::NORMAL;
						m_PlayerStateNum += PlayerState::DASH;
						SoundManager::Instance().PlaySE(L"SE_RUN");
					}
				}

				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
				{
					m_ParryJudge = true;

					AimRock(rot);
					m_Position = GetPosition();
					Vec3 forward = GetForward();

					//BoostMove(15.0f, forward);
					m_Stage->AddGameObject<HitSphere>(Vec3(m_Position), forward, GetThis<GameObject>());
					float rotate = atan2f(forward.x, forward.z);

				m_Effect->PlayEffect(L"Flash", Vec3(m_Position.x + forward.x / 2, m_Position.y + 0.25f, m_Position.z + forward.z / 2), 8.0f);
				m_Effect->SetRotation(Vec3(0.0f, 1.0f, 0.0f), rotate);
				m_Effect->SetScale(Vec3(0.2f, 0.2f, 0.2f));

					m_PlayerStateNum += PlayerState::ATTACK;
					m_PlayerStateNum -= PlayerState::NORMAL;

					SoundManager::Instance().PlaySE(L"SE_ATTACK_VOICE", 0.5f);
				}
			}

		}
		else {

		}

	}

	void Player::OnDraw()
	{
		Character::OnDraw();
	}
	void Player::Dead() {
		SetPosition(Vec3(0, 2, 0));
		InitHP(1000);
	}

	void Player::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"Bullet"))
		{
			if (m_DamageIntervalStart == false)
			{
				if (m_ParryJudge == true)
				{
					if (m_ParryTime <= 30 && m_ParryTime > 15)
					{
						m_HP -= 0;
						m_EnergyCharge += 0.2;
						Vec3 forward = GetForward();

						m_Effect->PlayEffect(L"Parry", Vec3(m_Position.x + forward.x / 2, m_Position.y, m_Position.z + forward.z / 2), 25.0f);
						m_Effect->SetScale(Vec3(0.1f, 0.1f, 0.1f));
					}
					else if (m_ParryTime <= 15 && m_ParryTime > 0)
					{
						Damage(1.0f, true);
						m_DamageIntervalStart = true;
						m_EnergyCharge += 0.1;
						ScoreManager::Instance()->AddDamage(1);
					}
					ScoreManager::Instance()->AddParryCount();
					SoundManager::Instance().PlaySE(L"SE_GUARD");
				}
				else {
					Damage(1.0f, false);
					m_DamageIntervalStart = true;
					m_EnergyCharge += 0.2;
					ScoreManager::Instance()->AddDamage(2);
					SoundManager::Instance().PlaySE(L"SE_HIT_PLAYER");
				}
			}
			m_HP = max(m_HP, 0);
			if (m_HP <= 0) {
				Dead();
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
		m_HitScale(Vec3(1.0f)),
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
			m_Speed = 8.0f;
		}
		else {
			m_FlyingTime = 0.5f;
			m_Speed = 12.0f;
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
		if (other->FindTag(L"Bullet") || other->FindTag(L"BossAttack"))
		{
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			player->OnCollisionEnter(other);
		}
		if (other->FindTag(L"Enemy"))
		{
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
	}

}
//end basecross

