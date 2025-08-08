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
		m_MoveMent{6.0f, 0.0f, 10.0f, 2.0f, Vec3()},
		m_Zone{0.0f, 0.0f, 2.5f, 1.0f},
		m_Parry{30.0f ,0.5f, 0.5f, false, false, false, false},
		m_Boost{0.2f, 0.5f, 0.0f},
		m_Attack{0.0f, 0.5f, 0.0f, 0.0f, 0.2f, L"Attack"},
		m_Damage{0.5f, 0.1f, 3.0f, false},
		m_Combo{false, 0.0f, 0.0f, 0.0f},
		m_PlayerStateNum(PlayerState::NORMAL),
		m_IsGoal(false),  
		m_HitScale(Vec3(1)),  
		m_TotalTime(0.0f),
		m_IsParryCounter(false),
		m_IsCharged(false),
		m_Handle(-1),
		m_BrinkHandle(-1),
		m_EarthQuakeHandle(-1)
	{}  

	Player::‾Player(){}

	Vec3 Player::GetMoveVector(float& rot) {
		Vec3 angle(0, 0, 0);
		auto in = InputReader::Get().Read();
		//入力の取得
		float moveX = in.moveX;
		float moveZ = in.moveY;

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
		// 1) 入力取得
		auto in = InputReader::Get().Read();

		// 2) 移動ベクトル／回転角の算出
		Vec3 moveDir(0, 0, 0);
		float targetYaw = 0.0f;
		auto angle = GetMoveVector(targetYaw);

		if (angle.length() > 0.0f) {
			SetSpeed(m_MoveMent.MoveSpeed);

			// 3) 回転補間＆移動
			float dt = GetElapsed();
			m_MoveMent.CurrentYaw = XMScalarLerpAngle(
				m_MoveMent.CurrentYaw,
				targetYaw,
				m_MoveMent.RotationSpeed * dt
			);
			SetRotation(Vec3(0, m_MoveMent.CurrentYaw, 0));

			Move(angle, false);
			SetAnim(L"Dash");
		}
		else if ((m_PlayerStateNum & PlayerState::NORMAL) == PlayerState::NORMAL) {
			SetAnim(L"Idle");
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

	void Player::ZoneActivation() {
		// 1) 入力読み込み ＆ 経過時間取得
		InputState in = InputReader::Get().Read();
		float elapsedTime = App::GetApp()->GetElapsedTime()
			* GameManager::GetInstance().GetGameSpeed();

		// 2) エネルギー制限
		if (m_Zone.EnergyCharge > 1.0f) {
			m_Zone.EnergyCharge = 1.0f;
		}

		// 3) 発動判定
		if (in.zonePressed && m_Zone.EnergyCharge >= 1.0f && !(m_PlayerStateNum & PlayerState::ZONE)) {
			SetAnim(L"Zone");
			m_Zone.ZoneAnim = 1.0f;
			m_HitScale = Vec3(3.0f);
			m_MoveMent.SearchDistance = 24.0f;
			m_Attack.MaxInterval /= 2;
			m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));
			m_PlayerStateNum |= PlayerState::ZONE;
			SoundManager::GetInstance().PlaySE(L"SE_USE_ULT");
			GameManager::GetInstance().StartZone(5.0f);
		}

		// 4) ZONE 中の持続処理
		if (m_PlayerStateNum & PlayerState::ZONE) {
			SetAttackDamage(10.0f);
			m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 1, 1));

			m_Zone.ZoneTime += elapsedTime;
			if (m_Zone.ZoneTime > 2.0f + m_Zone.ZoneAnim) {
				// リセット
				SetAttackDamage(3.0f);
				m_Zone.ZoneTime = 0.0f;
				m_Zone.EnergyCharge = 0.0f;
				m_HitScale = Vec3(1.0f);
				m_MoveMent.SearchDistance = 2.0f;
				m_Attack.MaxInterval = 0.2f;
				m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));
				m_PlayerStateNum &= ‾PlayerState::ZONE;
			}
		}
	}

	void Player::PlayAnimation()
	{
		// フレーム経過時間を取得
		float elapsedTime = GetElapsed();

		// プレイヤー状態に応じて適切なハンドラを呼び出し
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
		// インターバルが0.2秒経過したらダッシュ状態を解除して通常状態へ
		if (IntervalTimer(true, 0.2f, elapsedTime, m_Boost.Duration, /*Return=*/true)) {
			m_PlayerStateNum &= ‾PlayerState::DASH;
			m_PlayerStateNum |= PlayerState::NORMAL;
			m_Boost.Cooldown = 0.5f;  // 次回ダッシュまでの待機時間をセット
		}
		else {
			// ダッシュ中のアニメーションと移動処理
			SetAnim(L"Brink");
			BoostMove(m_MoveMent.MoveSpeed * 3.0f, m_MoveMent.BoostAngle, 0.2f);
		}
	}

	void Player::HandleAttack(const float& elapsedTime)
	{
		// 攻撃動作にかける時間(0.15秒)が経過したら通常状態へ
		if (IntervalTimer(true, 0.15f, elapsedTime, m_Attack.AnimTime, /*Return=*/true)) {
			m_PlayerStateNum &= ‾PlayerState::ATTACK;
			m_PlayerStateNum |= PlayerState::NORMAL;
			m_Attack.Interval = 0.15f;  // 攻撃再発動までの待機時間
		}
		else {
			// 攻撃アニメーションを再生
			SetAnim(m_Attack.AnimName);
		}
	}

	void Player::HandleAttackCharge(const float& elapsedTime)
	{
		// チャージ攻撃動作にかける時間(0.5秒)が経過したら通常状態へ
		if (IntervalTimer(true, 0.5f, elapsedTime, m_Attack.ChargeInterval, /*Return=*/true)) {
			m_PlayerStateNum &= ‾PlayerState::ATTACKCHARGE;
			m_PlayerStateNum |= PlayerState::NORMAL;
			m_Attack.ChargeTime = 0.5f;  // 次のチャージ攻撃待機時間
		}
		else {
			// チャージ攻撃アニメーション（ループ再生）
			SetAnim(L"QuakeAttack", /*loop=*/true);
		}
	}

	void Player::HandleNormal(const float& elapsedTime)
	{
		// 1) 入力取得
		InputState in = InputReader::Get().Read();

		// 2) タイマー判定
		bool isBoost = IntervalTimer(true, 0.5f, elapsedTime, m_Boost.Cooldown, false);
		bool isAttackAvail = IntervalTimer(true, m_Attack.MaxInterval, elapsedTime, m_Attack.Interval, false);
		bool isChargeAvail = IntervalTimer(true, 0.5f, elapsedTime, m_Attack.ChargeTime, false);

		Vec3 forward = GetForward();

		// 3) ダッシュ入力
		if (in.dashPressed && isBoost) {
			m_MoveMent.BoostAngle = forward;
			float rotY = atan2f(forward.x, forward.z);
			m_Effect->PlayEffect(m_BrinkHandle, L"Brick", GetPosition(), 0.0f);
			m_Effect->SetRotation(m_BrinkHandle, Vec3(0, 1, 0), rotY);

			m_PlayerStateNum &= ‾PlayerState::NORMAL;
			m_PlayerStateNum |= PlayerState::DASH;
			SoundManager::GetInstance().PlaySE(L"SE_ACCEPT");
		}

		// 4) チャージ攻撃入力（ホールド中は移動速度ダウン）
		if (!isChargeAvail) return;
		if (in.attackHeld) {
			m_Combo.ChargeTime += elapsedTime;
			m_MoveMent.MoveSpeed = 6.0f / 2.0f;
			m_Combo.ChargeTime = (m_Combo.ChargeTime < 5.0f) ? m_Combo.ChargeTime : 5.0f;
		}

		// 5) 通常移動
		MovePlayer(m_MoveMent.MoveSpeed);

		// 6) 攻撃ボタン離した瞬間
		if (in.attackReleased && isAttackAvail) {
			// フルチャージ攻撃
			if (m_Combo.ChargeTime >= 1.5f) {
				// フルチャージ攻撃の発動
				m_TargetObject = Vec3();
				m_Stage->AddGameObject<ChargeHitSphere>(
					GetPosition(), Vec3(1.0f), GetThis<GameObject>(), m_Combo.ChargeTime);

				// エフェクト／カメラ揺れ
				m_Effect->PlayEffect(m_EarthQuakeHandle, L"EarthQuake", GetPosition(), 0.0f);
				m_Effect->SetLocation(m_EarthQuakeHandle,
					Vec3(GetPosition().x, GetPosition().y - 0.25f, GetPosition().z));
				m_Effect->SetScale(m_EarthQuakeHandle, Vec3(m_Combo.ChargeTime));
				GetStage()->GetView()->GetTargetCamera()->ShakeStart(0.5f, 0.3f);
				m_PlayerStateNum |= PlayerState::ATTACKCHARGE;
				m_PlayerStateNum &= ‾PlayerState::NORMAL;
				SoundManager::GetInstance().PlaySE(L"SE_CHARGE_ATTACK");
			}
			else {
				//アニメーションを切り替え
				m_Attack.AnimName = (m_Attack.AnimName == L"Attack2") ? L"Attack" : L"Attack2";
				m_TargetObject = Vec3();

				// パリィ可能時間の開始
				m_Parry.IsJudgeActive = true;
				m_Parry.JudgeTime = 30.0f;

				// 敵狙いと当たり判定
				Vec3 rot = SearchRange(90.0f);
				AimRock(rot);
				m_Position = GetPosition();
				m_Stage->AddGameObject<HitSphere>(m_Position, forward, GetThis<GameObject>(), m_HitScale, m_MoveMent.SearchDistance);

				// ショックウェーブエフェクト
				float fwdRot = atan2f(forward.x, forward.z);
				m_Effect->PlayEffect(m_Handle, L"ShockWave",
					Vec3(m_Position.x + forward.x / 2, m_Position.y + 0.25f, m_Position.z + forward.z / 2), 0.0f);
				m_Effect->SetRotation(m_Handle, Vec3(0, 1, 0), fwdRot);
				m_Effect->SetScale(m_Handle, Vec3(m_HitScale * 0.5f));

				// ... （HitSphere 登録／エフェクトなど）
				m_PlayerStateNum |= PlayerState::ATTACK;
				m_PlayerStateNum &= ‾PlayerState::NORMAL;
				SoundManager::GetInstance().PlaySE(L"SE_ATTACK_VOICE", 1.0f);
			}
			// リセット
			m_Combo.ChargeTime = 0.0f;
			m_MoveMent.MoveSpeed = 6.0f;
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
		m_Zone.EnergyCharge += charge;
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
		// グループ内の全オブジェクトを取得
		const auto& list = group->GetGroupVectors();
		Vec3 position = m_Transform->GetPosition();

		shared_ptr<GameObject> nearest = nullptr;
		float bestSqrDist = std::numeric_limits<float>::infinity();

		// 各オブジェクトについて有効性と距離をチェック
		for (auto& weakObj : list) {
			if (auto obj = weakObj.lock()) {
				if (!obj->GetDrawActive()) continue;    // 描画非アクティブは無視

				// Character 型の場合、生存判定も行う
				auto chara = dynamic_pointer_cast<Character>(obj);
				if (chara && !chara->IsArive()) continue;

				// プレイヤーとの距離の二乗を計算
				Vec3 objPos = obj->GetComponent<Transform>()->GetPosition();
				Vec3 diff = objPos - position;
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				// 最も近いオブジェクトを更新
				if (sqrDist < bestSqrDist) {
					bestSqrDist = sqrDist;
					nearest = obj;
				}
			}
		}

		return nearest; // 発見した最も近いオブジェクトを返却
	}

	void Player::UpdateAnim() {
		// 前フレームからの経過時間を取得し、アニメーションを更新
		float elapsedTime = GetElapsed();
		auto draw = GetComponent<PNTBoneModelDraw>();
		draw->UpdateAnimation(elapsedTime);
	}

	void Player::AddAnimation(){
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		auto anim_fps = 60.0f;
		ptrDraw->AddAnimation(L"Idle", 11, 60, true, anim_fps);
		ptrDraw->AddAnimation(L"Attack", 81, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"Attack2", 421, 60, false, anim_fps * 2.5f);
		ptrDraw->AddAnimation(L"QuakeAttack", 720, 80, false, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Counter", 620, 80, false, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Zone", 151, 60, false, anim_fps * m_Zone.ZoneAnimProgress);
		ptrDraw->AddAnimation(L"Dash", 212, 60, true, anim_fps * 1.5f);
		ptrDraw->AddAnimation(L"Brink", 270, 1, true, anim_fps);
		ptrDraw->AddAnimation(L"Nock", 281, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Died", 351, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Clear", 491, 100, false, anim_fps);
	}


	void Player::Blinking(){
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		auto state = ptrDraw->GetBlendState();
		float elapsedTime = GetElapsed();
		if (IntervalTimer(true, 0.1f, elapsedTime, m_Damage.BlinkingInterval, true)){
			if (state == BlendState::AlphaToCoverage){
				ptrDraw->SetBlendState(BlendState::Additive);
			}
			else{
				ptrDraw->SetBlendState(BlendState::AlphaToCoverage);
			}
		}
	}

	bool Player::IntervalTimer(const bool& TimerStart,const float& MaxTimer,const float& frame,float& Timer,const bool& Return) {
		if (TimerStart) {
			// タイマー未起動 or カウントダウン完了
			if (Timer <= 0.0f) {
				// 完了時にタイマーをリセットする場合
				if (Return) {
					Timer = MaxTimer;
				}
				return true;
			}
			else {
				// まだ時間が残っている場合は減算
				Timer -= frame;
			}
		}
		// TimerStart が false の場合やカウント中は false
		return false;
	}

	// 各種状態やエフェクトの時間管理を一括で行う
	void Player::IntervalManagement()
	{
		float elapsedTime = GetElapsed();
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();

		// パリィ判定時間 (m_ParryTime) のカウントダウン
		if (IntervalTimer(m_Parry.IsJudgeActive, 15.0f, 1.0f, m_Parry.JudgeTime, true)) {
			// パリィ猶予終了時
			m_Parry.IsJudgeActive = false;
		}

		// パリィヒット演出の持続時間管理
		if (IntervalTimer(m_Parry.IsInDamageInterval, 0.75f, elapsedTime, m_Parry.DamageInterval, true)) {
			// エフェクト終了
			m_Parry.IsInDamageInterval = false;
		}

		// ダメージ無敵時間の管理
		if (IntervalTimer(m_Damage.IsInvincible, 0.5f, elapsedTime, m_Damage.InvincibleDuration, true)) {
			// 無敵時間終了
			m_Damage.IsInvincible = false;
			ptrDraw->SetBlendState(BlendState::AlphaToCoverage); // ブレンド状態を戻す
		}
		else if (m_Damage.IsInvincible) {
			// 無敵時間中は点滅演出
			Blinking();
		}

		// パリィコンボ猶予時間の管理
		if (IntervalTimer(m_Combo.IsActive, m_Combo.WindowAfterPerfect, elapsedTime, m_Combo.Timer, false)) {
			// 猶予切れ
			m_Combo.IsActive = false;
		}

		// 完璧パリィ判定時間の管理
		if (IntervalTimer(m_Parry.IsInDamageInterval, 0.25f, elapsedTime, m_Parry.PerfectWindow, false)) {
			// 判定時間切れ
			m_Parry.IsInDamageInterval = false;
			m_Parry.PerfectWindow = 1.0f; // 次回判定時間の初期化
		}
		else {
			// 判定時間中はパリィカウンター入力を処理
			HandlePerfectParryInput();
		}

		// パリィカウンター移動時間の管理
		if (IntervalTimer(m_IsParryCounter, 0.5f, elapsedTime, m_Boost.CounterTime, false)) {
			// カウンター移動終了
			m_IsParryCounter = false;
		}

		// パリィカウンター移動中の挙動
		if (m_IsParryCounter) {
			Vec3 playerPos = GetPosition();
			// カウンター移動方向 (ターゲット方向ベース)
			Vec3 dir = Vec3(m_TargetObject.x, 0.0f, m_TargetObject.z);
			dir.normalize();

			SetAnim(L"Counter", true);           // カウンターアニメーション
			// 強力なブースト移動
			BoostMove(m_Speed * 3.5f, dir * 1.2f, 0.5f);
		}
	}

	void Player::HandlePerfectParryInput()
	{
		if (m_Parry.HasCountered) return;

		InputState in = InputReader::Get().Read();

		// パリィカウンター入力
		if (in.parryPressed && m_TargetObject != Vec3()) {
			m_IsParryCounter = true;
			m_Boost.CounterTime = 0.5f;
			m_Parry.HasCountered = true;
			GetStage()->AddGameObject<CounterHitSphere>(
				GetThis<GameObject>(),
				Vec3(GetForward().x, 0.0f, GetForward().z),
				m_Boost.CounterTime,
				1.0f
			);
		}
	}

	void Player::OnCreate(){
		Character::OnCreate();
		InitHP(20);
		SetAttackDamage(3.0f);
		SetSpeed(4.0f);
		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(GameManager::GetInstance().IsDebug());//debug
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
		m_Stage->SetSharedGameObject(L"Player", GetThis<Player>());
	}

	void Player::OnUpdate()
	{
		// アニメーションの状態を更新
		UpdateAnim();

		// 照準用の矢印を描画
		DrawArrow();

		// ゴール未到達時の通常プレイ処理
		if (!m_IsGoal) {
			// ターゲットボードを表示
			m_TargetBoard->SetDrawActive(true);

			// ゾーンへの進入判定などを行う
			ZoneActivation();

			// 各種タイマーや状態遷移の管理
			IntervalManagement();

			// 入力や状態に応じたアニメーション再生
			PlayAnimation();
		}
		else {
			// ターゲットボードを非表示
			m_TargetBoard->SetDrawActive(false);

			// ステージの環境光を完全に暗転
			m_Stage->GetLight()->SetAmbientLightColor(Col4(0, 0, 0, 0));

			// HP が 0 以下なら死亡モーション、それ以外はアイドル
			if (m_HP <= 0) {
				SetAnim(L"Died");
			}
			else {
				SetAnim(L"Idle");
			}
		}

		// エネルギーチャージが最大に達したら一度だけ効果音を鳴らす
		if (m_Zone.EnergyCharge >= 1.0f && !m_IsCharged) {
			m_IsCharged = true;
			SoundManager::GetInstance().PlaySE(L"SE_ULT_CHARGED");
		}
		else if (m_Zone.EnergyCharge < 1.0f) {
			// チャージが減少したらフラグをリセット
			m_IsCharged = false;
		}

		// エフェクトの再生スピードをゲーム速度に合わせて調整
		float gameSpeed = GameManager::GetInstance().GetGameSpeed();
		m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f * gameSpeed);
		m_Effect->SetEffectSpeed(m_Handle, 1.0f * gameSpeed);
		m_Effect->SetEffectSpeed(m_BrinkHandle, 1.0f * gameSpeed);
	}

	void Player::OnDraw()
	{
		// 基底クラスの描画処理を呼び出す
		Character::OnDraw();
	}

	void Player::Dead()
	{
		// 「プレイヤー死亡」イベントをステージに通知
		PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"DeadPlayer");
	}

	bool Player::Damage(bool parry, float damage, const shared_ptr<GameObject> source)
	{
		// 無敵時間中はダメージ無効
		if (m_Damage.IsInvincible)
			return false;

		// HP が最大値の 1/3 以上だったかどうか (ピンチ演出の判定用)
		bool wasHighHP = (m_HP >= m_MaxHP / 3.0f);

		// ピンチラインを割ったら一度だけ演出を発火
		if (wasHighHP && m_HP < m_MaxHP / 3.0f) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"PinchPlayer");
		}

		// パリィ判定中の挙動
		if (m_Parry.IsJudgeActive) {
			// Parry 関数でダメージ軽減を計算
			float parryDamage = Parry(damage, m_Parry.JudgeTime);

			// 攻撃元オブジェクトとの相対位置を計算
			Vec3 rot = source->GetComponent<Transform>()->GetPosition() - GetPosition();

			// 完璧パリィ判定 (ダメージ 0 ＆ 有効範囲内)
			if (parryDamage == 0.0f && rot != Vec3()) {
				if (source->FindTag(L"Attack")) {
					auto attack = dynamic_pointer_cast<Attack>(source);
					if (attack) {
						// ボス本体を取得し方向を再計算
						auto boss = attack->GetDete();
						rot = boss->GetPosition() - GetPosition();

						// 攻撃を跳ね返す
						attack->ReflectParry(GetPosition());

						// カメラをシェイク
						auto cam = GetStage()->GetView()->GetTargetCamera();
						if (auto followCam = dynamic_pointer_cast<FollowCamera>(cam)) {
							followCam->SetShaking(true);
						}

						// 重力ベロシティをゼロリセット
						if (auto gravity = GetComponent<Gravity>(false)) {
							gravity->SetGravityVerocityZero();
						}
					}
				}

				// カウンター方向を保存して岩を狙う
				m_TargetObject = Vec3(rot.x, 0, rot.z);
				AimRock(rot);

				// コンボ回数を増加・猶予タイマーを再起動
				m_Combo.IsActive = true;
				m_Combo.Timer = m_Combo.WindowAfterPerfect;

				// パリィ判定を継続可能
				return true;
			}

			// 完璧パリィ以外はパリィ判定終了
			m_Parry.IsJudgeActive = false;

			// ダメージエフェクト未再生なら実ダメージ処理
			if (!m_Parry.IsInDamageInterval) {
				if (damage != parryDamage) {
					// 軽減後のダメージを適用
					Character::Damage(parryDamage, false);
					ScoreManager::Instance()->AddDamage(parryDamage);
				}
				else {
					// 通常被ダメ演出
					SetAnim(L"Nock");
					m_Combo.IsActive = true;
					SoundManager::GetInstance().PlaySE(L"SE_HIT_PLAYER");
					Character::Damage(damage, true);
					ScoreManager::Instance()->AddDamage(damage);
				}
			}
			return false;
		}
		// パリィ判定外の通常被ダメ
		else {
			if (!m_Parry.IsInDamageInterval) {
				SetAnim(L"Nock");
				m_Combo.IsActive = true;
				SoundManager::GetInstance().PlaySE(L"SE_HIT_PLAYER");
				Character::Damage(damage, true);
				ScoreManager::Instance()->AddDamage(damage);
				return false;
			}
		}

		return false;
	}


	float Player::Parry(float damage, const float& ParrySecond) {
		// 各判定の閾値とエネルギーボーナス値
		float PerfectThreshold = 20.0f;
		float GreatThreshold = 15.0f;
		float GoodThreshold = 5.0f;
		float EnergyPerfectBonus = 0.5f;
		float EnergyGreatBonus = 0.25f;
		float EnergyGoodBonus = 0.1f;

		// Good 判定以下なら通常ダメージを適用
		if (ParrySecond <= GoodThreshold) {
			return damage;
		}

		float reducedDamage = damage;
		float energyBonus = 0.0f;
		bool needEffect = false;
		bool isPerfect = false;

		// パリィのタイミングによって軽減率とボーナスを設定
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

		// エネルギーチャージにボーナスを加算
		m_Zone.EnergyCharge += energyBonus;
		m_Parry.IsInDamageInterval = needEffect;

		// 見た目用エフェクトを再生
		if (needEffect) {
			m_Effect->PlayEffect(m_ParryHandle, L"Parry", GetPosition() + GetForward(), 0.0f);
			m_Effect->SetScale(m_ParryHandle, Vec3(0.5f));
			m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f);
		}

		// パーフェクトパリィ時の追加処理
		if (isPerfect) {
			ScoreManager::Instance()->AddParryCount();              // カウンタ数を加算
			SoundManager::GetInstance().PlaySE(L"SE_GUARD");        // ガード音を再生
			PostEvent(0.0f, nullptr, GetStage(), L"HitStop");      // ヒットストップを発生
			m_Parry.IsInDamageInterval = true;
			m_IsParryCounter = false;
			m_Parry.HasCountered = false;
		}

		return reducedDamage; // 軽減後のダメージ値を返却
	}


	void Player::Debug() {
		auto scene = App::GetApp()->GetScene<Scene>();
		wstringstream wss(L"");
		wss << L"¥nZoneCharge : "
			<< m_Zone.EnergyCharge
			<< L"¥nHP"
			<< m_HP
			<< L"¥nx"
			<< GetPosition().x
			<< L"¥ny"
			<< GetPosition().y
			<< L"¥nz"
			<< GetPosition().z
			<< endl;
		scene->SetDebugString(wss.str());
	}
}
