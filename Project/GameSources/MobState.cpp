/*!
@file Enemy.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"
#include "MobState.h"

namespace basecross {

	void MobSearch::Enter(){
		EnemyState::Enter();
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		enemy->SetAnim(L"Walk", 0.0f);
		m_Path.clear();
		Execute();
	}

	void MobSearch::Execute(){
		Difficulty difficulty = GameManager::Instance()->GetDifficulty();
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		Vec3 pos = enemy->GetPosition();
		float elapsedTime = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();

		shared_ptr<Object> obj;
		auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
		auto groups = group->GetGroupVector();

		enemy->SetAnim(L"Walk", 0.0f);
		Vec3 objDirection;
		float objRange = FLT_MAX;

		for (const auto& citizen : groups){
			auto shObj = citizen.lock();
			Vec3 direction = shObj->GetComponent<Transform>()->GetPosition() - pos;
			float range = direction.length();
			if (range < objRange){
				objRange = range;
				objDirection = direction;
				obj = dynamic_pointer_cast<Object>(shObj);
			}
		}

		if (obj){
			objDirection = obj->GetComponent<Transform>()->GetPosition() - pos;
			m_IntruderAlert = m_Enemy->GetIntruderAlert();

			if (m_IntruderAlert){
				m_Enemy->ChangeState<MobAlert>();
				return;
			}

			float objRotate = atan2f(-objDirection.z, objDirection.x);
			m_Transform->SetRotation(Vec3(0, objRotate, 0));
			float objRange = objDirection.length();

			if (enemy->m_kariState == Mob::kariState::hakai && objRange <= enemy->m_BalletRange / 2){
				m_Enemy->ChangeState<MobAlert>();
			}
			else{
				pos += objDirection.normalize() * elapsedTime * m_Enemy->m_ZoneElapsedTime * ((int)difficulty * 2);
				enemy->SetPosition(pos);
			}
		}
		else{
			Vec3 direction = m_Player->GetPosition() - pos;
			float rotate = atan2f(direction.x, direction.z);
			m_Transform->SetRotation(Vec3(0, rotate, 0));
			float range = direction.length();

			if (range > enemy->m_BalletRange / 2){
				pos += direction.normalize() * elapsedTime * m_Enemy->m_ZoneElapsedTime * ((int)difficulty * 2);
				enemy->SetPosition(pos);
			}
			else{
				m_IntruderAlert = m_Enemy->GetIntruderAlert();
				if (m_IntruderAlert){
					m_Enemy->ChangeState<MobAlert>();
				}
			}
		}
	}

	void MobSearch::Exit(){
		auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
		enemy->SetAnim(L"SetUp", 0.0f);
	}

    void MobAlert::Enter(){
        EnemyState::Enter();
        auto mob = dynamic_pointer_cast<Mob>(m_Enemy);
        m_Stage = m_Enemy->GetStage();

        // 弾発射用の間隔を初期化（必要に応じてランダムな値も利用可能）  
        mob->m_ShotRandomInterval = mob->MAX_BALLET_INTERVAL * 0.5f; // 例：Util::RandZeroToOne() * (mob->MAX_BALLET_INTERVAL * 0.5f)
        m_BulletRemain = mob->m_BulletRemain;

        // ステージからエフェクト管理オブジェクトを取得  
        auto stage = static_pointer_cast<GameStage>(m_Stage);
        m_Effect = (stage != nullptr) ? stage->GetCreateEffect() : nullptr;

        Execute();
    }

    void MobAlert::Execute() {
        auto stage = static_pointer_cast<Stage>(m_Stage);
        // タイムレートを掛けた経過時間
        float elapsedTime = App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
        auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);

        // intruder 状況の確認
        m_IntruderAlert = enemy->GetIntruderAlert();

        // Citizen グループ内の最も近いオブジェクトを検索
        shared_ptr<Object> obj;
        auto group = m_Stage->GetSharedObjectGroup(L"Citizen");
        auto groups = group->GetGroupVector();
        Vec3 objDirection;
        float objRange = FLT_MAX;
        for (const auto& citizen : groups) {
            auto shObj = citizen.lock();
            if (shObj) {
                Vec3 currentDir = shObj->GetComponent<Transform>()->GetPosition() - m_Enemy->GetPosition();
                float currentRange = currentDir.length();
                if (currentRange < objRange) {
                    objRange = currentRange;
                    objDirection = currentDir;
                    obj = dynamic_pointer_cast<Object>(shObj);
                }
            }
        }

        Vec3 direction;
        // 現在の向きと位置の取得
        Vec3 forward = enemy->GetForward();
        Vec3 position = enemy->GetPosition();
        // forward ベクトルから回転角（ラジアン）の算出
        float rotate = atan2f(forward.x, forward.z);


        // 【＜IntruderAlert が有効な場合＞】
        if (m_IntruderAlert)
        {
            float frameElapsedTime = App::GetApp()->GetElapsedTime();
            // 既に障害物回避中ならその方向を一定時間維持する
            if (enemy->m_IsAvoiding)
            {
                enemy->m_AvoidTime -= frameElapsedTime;
                if (enemy->m_AvoidTime > 0.0f)
                {
                    // 補間付きの移動によって滑らかに回避
                    enemy->SetMoveDirection(enemy->m_AvoidDirection);
                    return;
                }
                else
                {
                    enemy->m_IsAvoiding = false;
                }
            }
            float elapsedTime = App::GetApp()->GetElapsedTime();

            // RayCast クールダウンを減らす
            enemy->m_RayCastCooldown -= elapsedTime;

            // クールタイムが残っていればレイキャストをスキップして、前回の方向で移動継続
            if (enemy->m_RayCastCooldown > 0.0f) {
                if (enemy->m_IsAvoiding && enemy->m_AvoidTime > 0.0f) {
                    enemy->m_AvoidTime -= elapsedTime;
                    enemy->SetMoveDirection(enemy->m_AvoidDirection);
                    return;
                }
            }
            else {
                // プレイヤー方向までの線分上の障害物検出（プレイヤーは除外しない）
                RayCastHit hit;
                auto line = Line(enemy->GetPosition(), m_Player->GetPosition());
                line.SetMaxLength(10.0f);
                vector<wstring> excludeTags = { L"Bullet", L"Line", L"Ground", L"Player"};
                if (RayCast::HitTestVec(hit, line, m_Stage->GetGameObjectVec(), excludeTags, enemy)) {
                    // ヒットした対象がプレイヤーでなければ回避処理へ
                    auto hitTransform = hit.m_Object->GetComponent<Transform>();
                    if (hitTransform) {
                        Vec3 avoidDir = enemy->GetPosition() - hitTransform->GetPosition();
                        if (avoidDir.length() > 0.001f) {
                            avoidDir.normalize();
                            enemy->m_AvoidDirection = avoidDir;
                            enemy->m_IsAvoiding = true;
                            enemy->m_AvoidTime = 0.8f;  // 回避状態を0.8秒維持
                            enemy->SetMoveDirection(avoidDir);
                            return;
                        }
                    }
                }
                else {
                    // プレイヤー方向へ通常移動
                    enemy->AlartMove(m_Player);
                }
            }
            direction = enemy->GetDirectionToIntruderObject(m_Player);
        }
        else if (obj != nullptr) { // 【＜何か Citizen ターゲットがある場合＞】
            enemy->AlartMove(obj);
            direction = m_Enemy->GetDirectionToIntruderObject(obj);
            // 対象オブジェクト方向へ回転
            float objRotate = atan2f(objDirection.x, objDirection.z);
            m_Transform->SetRotation(Vec3(0, objRotate, 0));
            // 一定距離以上離れていれば探索状態に戻す
            if (objRange > enemy->m_BalletRange / 2) {
                m_Enemy->ChangeState<MobSearch>();
                return;
            }
        }
        else { // 【＜ターゲットが見つからなければ＞】
            m_Enemy->ChangeState<MobSearch>();
            return;
        }
        // 攻撃状態のアニメーションに切り替え
        enemy->SetAnim(L"Set", 0.0f);

        // 弾の残数がある場合の処理 
        if (m_BulletRemain > 0)
        {
            // エフェクト発動条件チェック
            if (enemy->m_BalletInterval < 0.4f && enemy->m_ShotRandomInterval < 0.4f && !m_BulletEffect)
            {
                m_Effect->PlayEffect(m_Eyehandle, L"EnemyEye",
                    Vec3(position.x, position.y + 0.5f, position.z), 0.0f);
                m_Effect->SetRotation(m_Eyehandle, Vec3(0.0f, 1.0f, 0.0f), rotate);
                m_BulletEffect = true;
            }
            else if (enemy->m_BalletInterval < 0.25f && enemy->m_ShotRandomInterval < 0.25f && !m_BulletSound) {
                SoundManager::Instance().PlaySE(L"SE_ATTACK_SIGN", 1.0f);
                m_BulletSound = true;
            }
            else if (enemy->m_BalletInterval <= 0 && enemy->m_ShotRandomInterval <= 0) {
                m_Effect->PlayEffect(m_Handle, L"Flash",
                    Vec3(position.x + forward.x / 2, position.y + 0.25f, position.z + forward.z / 2), 8.0f);
                m_Effect->SetRotation(m_Handle, Vec3(0.0f, 1.0f, 0.0f), rotate);
                m_Effect->SetScale(m_Handle, Vec3(0.1f, 0.1f, 0.1f));

                auto ballet = m_Stage->AddGameObject<Bullet>(
                    m_Transform->GetPosition() + direction * enemy->m_MuzzleOffset,
                    enemy->m_BalletSpeed, direction, enemy->m_BalletRange);

                m_BulletEffect = false;
                m_BulletSound = false;
                enemy->m_BalletInterval = enemy->MAX_BALLET_INTERVAL;
                enemy->m_ShotRandomInterval = enemy->MAX_BALLET_INTERVAL;
                m_BulletRemain--;
                SoundManager::Instance().PlaySE(L"SE_SHOT");
            }
        }
        else {
            enemy->SetAnim(L"Reload", 0.0f);
            m_BulletRelord -= elapsedTime;
            if (m_BulletRelord < 0.0f) {
                enemy->m_BalletInterval = enemy->MAX_BALLET_INTERVAL;
                enemy->m_ShotRandomInterval = 1.0f;
                m_BulletRemain = enemy->m_BulletRemain;
                m_BulletEffect = false;
                m_BulletSound = false;
                m_BulletRelord = 3.0f;
            }
        }

        if (m_Effect) {
            m_Effect->SetEffectSpeed(m_Eyehandle, GameManager::Instance()->GetTimeRate());
            m_Effect->SetEffectSpeed(m_Handle, GameManager::Instance()->GetTimeRate());
        }
    }
    void MobAlert::Exit(){
        auto enemy = dynamic_pointer_cast<Mob>(m_Enemy);
        enemy->SetAnim(L"SetDown", 0.0f);
    }
}
