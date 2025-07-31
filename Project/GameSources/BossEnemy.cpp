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
		m_ComboTimer(Timer(1.0f,false)),m_IsStun(false), m_HealStun(Timer(7.5f,false)),
		m_InvincibleTimer(Timer(0.1f,0.1f,false)), m_AroundPlayerTime(0),
		m_MotionRate(1.0f), m_DeadEffect(false), m_IsGround(true),
		m_EffectBombHandle(0),m_EffectHandle(0),m_SmokeHandle(0)
	{
	}
	BossEnemy::‾BossEnemy()
	{
	}
	void BossEnemy::AddAnimation() {
		float fps = 60.0f;
		Difficulty difficulty = GameManager::GetInstance().GetDifficulty();
		m_MotionRate = max(1.0f, (int)difficulty * 0.6f);
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		draw->AddAnimation(L"Idle", 40, 1, true, fps);
		draw->AddAnimation(L"Walk", 40, 120, true, fps * 1.75f);
		draw->AddAnimation(L"Stan_First", 545, 56, false, fps * 0.6f);
		draw->AddAnimation(L"Stan", 601, 19, true, fps);
		draw->AddAnimation(L"Stan_Finish", 621, 20, false, fps * 0.5f);
		draw->AddAnimation(L"Blow", 545, 10, false, fps * 0.5f);
		
		draw->AddAnimation(L"Missile_First", 971, 9, false, fps * m_MotionRate);
		draw->AddAnimation(L"Missile", 981, 10, true, fps);
		draw->AddAnimation(L"Missile_Finish", 992, 8, false, fps * m_MotionRate);
		draw->AddAnimation(L"Crush", 1280, 90, false, fps * m_MotionRate);

		draw->AddAnimation(L"Jump", 2414, 5, true, fps);
		draw->AddAnimation(L"Landing_First", 2420, 16, false, fps * 0.5f);
		draw->AddAnimation(L"Landing", 2437, 67, false, fps * 0.5f);//16

		draw->AddAnimation(L"ShakeOff_First", 2300, 21, false, fps * 0.5f * m_MotionRate);
		draw->AddAnimation(L"ShakeOff_Bef", 2321, 17, false, fps * 1.5f);
		draw->AddAnimation(L"ShakeOff_Aft", 2339, 61, false, fps * 1.5f);
		draw->AddAnimation(L"ShakeOff_Parry", 2561, 118, false, fps * 1.5f);
	}
	void BossEnemy::SetAnimation(const wstring& key, const bool& isChange) {
		auto draw = GetComponent<BcPNTBoneModelDraw>();

		if (draw->GetCurrentAnimation() != key)
			if (draw->GetAnimeLoop()) {
				draw->ChangeCurrentAnimation(key);
			}
			else {
				if (draw->IsTargetAnimeEnd() || isChange) {
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
		//描画設定
		auto ptrDraw = AddComponent<BcPNTBoneModelDraw>();
		ptrDraw->SetMeshResource(L"BOSS");
		Mat4x4 meshMat;
		meshMat.affineTransformation(
			Vec3(0.3f, 0.3f, 0.3f), //(.1f, .1f, .1f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, XM_PI, 0.0f),
			Vec3(0.0f, -1.0f, 0.0f)
		);
		ptrDraw->SetMeshToTransformMatrix(meshMat);
		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"BOSS");
		shadowPtr->SetMeshToTransformMatrix(meshMat);


		m_currentState = make_unique<BossHostility>(GetThis<BossEnemy>());
		m_currentState->Enter();

		AddComponent<Gravity>(Vec3());

		AddTag(L"Boss");
		m_Effect = m_Stage->GetCreateEffect();

		AddTag(L"Boss");
	}

	void BossEnemy::OnAfterCreate() {
		AddAnimation();
		SetAnimation(L"Idle");
		RegisterAttack();

	}

	void BossEnemy::RegisterAttack() {
		Difficulty difficulty = GameManager::GetInstance().GetDifficulty();
		float addRate = max(1.0f, (int)difficulty * 0.75f);
		float crushDamage = 5.0f * addRate;
		float crushSize = 1.5f * addRate;
		float crushRange = max(2.0f, crushSize / 1.8f);
		float crushBlow = 3.0f * addRate;

		int missileCount = static_cast<int>(6.0f * addRate);
		float missileInterval = 0.25f;
		float explodeSize = 2.0f * addRate;

		auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);

		auto crush = m_Stage->AddGameObject<CrushAttack>(Vec3(crushSize), AttackDate(GetThis<BossEnemy>(), crushDamage, crushRange, 0.25f, 3.0f, 1.0f), crushBlow);
		auto missile = m_Stage->AddGameObject<Missile>(player->GetTransform(), AttackDate(GetThis<BossEnemy>(), 0.0f, 20.0f, missileInterval * (float)missileCount, 5.0f, 2.0f), explodeSize, missileCount, missileInterval);
		missile->AddMuzzle(Vec3(0.5f, 0, 0.25f));
		missile->AddMuzzle(Vec3(-0.5f, 0, 0.25f));
		auto shakeoff = m_Stage->AddGameObject<ShakeOffAttack>(Vec3(4.0f,1.0f,3.5f), AttackDate(GetThis<BossEnemy>(), crushDamage, crushRange, 0.25f, 3.0f, 1.0f), 10.0f);


		auto crushSkill = make_shared<BossCrush>(GetThis<BossEnemy>());
		crushSkill->SetSkill(crush);
		auto missileSkill = make_shared<BossGun>(GetThis<BossEnemy>());
		missileSkill->SetSkill(missile);
		auto shakeSkill = make_shared<BossShakeOff>(GetThis<BossEnemy>());
		shakeSkill->SetSkill(shakeoff);

		StageData data = m_Stage->GetStageData();
		if (data.stageNum == 0) {
			AddSkill(crushSkill, 60.0f);
			AddSkill(missileSkill, 40.0f);
		}
		else if (data.stageNum == 1) {
			AddSkill(shakeSkill, 20.0f);
			AddSkill(crushSkill, 40.0f);
			AddSkill(missileSkill, 30.0f);
		}
		else {
			AddSkill(shakeSkill, 20.0f);
			AddSkill(crushSkill, 40.0f);
			AddSkill(missileSkill, 30.0f);
		}
		

		float hp = GetMaxHP();
		hp *= (int)difficulty;
		InitHP(hp);

		float healStunTime = 7.5f;
		healStunTime /= addRate;
		m_HealStun.SetTime(healStunTime);

	}

	void BossEnemy::OnUpdate()
	{
		if (!GetDrawActive()) {
			SetUpdateActive(false);
			return;
		}
		Enemy::OnUpdate();
		float elapsed = GetGameElapsed();
		auto draw = GetComponent<BcPNTBoneModelDraw>();
		draw->UpdateAnimation(elapsed);
		m_InvincibleTimer.UpdateTimer();
		if (!m_IsStun) {
			m_currentState->Execute();
			if (m_HealStun.UpdateTimer() && m_Stun >= 0.25f) {
				m_Stun -= 0.05f * elapsed;
			}
			if (m_Skills.size() >= 3) {
				if ((m_Intruder->GetPosition() - GetPosition()).length() > 3.0f) {
					m_AroundPlayerTime += elapsed;
					if (m_AroundPlayerTime > 1.0f) {
						m_Skills[0].m_Prob *= 1.1f;
						m_AroundPlayerTime = 0.0f;
					}
				}
				else {
					m_AroundPlayerTime -= elapsed;
					if (m_AroundPlayerTime < -1.0f) {
						m_Skills[0].m_Prob = max(20.0f, m_Skills[0].m_Prob *= 0.9f);
						m_AroundPlayerTime = 0.0f;
					}
				}
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
			
			if (m_Stun < 0) {
				m_Stun = 0;
				SetAnimation(L"Stan_Finish");
				effect->StopEffect(m_EffectHandle);
			}
		}
	}
	void BossEnemy::OnSpawn() {
		Enemy::OnSpawn();
		auto gravity = GetComponent<Gravity>();
		gravity->SetGravity(Vec3(0.0f, -9.8f, 0.0f));

		Vec3 position = GetPosition();
		position.y += 70.0f;
		SetPosition(position);
		Vec3 direction = m_Intruder->GetPosition() - position;
		float rotationY = atan2f(direction.x, direction.z);
		SetRotation(Vec3(0, rotationY, 0));

		ChangeState<BossStarting>();
	}
	void BossEnemy::AddStun(float stun) {
		if (!m_IsStun) {
			float rate = (float)GameManager::GetInstance().GetDifficulty();
			stun /= rate;
			m_Stun += stun;
			m_HealStun.Reset();
			if (m_Stun > 1.0f) {
				if (stun > 0.25f / rate) {
					m_IsStun = true;
					SetAnimation(L"Stan_First", true);
				}
				else {
					m_Stun -= stun;
				}
				
			}
		}
	}
	void BossEnemy::OnCollisionEnter(shared_ptr<GameObject>& other) {
		if (other->FindTag(L"HitJudge")) {
			SoundManager::GetInstance().PlaySE(L"SE_HIT_ENEMY");
		}

		if (GetCurrentAnimationKey() == L"Jump" && other->FindTag(L"Ground")) {
			SetAnimation(L"Landing_First", true);
		}
	}
	void BossEnemy::Dead()
	{
		auto& effect = m_Stage->GetCreateEffect();
		if (!m_DeadEffect)
		{
			effect->PlayEffect(m_EffectBombHandle, L"Bomb", GetPosition(), 0.0f);
			effect->SetScale(m_EffectBombHandle, Vec3(0.5f));
			m_DeadEffect = true;
		}
		PostEvent(1.0f,GetThis<ObjectInterface>(), m_Stage, L"DefeatBoss");
		effect->StopEffect(m_EffectHandle);
		Enemy::Dead();
	}
	void BossEnemy::Damage(float damage, const bool& isSound) {
		if (m_InvincibleTimer.CheckTime()) {
			Enemy::Damage(damage, isSound);
			m_InvincibleTimer.Reset();
			AddStun(0.06f);
		}
		
	}
	void BossEnemy::Move(const Vec3& direction,const bool& isGameSpeed) {
		Character::Move(direction, isGameSpeed);
		SetAnimation(L"Walk");
	}
	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage) : BossEnemyLeg(stage, Vec3(), shared_ptr<Enemy>(), float()) {}

	BossEnemyLeg::BossEnemyLeg(const shared_ptr<Stage>& stage, const Vec3& position, const shared_ptr<Enemy>& enemy, const float& direction) :
		Enemy(stage, position, Vec3(1.0f, 3.0f, 1.0f)),
		m_Enemy(enemy),
		m_Direction(direction)
	{
	}

	BossEnemyLeg::‾BossEnemyLeg()
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
