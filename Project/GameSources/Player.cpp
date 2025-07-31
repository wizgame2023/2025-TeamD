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
		m_MoveSpeed(24.0f),  
		m_EnergyCharge(0.0f),  
		m_PlayerStateNum(PlayerState::NORMAL),  
		m_ZoneTime(0.0f),  
		m_ParryTime(30.0f),  
		m_ParryJudge(false),  
		m_BoostTime(0.2f),  
		m_BulletDire(Vec3(0)),  
		m_Attacktime(0.0f),  
		m_Damage(3.0f),  
		m_DamageInterval(0.5f),  
		m_BoostInterval(0.0f),  
		m_IsGoal(false),  
		m_zoneAnim(1.0f),  
		m_HitScale(Vec3(1)),  
		m_SearchDistance(2.0),  
		m_Length(4.0f),  
		m_AttackInterval(0.0f),  
		m_BlinkingInterval(0.1f),  
		m_ParryDamageInterval(false),  
		m_ParryDamageIntervalTime(0.5f),  
		m_IsPerfectParry(false),  
		m_IsParry(false),  
		m_PerfectParrySecond(0.5f),  
		m_AttackAnim(L"Attack"),  
		m_BrinkHandle(-1),
		m_DamageIntervalStart(false), 
		m_ParryComboActive(false),
		m_ParryComboCount(0),
		m_ParryComboTimer(0.0f),
		m_ParryComboWindow(0.0f),
		m_ParryDamage(0.0f),
		m_TotalTime(0.0f),
		m_time(0.0f),
		m_RotationSpeed(10.0f),
		m_CurrentYaw(0.0f),
		m_BoostConterTime(0.5f),
		m_IsParryCounter(false),
		m_ParryCountered(false),
		m_ChargeTime(0.0f),
		m_Speed(6.0f),
		m_AttackChargeInterval(0.5f)
	{}  
	Player::‾Player(){}

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

		if (moveX + moveZ != 0 || moveX - moveZ != 0) {
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

	float XMScalarLerpAngle(float startAngle, float endAngle, float t) {
		float delta = endAngle - startAngle;
		while (delta > XM_PI) delta -= XM_2PI;
		while (delta < -XM_PI) delta += XM_2PI;
		return startAngle + delta * t;
	}

	void Player::MovePlayer(const float Speed) {
		float elapsedTime = GetElapsed();
		float targetYaw;
		auto angle = GetMoveVector(targetYaw);
		SetSpeed(Speed);
		if (angle.length() > 0.0f) {
			m_CurrentYaw = XMScalarLerpAngle(
				m_CurrentYaw,        // 開始角度
				targetYaw,           // 目標角度
				m_RotationSpeed * elapsedTime
			);

			SetRotation(Vec3(0, m_CurrentYaw, 0));

			Move(angle, false);
			m_BulletDire = GetForward();
			SetAnim(L"Dash");
		}
		else {
			if ((m_PlayerStateNum & PlayerState::NORMAL) == 1) {
				SetAnim(L"Idle");
			}
		}
	}

	void Player::BoostMove(const float Speed, const Vec3 Angle, const float time)
	{
		float dt = GetElapsed();
		m_TotalTime += dt;

		// ダッシュ時間が終わったらリセットして終了
		if (m_TotalTime >= time) {
			m_TotalTime = 0.0f;
			// ステート解除などの後処理
			return;
		}

		// 現在位置＋水平移動量＋垂直移動量を足し込む
		Vec3 pos = GetPosition();
		Vec3 horizontal = Vec3(Angle.x, 0.0f, Angle.z) * Speed * dt;

		// Gravity コンポーネント側で毎フレーム更新された重力速度を取得
		float vy = GetComponent<Gravity>()->GetGravityVelocity().y;
		Vec3 vertical = Vec3(0.0f, vy, 0.0f) * dt;

		SetPosition(pos + horizontal + vertical);
	}

	void Player::ZoneActivation(){
		float elapsedTime = App::GetApp()->GetElapsedTime()* GameManager::Instance()->GetGameSpeed();
		auto cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		auto keyState = App::GetApp()->GetInputDevice().GetKeyState();
		if (m_EnergyCharge >= 1.0f) {
			m_EnergyCharge = 1.0f;
		}
		if (m_EnergyCharge >= 1.0){
			if (cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_B || keyState.m_bPressedKeyTbl[VK_SPACE]) {
				if ((m_PlayerStateNum & PlayerState::ZONE) == 0){
					SetAnim(L"Zone");
					m_zoneAnim = 1.0f;
					m_HitScale = Vec3(3.0f);
					m_SearchDistance = 24.0f;
					m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));
					m_PlayerStateNum += PlayerState::ZONE;
					SoundManager::Instance().PlaySE(L"SE_USE_ULT");
					GameManager::Instance()->StartZone(5.0f);
				}
			}
		}

		if ((m_PlayerStateNum & PlayerState::ZONE) != 0){
			SetAttackDamage(10.0f);
			m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));

			m_ZoneTime += elapsedTime;
			if (m_ZoneTime > 2.0f + m_zoneAnim){
				SetAttackDamage(3.0f);
				m_ZoneTime = 0;
				m_HitScale = Vec3(1.0f);
				m_EnergyCharge = 0;
				m_SearchDistance = 2.0f;
				m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));
				m_PlayerStateNum -= PlayerState::ZONE;
			}
		}
	}

	Vec3 Player::SearchRange(float angle) {
		// 前方ベクトルと自位置取得
		Vec3 forward = m_Transform->GetForward();
		Vec3 position = m_Transform->GetPosition();

		// 敵グループから最も近いオブジェクトを探索
		auto enemyGroup = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		auto targetEnemyObject = ObjectSearch(enemyGroup);
		if (!targetEnemyObject)
			return Vec3();

		// 敵の位置と自分からのベクトル
		Vec3 enemyPos = targetEnemyObject->GetComponent<Transform>()->GetPosition();
		Vec3 toEnemy = enemyPos - position;

		// 前方検出角度チェック（定数化）
		float kDetectionAngleDeg = angle;
		if (!IsWithinDetectionRange(forward, toEnemy, kDetectionAngleDeg))
			return Vec3();

		// ターゲット登録
		m_TargetBoard->SetTarget(targetEnemyObject);

		// 近距離判定（平方距離で比較）
		float kCloseDist = 10.0f;
		float sqrDistToEnemy = toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y + toEnemy.z * toEnemy.z;
		if (sqrDistToEnemy > kCloseDist * kCloseDist)
			return Vec3();

		return RotateTowardsTarget(position, enemyPos);
	}

	void Player::SetCharge(const float& charge){
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

	void Player::AimRock(Vec3 rot){
		if (rot != Vec3()){
			Vec3 dir = rot;
			dir.y = 0.0f;

			float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
			if (len < 1e-5f)
				return;     // XZベクトルが小さすぎたら回転せず抜ける

			// XZ平面上で正規化
			dir.x /= len;
			dir.z /= len;

			float yaw = atan2f(dir.x, dir.z);
			SetRotation(Vec3(0.0f, yaw, 0.0f));
		}
	}
	void Player::DrawArrow() {
		auto& trans = m_EnemyArrow->GetComponent<Transform>();

		auto enemyGroup = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		auto targetEnemyObject = ObjectSearch(enemyGroup);
		if (!targetEnemyObject) {
			m_EnemyArrow->SetDrawActive(false);
			return;
		}
		Vec3 pos = GetPosition();
		Vec3 enemyPos = targetEnemyObject->GetComponent<Transform>()->GetPosition();

		auto camera = OnGetDrawCamera();
		m_EnemyArrow->SetDrawActive(true);
		if (camera->CalcViewInPosition(enemyPos)) {
			m_EnemyArrow->SetDrawActive(false);
			return;
		}
		Vec3 toEnemy = enemyPos - pos;
		toEnemy = toEnemy.normalize();

		pos += toEnemy * 2.0f;
		pos.y = 0.51f;
		trans->SetPosition(pos);

		float angle = atan2f(toEnemy.x, toEnemy.z);

		Quat qx = Quat(sin(XMConvertToRadians(90) / 2.0f), 0.0f, 0.0f, cos(XMConvertToRadians(90) / 2.0f));
		Quat qy = Quat(0.0f, sin(angle / 2.0f), 0.0f, cos(angle / 2.0f));

		trans->SetQuaternion(qx * qy);
	}

	shared_ptr<GameObject> Player::ObjectSearch(const shared_ptr<GameObjectGroup>& group) {
		const auto& list = group->GetGroupVectors();
		Vec3 position = m_Transform->GetPosition();

		shared_ptr<GameObject> nearest = nullptr;
		float bestSqrDist = std::numeric_limits<float>::infinity();

		for (auto& weakObj : list) {
			if (auto obj = weakObj.lock()) {
				if (!obj->GetDrawActive()) continue;
				auto  chara = dynamic_pointer_cast<Character>(obj);
				if (chara) {
					if (!chara->IsArive()) continue;
				}

				Vec3 objPos = obj->GetComponent<Transform>()->GetPosition();
				Vec3 diff = objPos - position;
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				if (sqrDist < bestSqrDist) {
					bestSqrDist = sqrDist;
					nearest = obj;
				}
			}
		}
		return nearest;
	}

	void Player::UpdateAnim(){
		float elapsedTime = GetElapsed();
		auto draw = GetComponent<PNTBoneModelDraw>();
		draw->UpdateAnimation(elapsedTime);
	}

	float Player::Parry(float damage, const float& ParrySecond) {
		float PerfectThreshold = 20.0f;
		float GreatThreshold = 15.0f;
		float GoodThreshold = 5.0f;
		float EnergyPerfectBonus = 0.5f;
		float EnergyGreatBonus = 0.25f;
		float EnergyGoodBonus = 0.1f;

		if (ParrySecond <= GoodThreshold) {
			return damage;
		}

		float reducedDamage = damage;
		float energyBonus = 0.0f;
		bool  needEffect = false;
		bool  isPerfect = false;

		if (ParrySecond > PerfectThreshold) {
			reducedDamage = 0.0f;
			energyBonus = EnergyPerfectBonus;
			needEffect = true;
			isPerfect = true;
		}
		else if (ParrySecond > GreatThreshold) {
			reducedDamage = damage * 0.25f;
			energyBonus = EnergyGreatBonus;
			needEffect = true;
		}
		else {
			reducedDamage = damage * 0.5f;
			energyBonus = EnergyGoodBonus;
		}

		m_EnergyCharge += energyBonus;
		m_ParryDamageInterval = needEffect;

		if (needEffect) {
			m_Effect->PlayEffect(m_ParryHandle, L"Parry", GetPosition() + GetForward(), 0.0f);
			m_Effect->SetScale(m_ParryHandle, Vec3(0.5f));
			m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f);
		}

		if (isPerfect) {
			ScoreManager::Instance()->AddParryCount();
			SoundManager::Instance().PlaySE(L"SE_GUARD");
			PostEvent(0.0f, nullptr, GetStage(), L"HitStop");
			m_IsPerfectParry = true;
			m_IsParry = true;
			m_IsParryCounter = false;
			m_ParryCountered = false;

		}

		return reducedDamage;
	}


	void Player::AddAnimation(){
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		auto anim_fps = 60.0f;
		ptrDraw->AddAnimation(L"Idle", 11, 60, true, anim_fps);
		ptrDraw->AddAnimation(L"Attack", 81, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"Attack2", 421, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"QuakeAttack", 720, 80, false, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Counter", 620, 80, false, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Zone", 151, 60, false, anim_fps * 2.5f); 
		ptrDraw->AddAnimation(L"Dash", 212, 60, true, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Brink", 270, 1, true, anim_fps);
		ptrDraw->AddAnimation(L"Nock", 281, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Died", 351, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Clear", 491, 100, false, anim_fps);
	}

	void Player::PlayAnimation()
	{
		float elapsedTime = GetElapsed();

		if (m_PlayerStateNum & PlayerState::DASH) {
			HandleDash(elapsedTime);
		}
		else if (m_PlayerStateNum & PlayerState::ATTACK) {
			HandleAttack(elapsedTime);
		}
		else if (m_PlayerStateNum & PlayerState::ATTACKCHARGE) {
			HandleAttackCharge(elapsedTime);
		}
		else {
			HandleNormal(elapsedTime);
		}
	}

	void Player::HandleDash(const float& elapsedTime)
	{
		if (IntervalTimer(true, 0.2f, elapsedTime, m_BoostTime, /*Return=*/true)) {
			m_PlayerStateNum -= PlayerState::DASH;
			m_PlayerStateNum += PlayerState::NORMAL;
			m_BoostInterval = 0.5f;  // 次のダッシュ待機
		}
		else {
			SetAnim(L"Brink");
			BoostMove(m_Speed * 3.0f, m_BoostAngle, 0.2f);
		}
	}

	void Player::HandleAttack(const float& elapsedTime)
	{
		if (IntervalTimer(true, 0.15f, elapsedTime, m_Attacktime, true)) {
			m_PlayerStateNum -= PlayerState::ATTACK;
			m_PlayerStateNum += PlayerState::NORMAL;
			m_AttackInterval = 0.15f;
		}
		else {
			SetAnim(m_AttackAnim);
		}
	}

	void Player::HandleAttackCharge(const float& elapsedTime)
	{
		if (IntervalTimer(true, 0.5f, elapsedTime, m_AttackChargeInterval, true)) {
			m_PlayerStateNum -= PlayerState::ATTACKCHARGE;
			m_PlayerStateNum += PlayerState::NORMAL;
			m_AttackChaegetime = 0.5f;
		}
		else {
			SetAnim(L"QuakeAttack", true);
		}
	}

	void Player::HandleNormal(const float& elapsedTime)
	{
		auto& keyState = App::GetApp()->GetInputDevice().GetKeyState();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		Vec3 forward = GetForward();


		bool isBoost = IntervalTimer(true, 0.5f, elapsedTime, m_BoostInterval, false);
		bool isAttack = IntervalTimer(true, 0.2f, elapsedTime, m_AttackInterval, false);
		bool isAttackChaege = IntervalTimer(true, 0.5f, elapsedTime, m_AttackChaegetime, false);

		Vec3 rot = SearchRange(90.0f);
		if ((cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X || keyState.m_bPressedKeyTbl[VK_RBUTTON]) && isBoost) {
			m_BoostAngle = GetForward();
			float rotate = atan2f(m_BoostAngle.x, m_BoostAngle.z);
			m_Effect->PlayEffect(m_BrinkHandle, L"Brick", GetPosition(), 0.0f);
			m_Effect->SetRotation(m_BrinkHandle, Vec3(0, 1, 0), rotate);

			m_PlayerStateNum -= PlayerState::NORMAL;
			m_PlayerStateNum += PlayerState::DASH;
			SoundManager::Instance().PlaySE(L"SE_ACCEPT");
		}

		if (!isAttackChaege) return;
		if (cntlVec[0].wButtons & XINPUT_GAMEPAD_A || keyState.m_bPushKeyTbl[VK_LBUTTON])
		{
			m_ChargeTime += elapsedTime;
			m_Speed = 6.0f / 2.0f; // チャージ中は移動速度を下げる
			if(m_ChargeTime >= 5.0f) {
				m_ChargeTime = 5.0f; // 最大チャージ時間を制限
			}
		}
		
		MovePlayer(m_Speed);

		if (cntlVec[0].wReleasedButtons & XINPUT_GAMEPAD_A || keyState.m_bUpKeyTbl[VK_LBUTTON]) {
			if (isAttack)
			{
				if (m_ChargeTime >= 1.5f) {
					m_TargetObject = Vec3();
					m_Stage->AddGameObject<ChargeHitSphere>(GetPosition(), Vec3(1.0f), GetThis<GameObject>(), m_ChargeTime);

					m_Effect->PlayEffect(m_EarthQuakeHandle, L"EarthQuake", GetPosition(), 0.0f);
					m_Effect->SetLocation(m_EarthQuakeHandle, Vec3(GetPosition().x, GetPosition().y - 0.25f, GetPosition().z));
					m_Effect->SetScale(m_EarthQuakeHandle, Vec3(m_ChargeTime));
					auto camera = GetStage()->GetView()->GetTargetCamera();;
					camera->ShakeStart(0.5f, 0.3f);

					m_PlayerStateNum += PlayerState::ATTACKCHARGE;
					m_PlayerStateNum -= PlayerState::NORMAL;
					SoundManager::Instance().PlaySE(L"SE_CHARGE_ATTACK");
				}
				else {
					if (m_AttackAnim == L"Attack2") {
						m_AttackAnim = L"Attack";
					}
					else {
						m_AttackAnim = L"Attack2";
					}
					m_TargetObject = Vec3();

					m_ParryJudge = true;
					m_ParryTime = 30.0f;
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
			m_ChargeTime = 0;
			m_Speed = 6.0f;
		}
	}
	
	void Player::Blinking(){
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		auto state = ptrDraw->GetBlendState();
		float elapsedTime = GetElapsed();
		if (IntervalTimer(true, 0.1f, elapsedTime, m_BlinkingInterval, true)){
			if (state == BlendState::AlphaToCoverage){
				ptrDraw->SetBlendState(BlendState::Additive);
			}
			else{
				ptrDraw->SetBlendState(BlendState::AlphaToCoverage);
			}
		}
	}

	bool Player::IntervalTimer(const bool& TimerStart, const float& MaxTimer, const float& frame, float& Timer, const bool& Return)
	{
		if (TimerStart) {
			if (Timer <= 0.0f) {
				if(Return) Timer = MaxTimer;
				return true;
			}
			else {
				Timer -= frame;
			}
		}
		return false;
	}

	void Player::IntervalManagement()
	{
		float elapsedTime = GetElapsed();
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		if (IntervalTimer(m_ParryJudge,15.0f, 1.0f ,m_ParryTime, true)){
			m_ParryJudge = false;
		}

		if (IntervalTimer(m_ParryDamageInterval,0.75f, elapsedTime, m_ParryDamageIntervalTime, true)){
			m_ParryDamageInterval = false;
		}

		if (IntervalTimer(m_DamageIntervalStart, 0.5f, elapsedTime, m_DamageInterval, true)) {
			m_DamageIntervalStart = false;
			ptrDraw->SetBlendState(BlendState::AlphaToCoverage);
		}
		else if(m_DamageIntervalStart) {
			Blinking();
		}

		if (IntervalTimer(m_ParryComboActive, m_ParryComboWindow,elapsedTime,m_ParryComboTimer,false)){
			// 猶予切れ
			m_ParryComboActive = false;
			m_ParryComboCount = 0;
		}

		if (IntervalTimer(m_IsPerfectParry, 0.25f, elapsedTime, m_PerfectParrySecond, false)){
			m_IsPerfectParry = false;
			m_IsParry = false;
			m_PerfectParrySecond = 1.0f;
		}
		else{
			HandlePerfectParryInput();
		}

		if (IntervalTimer(m_IsParryCounter, 0.5f, elapsedTime, m_BoostConterTime, false)) {
			m_IsParryCounter = false;
		}

		if (m_IsParryCounter)
		{
			Vec3 playerPos = GetPosition();
			Vec3 dir = Vec3(m_TargetObject.x, 0.0f,m_TargetObject.z);
			dir.normalize();
			SetAnim(L"Counter", true);

			BoostMove(m_Speed * 3.5f, dir * 1.2f, 0.5f);
		}
	}

	void Player::HandlePerfectParryInput()
	{
		if (m_ParryCountered) return;
		auto& keyState = App::GetApp()->GetInputDevice().GetKeyState();
		auto& cntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		Vec3 forward = GetForward();
		if ((cntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A || keyState.m_bPressedKeyTbl[VK_LBUTTON] ) && m_TargetObject != Vec3()) {
			m_IsParryCounter = true;
			m_BoostConterTime = 0.5f;
			m_ParryCountered = true;
			GetStage()->AddGameObject<CounterHitSphere>(GetThis<GameObject>(), Vec3(forward.x,0.0f, forward.z),m_BoostConterTime,1.0f);
		}
	}

	void Player::OnCreate(){
		Character::OnCreate();
		InitHP(20);
		SetAttackDamage(3.0f);
		SetSpeed(4.0f);
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(GameManager::Instance()->IsDebug());//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<PNTBoneModelDraw>();
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

		ptrDraw->SetDepthStencilState(DepthStencilState::Default);
		ptrDraw->SetRasterizerState(RasterizerState::DoubleDraw);
		ptrDraw->SetOwnShadowActive(false);
		//ptrDraw->SetFogEnabled(true);
		ptrDraw->SetModelDiffusePriority(true);
		AddAnimation();
		//重力をつける
		auto ptrGra = AddComponent<Gravity>();

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"PLAYER");
		shadowPtr->SetMeshToTransformMatrix(meshMat);
		AddTag(L"Player");

		m_TargetBoard = m_Stage->AddGameObject<TargetBoard>(GetThis<Player>());
		m_EnemyArrow = m_Stage->AddGameObject<Board>(L"ARROW", Vec3(0, 0.51f, 0), Vec3(1, 1, 1), false);
		auto stage = static_pointer_cast<GameStage>(m_Stage);
		if (stage != nullptr) {
			m_Effect = stage->GetCreateEffect();
		}
		else {
			m_Effect = nullptr;
		}
		m_PositionY = GetPosition().y;
		m_Stage->SetSharedGameObject(L"Player", GetThis<Player>());
	}

	void Player::OnUpdate(){
		UpdateAnim();
		DrawArrow();
		if (m_IsGoal == false){
			m_TargetBoard->SetDrawActive(true);
			ZoneActivation();
			IntervalManagement();
			PlayAnimation();
		}
		else{
			m_TargetBoard->SetDrawActive(false);
			m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));
			if (m_HP <= 0){
				SetAnim(L"Died");
			}
			else {
				SetAnim(L"Idle");
			}
		}

		if (m_EnergyCharge >= 1.0f && !m_IsCharged) {
			m_IsCharged = true;
			SoundManager::Instance().PlaySE(L"SE_ULT_CHARGED");
		}
		else if (m_EnergyCharge < 1.0f) {
			m_IsCharged = false;
		}
		m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f * GameManager::Instance()->GetGameSpeed());
		m_Effect->SetEffectSpeed(m_Handle, 1.0f * GameManager::Instance()->GetGameSpeed()); 
		m_Effect->SetEffectSpeed(m_BrinkHandle, 1.0f * GameManager::Instance()->GetGameSpeed());
	}

	void Player::OnDraw(){
		Character::OnDraw();
	}

	void Player::Dead() {
		PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"DeadPlayer");
	}

	bool Player::Damage(bool parry, float damage, const shared_ptr<GameObject> source){
		if (m_DamageIntervalStart) return false;
		bool wasHighHP = (m_HP >= m_MaxHP / 3.0f);
		// ピンチ演出（到達しないなら不要）
		if (wasHighHP && m_HP < m_MaxHP / 3.0f) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"PinchPlayer");
		}

		if (m_ParryJudge) {
			float parryDamage = Parry(damage, m_ParryTime);
			Vec3 rot = source->GetComponent<Transform>()->GetPosition() - GetPosition();
			// 完璧パリィ（0ダメージ＆有効範囲内）の場合
			if (parryDamage == 0.0f && rot != Vec3()) {
				if (source && (source->FindTag(L"Attack"))) {
					auto attack = dynamic_pointer_cast<Attack>(source);
					if (attack) {
						auto boss = attack->GetDete();
						rot = boss->GetPosition() - GetPosition();
						attack->ReflectParry(GetPosition());
						auto camera = GetStage()->GetView()->GetTargetCamera();;
						auto get = dynamic_pointer_cast<FollowCamera>(camera);
						get->SetShaking(true);
					}
					auto gravity = GetComponent<Gravity>(false);
					if (gravity) {
						gravity->SetGravityVerocityZero();
					}
				}
				m_TargetObject = Vec3(rot.x,0, rot.z);
				AimRock(rot);
				++m_ParryComboCount;                // コンボ回数増加
				m_ParryComboActive = true;          // 猶予タイマー開始
				m_ParryComboTimer = m_ParryComboWindow;
				// m_ParryJudge はクリアせずそのまま → 連続判定可能

				return true;
			}

			// それ以外のパリィ――判定終了
			m_ParryJudge = false;

			if (!m_ParryDamageInterval) {
				if (damage != parryDamage){
					Character::Damage(parryDamage, false);
					ScoreManager::Instance()->AddDamage(parryDamage);
				}
				else {
					// 通常被弾
					SetAnim(L"Nock");
					m_DamageIntervalStart = true;
					SoundManager::Instance().PlaySE(L"SE_HIT_PLAYER");
					Character::Damage(damage, true);
					ScoreManager::Instance()->AddDamage(damage);
				}
			}
			return false;
		}
		else {
			if (!m_ParryDamageInterval){
				// 通常被弾
				SetAnim(L"Nock");
				m_DamageIntervalStart = true;
				SoundManager::Instance().PlaySE(L"SE_HIT_PLAYER");
				Character::Damage(damage, true);
				ScoreManager::Instance()->AddDamage(damage);
				return false;
			}
		}
		return false;
	}

	void Player::Debug() {
		auto scene = App::GetApp()->GetScene<Scene>();
		wstringstream wss(L"");
		wss << L"¥nZoneCharge : "
			<< m_EnergyCharge
			<< L"¥nHP"
			<< m_HP
			<< L"¥nx"
			<< GetPosition().x
			<< L"¥ny"
			<< GetPosition().y
			<< L"¥nz"
			<< GetPosition().z
			<< L"¥nY"
			<< m_PositionY
			<< endl;
		scene->SetDebugString(wss.str());
	}

	Vec3 Player::GetForward() {
		return m_Transform->GetForward();
	}
	int Player::GetStates() {
		return m_PlayerStateNum;
	}
	float Player::GetEnergy() {
		return m_EnergyCharge;
	}
	float Player::GetDamage() {
		return m_Damage;
	}
	bool Player::GetParry() {
		return m_ParryJudge;
	}
	bool Player::IsAttack() {
		return IntervalTimer(true, 0.2f, 0.0f, m_AttackInterval, false) && !(m_PlayerStateNum & PlayerState::ATTACK);
	}
	bool Player::IsDash() {
		return IntervalTimer(true, 0.5f, 0.0f, m_BoostInterval, false) && !(m_PlayerStateNum & PlayerState::DASH);
	}
	void Player::SetParryPosition(const Vec3& position) {
		m_EffectVec = position;
	}
	void Player::SetDamage(const float& damage) {
		m_Damage = damage;
	}
	void Player::SetIsGaol(const bool& goal) {
		m_IsGoal = goal;
	}
}
