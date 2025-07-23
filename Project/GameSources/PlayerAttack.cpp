/*!
@file PlayerAttack.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Player.h"

namespace basecross {
	BaseHitObject::BaseHitObject(
		const shared_ptr<Stage>& stage,
		const Vec3& position,
		const Vec3& scale,
		const shared_ptr<GameObject>& player
	) :
		Object(stage),
		m_Position(position),
		m_Scale(scale),
		m_Player(player)
	{
	}

	BaseHitObject::‾BaseHitObject()
	{
		m_Effect->StopEffect(m_MainHandle);
	}

	void BaseHitObject::OnCreate() {
		// Transform 初期化
		auto t = AddComponent<Transform>();
		t->SetPosition(m_Position);
		t->SetScale(m_Scale);

		// エフェクトマネージャ取得
		m_Effect = static_pointer_cast<GameStage>(GetStage())->GetCreateEffect();
	}

	void BaseHitObject::OnUpdate() {
		// 共通：エフェクト速度をゲーム速度に合わせる
		float speed = GameManager::Instance()->GetGameSpeed();
		m_Effect->SetEffectSpeed(m_MainHandle, speed);
		m_Effect->SetEffectSpeed(m_HitHandle, speed);

		// 派生クラスはここで移動や寿命管理を行う
	}

	void BaseHitObject::OnCollisionEnter(shared_ptr<GameObject>& other) {
		if (!other || !other->FindTag(L"Enemy")) return;

		auto enemy = dynamic_pointer_cast<Character>(other);
		if (!enemy) return;

		ApplyHit(enemy);
		if (m_Effect && m_HitHandle != -1) {
			m_Effect->PlayEffect(m_HitHandle, L"HitEffect", enemy->GetPosition(), 0.0f);
			float rot = atan2f(enemy->GetRotation().x, enemy->GetRotation().z);
			m_Effect->SetRotation(m_HitHandle, Vec3(0, 1, 0), rot);
		}

		XINPUT_VIBRATION vib{ 65535, 65535 };
		XInputSetState(0, &vib);
		PostEvent(0.25f, nullptr, GetStage(), L"StopVibration");
	}

	HitSphere::HitSphere(
		const shared_ptr<Stage>& stage,
		const Vec3& position,
		const Vec3& forward,
		const shared_ptr<GameObject>& player,
		const Vec3& scale,
		float length
	) :
		BaseHitObject(stage, position, scale, player),
		m_Direction(forward)
	{
		bool inZone = (static_pointer_cast<Player>(m_Player)->GetStates()
			& Player::PlayerState::ZONE) != 0;
		m_FlyingTime = inZone ? 1.0f : 0.1f;
		m_Speed = length / m_FlyingTime;
	}

	void HitSphere::ApplyHit(shared_ptr<Character> enemy) {
		auto player = static_pointer_cast<Player>(m_Player);
		player->SetCharge(0.1f);
		enemy->Damage(player->GetAttackDamage(), false);
	}
	void HitSphere::OnCreate() {
		BaseHitObject::OnCreate();
		auto col = AddComponent<CollisionSphere>();
		col->SetDrawActive(GameManager::Instance()->IsDebug());
		col->SetFixed(false);
		col->SetAfterCollision(AfterCollision::None);

		AddTag(L"HitJudge");

		bool inZone = (static_pointer_cast<Player>(m_Player)->GetStates()
			& Player::PlayerState::ZONE) != 0;
		if (!inZone) return;

		m_Effect->PlayEffect(m_MainHandle, L"Panchi", m_Position, 20.0f);
		float rot = atan2f(m_Direction.x, m_Direction.z);
		m_Effect->SetRotation(m_MainHandle, Vec3(0, 1, 0), rot);
		m_Effect->SetScale(m_MainHandle, m_Scale + 0.5f);

	}

	void HitSphere::OnUpdate() {
		// 共通更新
		BaseHitObject::OnUpdate();

		float dt = App::GetApp()->GetElapsedTime()
			* GameManager::Instance()->GetGameSpeed();
		m_Elapsed += dt;

		if (m_Elapsed < m_FlyingTime) {
			auto t = GetComponent<Transform>();
			Vec3 pos = t->GetPosition() + m_Speed * m_Direction * dt;
			t->SetPosition(pos);
			m_Effect->SetLocation(m_MainHandle, pos);
		}
		else {
			GetStage()->RemoveGameObject<HitSphere>(GetThis<HitSphere>());
		}
	}


	ChargeHitSphere::ChargeHitSphere(
		const shared_ptr<Stage>& stage,
		const Vec3& position,
		const Vec3& scale,
		const shared_ptr<GameObject>& player,
		float chargeTime
	) :
		BaseHitObject(stage, position, scale, player),
		m_TotalTime(1.0f),
		m_ChargeRate(chargeTime * 10.0f)
	{
	}

	void ChargeHitSphere::ApplyHit(shared_ptr<Character> enemy) {
		auto player = static_pointer_cast<Player>(m_Player);
		player->SetCharge(0.1f);
		enemy->Damage(player->GetAttackDamage() / 4, false);

		// 衝突後は除外
		if (!enemy->FindTag(L"Boss"))
		{
			GetComponent<CollisionCapsule>()->AddExcludeCollisionGameObject(enemy);
		}
	}

	void ChargeHitSphere::OnCreate() {
		BaseHitObject::OnCreate();
		auto col = AddComponent<CollisionCapsule>();
		col->SetDrawActive(GameManager::Instance()->IsDebug());
		col->SetFixed(false);
		col->SetAfterCollision(AfterCollision::None);
		AddTag(L"CaargeHitJudge");
	}


	void ChargeHitSphere::OnUpdate() {
		BaseHitObject::OnUpdate();

		float dt = App::GetApp()->GetElapsedTime()
			* GameManager::Instance()->GetGameSpeed();
		m_TotalTime -= dt;

		if (m_TotalTime <= 0.0f) {
			GetStage()->RemoveGameObject<ChargeHitSphere>(GetThis<ChargeHitSphere>());
			return;
		}

		m_Scale += Vec3(m_ChargeRate * dt, 0.01f, m_ChargeRate * dt);
		GetComponent<Transform>()->SetScale(m_Scale);
	}

	CounterHitSphere::CounterHitSphere(
		const shared_ptr<Stage>& stage,
		const shared_ptr<GameObject>& player,
		const Vec3& offset,
		float attachTime,
		float chargeRate
	) :
		BaseHitObject(stage, Vec3(), Vec3(2.0f), player),
		m_LocalOffset(offset),
		m_AttachDuration(attachTime),
		m_ChargeRate(chargeRate)
	{
		// m_Position, m_Scale は BaseHitObject に渡しておく
	}

	void CounterHitSphere::ApplyHit(shared_ptr<Character> enemy) {
		auto player = static_pointer_cast<Player>(m_Player);
		player->SetCharge(0.1f);
		enemy->Damage(player->GetAttackDamage(), false);
		PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"ContorStop");
	}

	void CounterHitSphere::OnCreate() {
		BaseHitObject::OnCreate();

		auto col = AddComponent<CollisionSphere>();
		col->SetDrawActive(GameManager::Instance()->IsDebug());
		col->SetAfterCollision(AfterCollision::None);
		col->AddExcludeCollisionGameObject(m_Player);
		col->AddExcludeCollisionTag(L"Attack");
		auto player = static_pointer_cast<Player>(m_Player);

		AddTag(L"CounterHitJudge");
		// 初期位置をプレイヤー＋オフセットに
		auto t = GetComponent<Transform>();
		Vec3 playerPos = m_Player->GetComponent<Transform>()->GetPosition();
		t->SetPosition(playerPos + m_LocalOffset);
		// エフェクト位置も同期
		m_Effect->SetLocation(m_MainHandle, playerPos + m_LocalOffset);

	}
	void CounterHitSphere::OnUpdate() {
		// 共通エフェクト速度同期のみ実行
		BaseHitObject::OnUpdate();

		float dt = App::GetApp()->GetElapsedTime()
			* GameManager::Instance()->GetGameSpeed();
		m_Elapsed += dt;

		if (m_Elapsed >= m_AttachDuration) {
			// くっつく時間終了 → 自身をステージから除去

			GetStage()->RemoveGameObject<CounterHitSphere>(GetThis<CounterHitSphere>());
			return;
		}

		// 追随：プレイヤー位置＋ローカルオフセット
		Vec3 playerPos = m_Player->GetComponent<Transform>()->GetPosition();
		Vec3 newPos = playerPos + m_LocalOffset;
		GetComponent<Transform>()->SetPosition(newPos.x, newPos.y, newPos.z);
		m_Effect->SetLocation(m_MainHandle, newPos);

		// 時間経過で半径（スケール）を徐々に拡大
		float deltaR = m_ChargeRate * dt;
		m_Scale += Vec3(deltaR);
		GetComponent<Transform>()->SetScale(m_Scale);

	}

}
