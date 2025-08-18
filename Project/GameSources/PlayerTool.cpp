/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Player.h"


namespace basecross {

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

	void Player::AimRock(Vec3 rot) {
		if (rot != Vec3()) {
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


	void Player::Blinking() {
		auto ptrDraw = GetComponent<PNTBoneModelDraw>();
		auto state = ptrDraw->GetBlendState();
		float elapsedTime = GetElapsed();
		if (IntervalTimer(true, 0.1f, elapsedTime, m_Damage.BlinkingInterval, true)) {
			if (state == BlendState::AlphaToCoverage) {
				ptrDraw->SetBlendState(BlendState::Additive);
			}
			else {
				ptrDraw->SetBlendState(BlendState::AlphaToCoverage);
			}
		}
	}

	bool Player::IntervalTimer(const bool& TimerStart, const float& MaxTimer, const float& frame, float& Timer, const bool& Return) {
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


	// HPが1/3を下回った瞬間に一度だけ演出
	void Player::HandlePinchEvent()
	{
		// HP が最大値の 1/3 以上だったかどうか (ピンチ演出の判定用)
		bool wasHighHP = (m_HP >= m_MaxHP / 3.0f);

		// ピンチラインを割ったら一度だけ演出を発火
		if (wasHighHP && m_HP < m_MaxHP / 3.0f) {
			PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"PinchPlayer");
		}
	}

	// パリィ判定のルート
	bool Player::TryParry(float incoming, const shared_ptr<GameObject>& source)
	{
		float reduced = Parry(incoming, m_Parry.JudgeTime);
		Vec3  dir = (source->GetComponent<Transform>()->GetPosition() - GetPosition());

		// 完璧パリィ
		if (reduced == 0.0f && dir != Vec3()) {
			dir.normalize();
			HandlePerfectParry(source, dir);
			return true;
		}

		// それ以外はパリィ終了後に通常ダメージ
		m_Parry.IsJudgeActive = false;
		if (!m_Parry.IsInDamageInterval)
			ApplyDamage(reduced, reduced != incoming);

		return false;
	}

	void Player::HandlePerfectParry(const shared_ptr<GameObject>& source, const Vec3& dir)
	{
		Vec3 rot = dir;
		// 攻撃元オブジェクトを取得
		auto attack = dynamic_pointer_cast<Attack>(source);
		if (attack) {
			// 攻撃を跳ね返す
			attack->ReflectParry(GetPosition());
			auto boss = attack->GetDete();
			rot = boss->GetPosition() - GetPosition();
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
		// カウンター方向を保存して狙いを固定
		m_TargetObject = Vec3(rot.x, 0, rot.z);
		AimRock(rot);
		// コンボ回数を増加・猶予タイマーを再起動
		m_Combo.IsActive = true;
		m_Combo.Timer = m_Combo.WindowAfterPerfect;
	}

	// ダメージ適用＋スコア加算
	void Player::ApplyDamage(float amount, bool playEffect)
	{
		Character::Damage(amount, playEffect);
		ScoreManager::Instance()->AddDamage(amount);
		// 無敵時間開始フラグ／点滅開始など
		m_Damage.IsInvincible = true;
	}

	// 通常ヒット時の共通処理
	void Player::HandleNormalHit(float damage)
	{
		if (m_Parry.IsInDamageInterval)
			return;

		SetAnim(L"Nock");
		m_Combo.IsActive = true;
		SoundManager::GetInstance().PlaySE(L"SE_HIT_PLAYER");
		ApplyDamage(damage, true);
	}

}