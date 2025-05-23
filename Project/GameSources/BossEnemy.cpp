/*!
@file BossEnemy.cpp
@brief ボス敵の実装
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	BossEnemy::BossEnemy(const shared_ptr<Stage>& stage) : BossEnemy(stage, Vec3(), Vec3(1.0f)) {}

	BossEnemy::BossEnemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Enemy(stage, position, scale),
		m_IsAppearance(false), m_ConditionTime(0.0f), m_ConditionDefeat(100),m_ComboCount(0),m_Stun(0),m_StartPosition(position),
		m_ComboTimer(Timer(1.0f,false)),m_IsStun(false),m_DamageEffectTime(Timer(0.2f,0.2f,false))
	{
	}
	BossEnemy::~BossEnemy()
	{
	}
	void BossEnemy::AddAnimation() {
		float fps = 60.0f;
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		draw->AddAnimation(L"Idle", 0, 1, true, fps);
		draw->AddAnimation(L"Walk", 40, 120, true, fps * 1.75f);
		draw->AddAnimation(L"Stan_First", 545, 56, false, fps * 0.6f);
		draw->AddAnimation(L"Stan", 601, 19, true, fps);
		draw->AddAnimation(L"Stan_Finish", 621, 20, false, fps * 0.5f);
		draw->AddAnimation(L"Blow", 545, 10, false, fps * 0.5f);
		
		draw->AddAnimation(L"Missile_First", 971, 9, false, fps);
		draw->AddAnimation(L"Missile", 981, 10, true, fps);
		draw->AddAnimation(L"Missile_Finish", 992, 8, false, fps);
		draw->AddAnimation(L"Crush", 1280, 90, false, fps);
	}
	void BossEnemy::SetAnimation(const wstring& key) {
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		
		if (draw->GetCurrentAnimation() != key)
			if (draw->GetAnimeLoop()) {
				draw->ChangeCurrentAnimation(key);
			}
			else {
				if (draw->IsTargetAnimeEnd()) {
					draw->ChangeCurrentAnimation(key);
				}
			}
				 
	}
	bool BossEnemy::GetAnimationFinish() {
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		return draw->IsTargetAnimeEnd();
	}
	wstring BossEnemy::GetCurrentAnimationKey() {
		return GetComponent<BcPNTBoneModelDraw>()->GetCurrentAnimation();
	}
	void BossEnemy::OnCreate()
	{
		Enemy::OnCreate();
		SetSpeed(3.0f);
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
		if (player != nullptr) {
			SetIntruder(player);
		}
		m_Stage->SetSharedGameObject(L"BOSS", GetThis<BossEnemy>());
		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(false);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		ptrDraw->SetMeshResource(L"BOSS");

		AddAnimation();
		SetAnimation(L"Idle");
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.6f, 0.6f, 0.6f), //(.1f, .1f, .1f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -0.3f, 0.0f)
		);
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_CUBE");

		auto navi = AddComponent<Navigate>();

		m_currentState = make_unique<BossHostility>(GetThis<BossEnemy>());
		m_currentState->Enter();

		AddComponent<Gravity>();

		m_Effect = m_Stage->GetCreateEffect();

		m_Cruch = m_Stage->AddGameObject<CrushAttack>(Vec3(1.5f), AttackDate(GetThis<BossEnemy>(),5.0f, 1.0f, 0.25f, 3.0f, 1.0f), 3.0f);
		m_Gun = m_Stage->AddGameObject<MachineGun>(m_Intruder, AttackDate(GetThis<BossEnemy>(),1.0f, 10.0f, 2.0f, 10.0f, 2.0f), 20.0f);
		m_Missile = m_Stage->AddGameObject<Missile>(player->GetTransform(), AttackDate(20.0f,1.7f, 5.0f, 2.0f), 0.0f, 6, 0.25f);
		m_Missile->AddMuzzle(Vec3(0.5f, 0, 0.25f));
		m_Missile->AddMuzzle(Vec3(-0.5f, 0, 0.25f));
	}
	

	void BossEnemy::OnUpdate()
	{
		Enemy::OnUpdate();
		float elapsed = App::GetApp()->GetElapsedTime();
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		draw->UpdateAnimation(elapsed);
		if (!m_IsStun) {
			if (GetCurrentAnimationKey() != L"Blow") {
				m_currentState->Execute();
			}
			else if (GetAnimationFinish()) {
				SetAnimation(L"Idle");
			}
			
			if (m_DamageEffectTime.UpdateTimer()) {
				draw->SetDiffuse(Col4(1, 1, 1, 1));
			}
			else {
				draw->SetDiffuse(Col4(1, 0, 0, 1));
			}
		}
		else {
			auto& effect = m_Stage->GetCreateEffect();
			if (GetAnimationFinish() && GetCurrentAnimationKey() == L"Stan_First") {
				SetAnimation(L"Stan");
				
				effect->PlayEffect(m_EffectHandle,L"Smoke", GetPosition(), 0.0f);
				effect->SetScale(m_EffectHandle,Vec3(0.2f));
				effect->SetEffectSpeed(m_EffectHandle,0.5f);
			}
			if (GetAnimationFinish() && GetCurrentAnimationKey() == L"Stan_Finish") {
				m_IsStun = false;
			}
			if (GetCurrentAnimationKey() == L"Stan") {
				m_Stun -= elapsed / 4.0f;
			}
			//draw->SetDiffuse(Col4(0, 0, 0, 1));
			
			if (m_Stun < 0) {
				m_Stun = 0;
				//m_IsStun = false;
				SetAnimation(L"Stan_Finish");
				effect->StopEffect(m_EffectHandle);
			}
		}
	}
	void BossEnemy::AddStun(float stun) {
		if (!m_IsStun) {
			m_Stun += stun;
			if (m_Stun > 1.0f) {
				m_IsStun = true;
				SetAnimation(L"Stan_First");
			}
			else if (stun >= 0.3f) {
				SetAnimation(L"Blow");
			}
		}
	}
	void BossEnemy::OnCollisionEnter(shared_ptr<GameObject>& other) {
		if (other->FindTag(L"HitJudge")) {
			Damage(2.0f, false);
			AddStun(0.01f * (m_ComboCount + 1));
			if (!m_IsStun) {
				m_ComboCount++;
				m_ComboCount = min(4, m_ComboCount);
				m_ComboTimer.SetTime(1.0f, true);
			}
			SoundManager::Instance().PlaySE(L"SE_HIT_ENEMY");
		}
	}
	void BossEnemy::Dead()
	{
		PostEvent(1.0f,GetThis<ObjectInterface>(), m_Stage, L"DefeatBoss");
		Enemy::Dead();
	}
	void BossEnemy::Damage(float damage, const bool& isSound) {
		Enemy::Damage(damage, isSound);
		m_DamageEffectTime.SetTime(0.2f, true);
	}
	void BossEnemy::Move(const Vec3& direction) {
		Character::Move(direction);
		SetAnimation(L"Walk");
	}
	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage) : BossEnemyLeg(stage, Vec3(), shared_ptr<Enemy>(), float()) {}

	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage, const Vec3& position, const shared_ptr<Enemy>& enemy, const float& direction) :
		Enemy(stage, position, Vec3(1.0f, 3.0f, 1.0f)),
		m_Enemy(enemy),
		m_Direction(direction)
	{
	}

	BossEnemyLeg::~BossEnemyLeg()
	{
	}

	void BossEnemyLeg::OnCreate()
	{
		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(m_Position.x + m_Direction, m_Position.y - 1.5f, m_Position.z);
		m_Transform->SetRotation(m_Rotation);
		m_Transform->SetScale(Vec3(1.0f, 3.0f, 1.0f));

		auto ptrColl = AddComponent<CollisionObb>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(true);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_CUBE");

		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_CUBE");

	}
	void BossEnemyLeg::OnUpdate()
	{
	}
	void BossEnemyLeg::Dead()
	{
		Enemy::Dead();
	}
}
