/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Player.h"


namespace basecross {
	Player::Player(const shared_ptr<Stage>& stage) : Player(stage, Vec3(), Vec3(), Vec3(1.0f)) {}

	Player::Player(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
		Character(stage, position, rotation, scale),
		m_ParryHandle(-1),
		m_Handle(-1),
		m_MoveSpeed(6.0f),
		m_EnergyCharge(0.0f),
		m_PlayerStateNum(PlayerState::NORMAL),
		m_ZoneTime(0.0f),
		m_ParryTime(30.0f),
		m_ParryJudge(false),
		m_BoostTime(1.0f),
		m_BulletDire(Vec3(0)),
		m_Attacktime(1.0f),
		m_Damage(1.0f),
		m_DamageInterval(0.5f),
		m_BoostInterval(1.0F),
		m_IsGoal(false),
		m_zoneAnim(1.0f),
		m_HitScale(Vec3(1)),
		m_SearchDistance(2.0),
		m_Length(4.0f)

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
			Move(angle,false);
		}
		//回転の計算
		if (angle.length() > 0.0f) {
			//auto utilPtr = GetBehavior<UtilBehavior>();
			//utilPtr->RotToHead(angle, 1.0f);
			SetRotation(Vec3(0, rot, 0));
			m_BulletDire = GetForward();
			SetAnim(L"Dash");
		}
		else {
			if ((m_PlayerStateNum & PlayerState::NORMAL) == 1)
			{
				SetAnim(L"Idle");
			}
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
					SetAnim(L"Zone");
					m_Damage = 3.0f;
					m_zoneAnim = 1.0f;
					m_HitScale = Vec3(3.0f);
					m_SearchDistance = 24.0f;
					m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));
					SoundManager::Instance().PlaySE(L"SE_USE_ULT");
					m_PlayerStateNum += PlayerState::ZONE;
					m_PlayerStateNum -= PlayerState::NORMAL;

					GameManager::Instance()->StartZone(5.0f);
				}
			}
			m_EnergyCharge = 1.0f;
		}

		if ((m_PlayerStateNum & PlayerState::ZONE) != 0)
		{
			SetAttackDamage(3.0f);
			m_ZoneTime += elapsedTime;
			if (m_ZoneTime > 5.0f + m_zoneAnim)
			{
				m_Damage = 1.0f;
				m_ZoneTime = 0;
				SetAttackDamage(1.0f);
				m_HitScale = Vec3(1.0f);
				m_EnergyCharge = 0;
				m_SearchDistance = 2.0f;
				m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));
				m_PlayerStateNum -= PlayerState::ZONE;
				m_PlayerStateNum += PlayerState::NORMAL;

				//GameManager::Instance()->SetTimeRate(1.0f);
			}
		}
	}

	Vec3 Player::SearchRange()
	{
		Vec3 forward = m_Transform->GetForward();
		Vec3 position = m_Transform->GetPosition();
		auto bulletGroup = GetStage()->GetSharedObjectGroup(L"BulletGroup");
		auto enemyGroup = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		auto targetBulletVector = ObjectSearch(bulletGroup);
		auto targetEnemyVector = ObjectSearch(enemyGroup);
		m_TargetBoard->SetTarget(nullptr);
		if (targetEnemyVector != nullptr)
		{
			Vec3 targetEnemy = targetEnemyVector->GetComponent<Transform>()->GetPosition();
			if (IsWithinDetectionRange(forward, targetEnemy - position, 90.0)) {
				m_TargetBoard->SetTarget(targetEnemyVector);
				if ((position - targetEnemy).length() <= m_SearchDistance + 1.0f + 0.85f)
				{
					//この方向に少し動く、動いている間はコントローラで移動できない
					Vec3 rot = RotateTowardsTarget(position, targetEnemy);
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

	void Player::SetCharge(const float& charge)
	{
		m_EnergyCharge += charge;
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
		auto target = group->GetGroupVectors();
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

	void Player::UpdateAnim()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime();
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		draw->UpdateAnimation(elapsedTime);
	}

	float Player::Parry(float damage, const float& ParrySecond)
	{
		float parryTime = 30.0f;
		Vec3 forward = GetForward();
		if (ParrySecond > 15)
		{
			m_EnergyCharge += 0.2;

			m_Effect->PlayEffect(m_ParryHandle, L"Parry", GetPosition() + GetForward(), 0.0f);
			m_Effect->SetScale(m_ParryHandle, Vec3(0.25f));

			m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f);

			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535;
			vibration.wRightMotorSpeed = 65535;
			XInputSetState(0, &vibration);

			ScoreManager::Instance()->AddParryCount();
			SoundManager::Instance().PlaySE(L"SE_GUARD");

			PostEvent(0.5f, nullptr, GetStage(), L"StopVibration");
			return 0;
		}
		else if (ParrySecond <= 15 && ParrySecond > 5)
		{
			m_EnergyCharge += 0.1;
			m_Effect->PlayEffect(m_ParryHandle, L"Parry", GetPosition() + GetForward(), 0.0f);
			m_Effect->SetScale(m_ParryHandle, Vec3(0.25f));
			m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f);

			//XINPUT_VIBRATION vibration;
			//vibration.wLeftMotorSpeed = 65535 * 0.5f;
			//vibration.wRightMotorSpeed = 65535 * 0.5f;
			//XInputSetState(0, &vibration);
			ScoreManager::Instance()->AddParryCount();
			SoundManager::Instance().PlaySE(L"SE_GUARD");

			//PostEvent(0.25f, nullptr, GetStage(), L"StopVibration");
			return 0;
		}
		else
		{
			m_DamageIntervalStart = true;
			m_EnergyCharge += 0.2;
			SoundManager::Instance().PlaySE(L"SE_HIT_PLAYER");
			return damage;
		}

	}

	void Player::AddAnimation()
	{
		auto ptrDraw = GetComponent<BcPNTBoneModelDraw>();
		auto anim_fps = 60.0f;
		ptrDraw->AddAnimation(L"Idle", 11, 60, true, anim_fps);
		ptrDraw->AddAnimation(L"Attack", 81, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"Attack2", 421, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"Zone", 151, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"Dash", 212, 60, true, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Brink", 270, 1, true, anim_fps);
		ptrDraw->AddAnimation(L"Nock", 281, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Died", 351, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Clear", 491, 109, false, anim_fps);
	}

	void Player::PlayAnimation()
	{
		if ((m_PlayerStateNum & PlayerState::ZONE) == 1)
		{
		}
	}

	void Player::Debug()
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		wstringstream wss(L"");
		wss << L"¥nZoneCharge : "
			<< m_EnergyCharge
			<< L"¥nHP"
			<< m_HP
			<< L"¥nx"
			<< m_Rotation.x
			<< L"¥ny"
			<< m_Rotation.y
			<< L"¥nz"
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
	float Player::GetEnergy()
	{
		return m_EnergyCharge;
	}
	float Player::GetDamage()
	{
		return m_Damage;
	}
	bool Player::GetParry()
	{
		return m_ParryJudge;
	}
	void Player::SetParryPosition(const Vec3& position)
	{
		m_EffectVec = position;
		m_ParryJudge = true;
	}
	void Player::SetDamage(const float& damage)
	{
		m_Damage = damage;
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
		SetSpeed(4.0f);
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
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
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		ptrDraw->SetBlendState(BlendState::AlphaToCoverage);
		ptrDraw->SetOwnShadowActive(true);
		AddAnimation();
		ptrDraw->SetDiffuse(Col4(1, 0, 0, 1));
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"PLAYER");
		shadowPtr->SetMeshToTransformMatrix(meshMat);
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
		//cntlVec
		float elapsedTime = App::GetApp()->GetElapsedTime();

		UpdateAnim();
		float spped = 0.0f;
		if (m_IsGoal == false)
		{
			ZoneActivation();
			//Debug();
			Vec3 forward = GetForward();

			if (m_ParryJudge)
			{
				m_ParryTime--;
				if (m_ParryTime < 0.0f)
				{
					m_ParryJudge = false;
					m_ParryTime = 15.0f;
				}
			}
			if (m_DamageIntervalStart)
			{
				m_DamageInterval -= elapsedTime;
				if (m_DamageInterval <= 0.0f)
				{
					m_DamageIntervalStart = false;
					m_DamageInterval = 0.5f;
				}
			}

			Vec3 rot = SearchRange();

			if ((m_PlayerStateNum & PlayerState::DASH) != 0)
			{
				m_BoostTime -= elapsedTime;
				if (m_BoostTime >= 0.0f)
				{
					SetAnim(L"Brink");
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
					SetAnim(m_AttackAnim);

				}
				else
				{
					m_PlayerStateNum -= PlayerState::ATTACK;
					m_PlayerStateNum += PlayerState::NORMAL;
				}
			}
			else {
				m_BoostTime = 0.2f;
				m_Attacktime = 0.25f;

				m_BoostInterval -= elapsedTime;
				m_AttackInterval -= elapsedTime;
				MovePlayer(6.0f);

				Vec3 rot = SearchRange();
				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X)
				{
					m_BoostAngle = GetForward();
					if (m_BoostInterval <= 0.0f)
					{
						float rotate = atan2f(m_BoostAngle.x, m_BoostAngle.z);
						m_Effect->PlayEffect(m_BrinkHandle, L"Brick", GetPosition(), 0.0f);
						m_Effect->SetRotation(m_BrinkHandle, Vec3(0, 1, 0), rotate);

						m_PlayerStateNum -= PlayerState::NORMAL;
						m_PlayerStateNum += PlayerState::DASH;
						SoundManager::Instance().PlaySE(L"SE_ACCEPT");
					}
				}

				if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
				{
					if (m_AttackAnim == L"Attack2")
					{
						m_AttackAnim = L"Attack";
					}
					else
					{
						m_AttackAnim = L"Attack2";
					}

					m_ParryJudge = true;
					m_ParryTime = 15.0f;
					AimRock(rot);
					m_Position = GetPosition();
					m_Stage->AddGameObject<HitSphere>(Vec3(m_Position), forward, GetThis<GameObject>(), m_HitScale, m_SearchDistance);
					float rotate = atan2f(forward.x, forward.z);

					m_Effect->PlayEffect(m_Handle, L"ShockWave", Vec3(m_Position.x + forward.x / 2, m_Position.y + 0.25f, m_Position.z + forward.z / 2), 0.0f);
					m_Effect->SetRotation(m_Handle, Vec3(0.0f, 1.0f, 0.0f), rotate);
					m_Effect->SetScale(m_Handle, Vec3(m_HitScale * 0.5f));

					m_PlayerStateNum += PlayerState::ATTACK;
					m_PlayerStateNum -= PlayerState::NORMAL;

					SoundManager::Instance().PlaySE(L"SE_ATTACK_VOICE", 1.0f);
				}
			}
		}
		else
		{
			if (m_HP <= 0)
			{
				SetAnim(L"Died");
			}
			else {
				SetAnim(L"Idle");
			}
		}
	}

	void Player::OnDraw()
	{
		Character::OnDraw();
	}

	void Player::Dead() {
		PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"DeadPlayer");
	}

	bool Player::Damage(bool parry, float damage, const shared_ptr<GameObject> sorce)
	{
		bool isPinch = false, isBeforePinch = true;
		if (m_DamageIntervalStart == false)
		{
			if (m_HP >= m_MaxHP / 3.0f) {
				isBeforePinch = false;
			}
			if (m_ParryJudge)
			{
				Vec3 rot = SearchRange();
				float parryDamage = Parry(damage, m_ParryTime);
				if (parryDamage < damage) {
					if (sorce && sorce->FindTag(L"Attack")) {
						auto attack = static_pointer_cast<Attack>(sorce);
						attack->ReflectParry(GetPosition());
					}
				}
				if (parryDamage == 0 && rot != Vec3())
				{
					parry = m_ParryJudge;
					return true;
				}
				Character::Damage(parryDamage, true);
				ScoreManager::Instance()->AddDamage(parryDamage);
				m_ParryTime = false;
				return false;
			}
			else {
				SetAnim(L"Nock");
				SoundManager::Instance().PlaySE(L"SE_HIT_PLAYER");
				Character::Damage(damage, true);
				ScoreManager::Instance()->AddDamage(damage);
				m_ParryTime = false;
				return false;
			}
			if (!isBeforePinch && m_HP < m_MaxHP / 3.0f) {
				PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"PinchPlayer");
			}
		}
		m_HP = max(m_HP, 0);

	}

	void Player::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
	}

	HitSphere::HitSphere(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& forward, const shared_ptr<GameObject> player, const Vec3 scale,const float& length) :
		Object(stage),
		m_HitPosition(position),
		m_HitRotation(forward),
		m_Player(player),
		m_HitScale(scale),
		m_FlyingTime(0),
		m_TotalTime(0.0f),
		m_Speed(0.0f),
		m_Length(length)
	{
	}

	HitSphere::~HitSphere()
	{
		m_Effect->StopEffect(m_Handle);
	}

	void HitSphere::OnCreate()
	{
		auto ptr = AddComponent<Transform>();
		ptr->SetPosition(m_HitPosition);
		ptr->SetRotation(m_HitRotation);
		ptr->SetScale(m_HitScale);

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
		ptrColl->SetFixed(false);
		ptrColl->SetAfterCollision(AfterCollision::None);

		AddTag(L"HitJudge");

		auto stage = static_pointer_cast<GameStage>(GetStage());
		if (stage != nullptr) {
			m_Effect = stage->GetCreateEffect();
		}
		else {
			m_Effect = nullptr;
		}
		auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();

		if ((state & Player::PlayerState::ZONE) == 0) {

			m_FlyingTime = 0.1f;
		}
		else {
			m_Effect->PlayEffect(m_Handle, L"Panchi", ptr->GetPosition(), 20.0f);
			float rotate = atan2f(m_HitRotation.x, m_HitRotation.z);
			m_Effect->SetRotation(m_Handle, Vec3(0, 1, 0), rotate);
			m_Effect->SetScale(m_Handle, m_HitScale + 0.5f);

			m_FlyingTime = 0.5f;
		}
		m_Speed = m_Length / m_FlyingTime;
	}

	void HitSphere::OnUpdate()
	{
		float elapsedTime = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
		Vec3 hitPosition = GetComponent<Transform>()->GetPosition();
		if (m_FlyingTime > m_TotalTime)
		{
			hitPosition += m_Speed * m_HitRotation * elapsedTime;
			f += (m_Speed * m_HitRotation * elapsedTime).length();
		}
		else {
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
		m_Effect->SetLocation(m_Handle, hitPosition);
		GetComponent<Transform>()->SetPosition(hitPosition);
		m_TotalTime += elapsedTime;
	}

	void HitSphere::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"Enemy"))
		{
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			auto enemy = dynamic_pointer_cast<Character>(other);
			float rot = atan2f(enemy->GetRotation().x, enemy->GetRotation().z);
			player->SetCharge(0.1f);
			enemy->Damage(player->GetDamage(), false);

			m_Effect->PlayEffect(m_HitHandle, L"HitEffect", enemy->GetPosition(), 0.0f);
			m_Effect->SetRotation(m_HitHandle, Vec3(0, 1, 0), rot);

			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = 65535 * 0.5f;
			vibration.wRightMotorSpeed = 65535 * 0.5f;
			XInputSetState(0, &vibration);

			PostEvent(0.25f, nullptr, GetStage(), L"StopVibration");
		}
	}

}
