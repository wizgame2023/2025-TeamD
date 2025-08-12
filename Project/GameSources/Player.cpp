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
		m_Boost{0.2f, 0.5f, 0.0f, true},
		m_Attack{0.0f, 0.5f, 0.0f, 0.0f, 0.2f,true,true, L"Attack"},
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
			//カメラの取得
			float angleY = dynamic_pointer_cast<FollowCamera>(ptrCamera)->GetAngle();
			float movemove = atan2f(-moveZ, moveX);
			float fRotate = movemove - angleY - XM_PIDIV2;
			//カメラの角度と入力の角度から回転角を算出
			angle = Vec3(cos(fRotate), 0.0f, -sin(fRotate));
			float rotate = fRotate + XM_PIDIV2;
			//入力ベクトルを正規化
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
		//入力取得
		auto in = InputReader::Get().Read();

		//移動ベクトル／回転角の算出
		float targetYaw = 0.0f;
		auto angle = GetMoveVector(targetYaw);

		if (angle.length() > 0.0f) {
			SetSpeed(m_MoveMent.MoveSpeed);

			//回転補間＆移動
			float dt = GetElapsed();
			m_MoveMent.CurrentYaw = XMScalarLerpAngle(
				m_MoveMent.CurrentYaw,
				targetYaw,
				m_MoveMent.RotationSpeed * dt
			);
			SetRotation(Vec3(0, m_MoveMent.CurrentYaw, 0));
			// 移動ベクトルを正規化して移動
			Move(angle, false);
			SetAnim(L"Dash");
		}
		else if (m_PlayerStateNum & PlayerState::NORMAL) {
			// 移動していない場合はアニメーションをIdleに設定	
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

	void Player::DispatchStateTransition()
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
		 // 1) 入力取得：キーボードやコントローラからの操作状態を読み込む
		InputState in = InputReader::Get().Read();

		// 2) タイマー判定：ダッシュや攻撃チャージ用タイマーを更新
		UpdateTimers(elapsedTime);
		Vec3 forward = GetForward();  // プレイヤーの前方向ベクトルを取得

		// 3) ダッシュ入力：ダッシュキーが押され、ブーストが使用可能ならダッシュ
		if (in.dashPressed && m_Boost.IsReady) {
			TryDash(forward);
		}

		// 4) チャージ攻撃入力：攻撃ボタンをホールド中はチャージを蓄積しつつ速度ダウン
		if (!m_Attack.IsCharging) return;
		if (in.attackHeld) {
			HandleChargeHold(elapsedTime);
		}

		// 5) 通常移動：ホールドしていない間は移動速度に従って移動
		MovePlayer(m_MoveMent.MoveSpeed);

		// 6) 攻撃ボタン離した瞬間：フルチャージの判定後、攻撃を実行して状態をリセット
		if (in.attackReleased && m_Attack.IsAttack) {
			m_TargetObject = Vec3();  // 攻撃対象リセット
			if (m_Combo.ChargeTime >= 1.5f) {
				ExecuteFullChargeAttack();  // フルチャージ攻撃発動
			}
			else {
				ExecuteNormalAttack();      // 通常攻撃発動
			}
			// 攻撃後リセット
			m_Combo.ChargeTime = 0.0f;
			m_MoveMent.MoveSpeed = 6.0f;
		}
	}

	void Player::UpdateTimers(const float& dt)
	{
		 // ブースト使用可能判定
		m_Boost.IsReady = IntervalTimer(true, 0.5f, dt, m_Boost.Cooldown, false);
		// 攻撃インターバル判定
		m_Attack.IsAttack = IntervalTimer(true, m_Attack.MaxInterval, dt, m_Attack.Interval, false);
		// チャージ攻撃可能判定
		m_Attack.IsCharging = IntervalTimer(true, 0.5f, dt, m_Attack.ChargeTime, false);
	}

	void Player::TryDash(const Vec3& forward)
	{
		 // ダッシュ方向設定
		m_MoveMent.BoostAngle = forward;
		float rotY = atan2f(forward.x, forward.z);

		// エフェクト：ダッシュブリンク
		m_Effect->PlayEffect(m_BrinkHandle, L"Brick", GetPosition(), 0.0f);
		m_Effect->SetRotation(m_BrinkHandle, Vec3(0, 1, 0), rotY);

		// ステート変更：NORMAL→DASH
		m_PlayerStateNum &= ‾PlayerState::NORMAL;
		m_PlayerStateNum |= PlayerState::DASH;

		// サウンド：ダッシュ開始SE
		SoundManager::GetInstance().PlaySE(L"SE_ACCEPT");
	}

	void Player::HandleChargeHold(float dt)
	{
		 // チャージ時間を蓄積
		m_Combo.ChargeTime += dt;
		// 移動速度を半減
		m_MoveMent.MoveSpeed = 6.0f / 2.0f;
		// 最大チャージ時間(5秒)を制限
		m_Combo.ChargeTime = min(m_Combo.ChargeTime, 5.0f);
	}

	void Player::ExecuteFullChargeAttack()
	{
		 // 攻撃判定用オブジェクト生成
		m_Stage->AddGameObject<ChargeHitSphere>(
			GetPosition(), Vec3(1), GetThis<GameObject>(), m_Combo.ChargeTime);

		// 地震エフェクト再生
		m_Effect->PlayEffect(m_EarthQuakeHandle, L"EarthQuake", GetPosition(), 0.0f);
		m_Effect->SetLocation(
			m_EarthQuakeHandle,
			Vec3(GetPosition().x, GetPosition().y - 0.25f, GetPosition().z));
		m_Effect->SetScale(m_EarthQuakeHandle, Vec3(m_Combo.ChargeTime));

		// カメラシェイク
		GetStage()->GetView()->GetTargetCamera()->ShakeStart(0.5f, 0.3f);

		// ステート変更：NORMAL→ATTACKCHARGE
		m_PlayerStateNum |= PlayerState::ATTACKCHARGE;
		m_PlayerStateNum &= ‾PlayerState::NORMAL;

		// サウンド：フルチャージ攻撃SE
		SoundManager::GetInstance().PlaySE(L"SE_CHARGE_ATTACK");
	}

	void Player::ExecuteNormalAttack()
	{
		 // アニメーション名をトグル
		m_Attack.AnimName = (m_Attack.AnimName == L"Attack2") ? L"Attack" : L"Attack2";

		// パリィ判定を有効化
		m_Parry.IsJudgeActive = true;
		m_Parry.JudgeTime = 30.0f;

		// 攻撃方向取得
		Vec3 forward = GetForward();
		Vec3 targetDir = SearchRange(90.0f);
		AimRock(targetDir);

		// 攻撃判定用オブジェクト生成
		m_Stage->AddGameObject<HitSphere>(
			GetPosition(), forward, GetThis<GameObject>(),
			m_HitScale, m_MoveMent.SearchDistance);

		// ショックウェーブエフェクト再生
		float fwdRot = atan2f(forward.x, forward.z);
		m_Effect->PlayEffect(
			m_Handle, L"ShockWave",
			Vec3(
				m_Position.x + forward.x * 0.5f,
				m_Position.y + 0.25f,
				m_Position.z + forward.z * 0.5f
			),
			0.0f);
		m_Effect->SetRotation(m_Handle, Vec3(0, 1, 0), fwdRot);
		m_Effect->SetScale(m_Handle, Vec3(m_HitScale * 0.5f));

		// ステート変更：NORMAL→ATTACK
		m_PlayerStateNum |= PlayerState::ATTACK;
		m_PlayerStateNum &= ‾PlayerState::NORMAL;

		// サウンド：攻撃ボイスSE
		SoundManager::GetInstance().PlaySE(L"SE_ATTACK_VOICE", 1.0f);
	}

	void Player::SetCharge(const float& charge){
		m_Zone.EnergyCharge += charge;
	}

	void Player::UpdateAnim() {
		// 前フレームからの経過時間を取得し、アニメーションを更新
		float elapsedTime = GetElapsed();
		auto draw = GetComponent<PNTBoneModelDraw>();
		draw->UpdateAnimation(elapsedTime);
	}

	void Player::AddAnimation(){
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		float animSpeed = 1.5f;
		float animFastSpeed = 2.5f;
		auto anim_fps = 60.0f;
		ptrDraw->AddAnimation(L"Idle", 11, 60, true, anim_fps);
		ptrDraw->AddAnimation(L"Attack", 81, 60, false, anim_fps * animFastSpeed);
		ptrDraw->AddAnimation(L"Attack2", 421, 60, false, anim_fps * animFastSpeed);
		ptrDraw->AddAnimation(L"QuakeAttack", 720, 80, false, anim_fps * animSpeed);
		ptrDraw->AddAnimation(L"Counter", 620, 80, false, anim_fps * animSpeed);
		ptrDraw->AddAnimation(L"Zone", 151, 60, false, anim_fps * m_Zone.ZoneAnimProgress);
		ptrDraw->AddAnimation(L"Dash", 212, 60, true, anim_fps * animSpeed);
		ptrDraw->AddAnimation(L"Brink", 270, 1, true, anim_fps);
		ptrDraw->AddAnimation(L"Nock", 281, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Died", 351, 60, false, anim_fps);
		ptrDraw->AddAnimation(L"Clear", 491, 100, false, anim_fps);
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
			DispatchStateTransition();
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
	}

	void Player::OnDraw()
	{
		// 基底クラスの描画処理を呼び出す
		Character::OnDraw();
		// エフェクトの再生スピードをゲーム速度に合わせて調整
		float gameSpeed = GameManager::GetInstance().GetGameSpeed();
		m_Effect->SetEffectSpeed(m_ParryHandle, 2.0f * gameSpeed);
		m_Effect->SetEffectSpeed(m_Handle, 1.0f * gameSpeed);
		m_Effect->SetEffectSpeed(m_BrinkHandle, 1.0f * gameSpeed);
	}

	void Player::Dead()
	{
		// 「プレイヤー死亡」イベントをステージに通知
		PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"DeadPlayer");
	}

	bool Player::Damage(bool parry, float damage, const shared_ptr<GameObject> source)
	{
		// 1) 無敵時間中は即終了
		if (m_Damage.IsInvincible)
			return false;

		// 2) HPピンチ演出
		HandlePinchEvent();

		// 3) パリィ可能ならパリィ用ロジックへ
		if (m_Parry.IsJudgeActive)
			return TryParry(damage, source);

		// 4) 通常被ダメージ処理
		HandleNormalHit(damage);
		return false;
	}
}
