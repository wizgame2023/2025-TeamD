/*!
 @file   MobState.cpp
 @brief  MobSearch / MobAlert の実装
*/

#include "stdafx.h"
#include "Project.h"
#include "MobState.h"

namespace basecross {

    //―――――――――――――――――――――――――――
    // 障害物回避ロジック
    //―――――――――――――――――――――――――――

    /**
     * @brief 障害物回避ロジックを実行します。
     * @param mob 回避動作を行う Mob インスタンス
     * @param stage 現在のステージインスタンスポインタ
     */
    static void HandleObstacleAvoidance(std::shared_ptr<Mob> mob, Stage* stage) {
        // レイキャストのクールダウン減衰
        float frameTime = App::GetApp()->GetElapsedTime();
        mob->m_RayCastCooldown -= frameTime;
        if (mob->m_RayCastCooldown > 0.0f) return;

        auto enemy = dynamic_pointer_cast<Enemy>(mob);

        // プレイヤー方向へのレイキャスト
        RayCastHit hit;
        Line ray(mob->GetPosition(), enemy->GetIntruder()->GetPosition());
        ray.SetMaxLength(10.0f);

        std::vector<std::wstring> excludeTags{ L"Bullet", L"Line", L"Ground", L"Player" };
        if (RayCast::HitTestVec(hit, ray, stage->GetGameObjectVec(), excludeTags, mob)) {
            // 障害物を検出したら回避方向を設定
            auto ht = hit.m_Object->GetComponent<Transform>();
            if (ht) {
                Vec3 avoid = mob->GetPosition() - ht->GetPosition();
                if (avoid.length() > 0.001f) {
                    avoid.normalize();
                    mob->m_AvoidDirection = avoid;
                    mob->m_IsAvoiding = true;
                    mob->m_AvoidTime = AVOID_DURATION;
                    mob->SetMoveDirection(avoid);
                }
            }
        }
    }

    //―――――――――――――――――――――――――――
    // 射撃エフェクト＆サウンド更新
    //―――――――――――――――――――――――――――

    /**
     * @brief 射撃に関するエフェクトとサウンドを更新します。
     * @param mob 射撃を行う Mob インスタンス
     * @param effect エフェクトマネージャ
     * @param eyeHandle 予告サイン用エフェクトハンドル
     * @param flashHandle 発射フラッシュエフェクトハンドル
     * @param pos エフェクト再生位置
     * @param forward 射撃方向ベクトル
     * @param bullets 残弾数（参照で更新）
     * @param eyeFxPlayed 予告サイン再生済みフラグ（参照で更新）
     * @param shotSignPlayed 射撃サイン音再生済みフラグ（参照で更新）
     */
    static void UpdateShootingEffects(
        std::shared_ptr<Mob> mob,
        shared_ptr<EffectManager> effect,
        Effekseer::Handle& eyeHandle,
        Effekseer::Handle& flashHandle,
        const Vec3& pos,
        const Vec3& forward,
        int& bullets,
        bool& eyeFxPlayed,
        bool& shotSignPlayed) {

        float& interval = mob->m_BalletInterval;
        float& randomIntvl = mob->m_ShotRandomInterval;
        const float maxIntv = mob->MAX_BALLET_INTERVAL;

        // 1) エフェクト予告サイン
        if (interval < EFFECT_THRESHOLD1 &&
            randomIntvl < EFFECT_THRESHOLD1 &&
            !eyeFxPlayed) {
            effect->PlayEffect(
                eyeHandle,
                L"EnemyEye",
                Vec3(pos.x, pos.y + 0.5f, pos.z),
                0.0f
            );
            effect->SetRotation(
                eyeHandle,
                Vec3(0, 1, 0),
                atan2f(forward.x, forward.z)
            );
            eyeFxPlayed = true;
        }
        // 2) 攻撃サイン音
        else if (interval < EFFECT_THRESHOLD2 &&
            randomIntvl < EFFECT_THRESHOLD2 &&
            !shotSignPlayed) {
            SoundManager::GetInstance().PlaySE(L"SE_ATTACK_SIGN", 1.0f);
            shotSignPlayed = true;
        }
        // 3) 実弾発射
        else if (interval <= 0.0f &&
            randomIntvl <= 0.0f) {
            // フラッシュエフェクト
            effect->PlayEffect(
                flashHandle,
                L"Flash",
                Vec3(
                    pos.x + forward.x * 0.5f,
                    pos.y + 0.25f,
                    pos.z + forward.z * 0.5f
                ),
                8.0f
            );
            effect->SetRotation(
                flashHandle,
                Vec3(0, 1, 0),
                atan2f(forward.x, forward.z)
            );
            effect->SetScale(
                flashHandle,
                Vec3(0.1f, 0.1f, 0.1f)
            );

            // 弾を生成
            Vec3 dir = mob->GetDirectionToIntruderObject(mob->m_Intruder);
            mob->GetStage()->AddGameObject<Bullet>(
                mob->GetTransform()->GetPosition() + dir * mob->m_MuzzleOffset,
                mob->m_BalletSpeed,
                dir,
                mob->m_BalletRange,
                mob->GetThis<Enemy>()
            );

            // 発射後リセット
            eyeFxPlayed = false;
            shotSignPlayed = false;
            interval = maxIntv;
            randomIntvl = maxIntv;
            --bullets;
            SoundManager::GetInstance().PlaySE(L"SE_SHOT");
        }
    }

    //--------------------------------------------------
    // MobSearch
    //--------------------------------------------------

    /**
     * @brief MobSearch ステートに入ったときの初期化処理
     */
    void MobSearch::Enter() {
        EnemyState::Enter();
        auto mob = std::dynamic_pointer_cast<Mob>(m_Enemy);
        m_Path.clear();
    }

    /**
     * @brief MobSearch ステートの毎フレーム実行処理
     */
    void MobSearch::Execute() {
        auto mob = std::dynamic_pointer_cast<Mob>(m_Enemy);
        float dt = DeltaTime();

        // 最も近い市民探索
        Vec3 dir;
        float dist;
        auto target = FindNearest(L"Citizen", dir, dist);

        // プレイヤー検知
        if (IsAlert()) {
            mob->ChangeState<MobAlert>();
            return;
        }
        mob->SetAnim(L"Walk", 0.0f);

        // 追跡ロジック
        if (target) {
            RotateTo(dir);
            if (mob->m_kariState == Mob::kariState::hakai &&
                dist < mob->m_BalletRange) {
                mob->ChangeState<MobAlert>();
                return;
            }
            Vec3 step = dir.normalize()
                * dt
                * mob->m_ZoneElapsedTime
                * (static_cast<int>(GameManager::GetInstance().GetDifficulty()) * 2);
            mob->SetPosition(mob->GetPosition() + step);
        }
        else {
            Vec3 pdir = m_Player->GetPosition() - mob->GetPosition();
            RotateTo(pdir);
            if (pdir.length() > mob->m_BalletRange) {
                Vec3 step = pdir.normalize()
                    * dt
                    * mob->m_ZoneElapsedTime
                    * (static_cast<int>(GameManager::GetInstance().GetDifficulty()) * 2);
                mob->SetPosition(mob->GetPosition() + step);
            }
        }
    }

    /**
     * @brief MobSearch ステートを抜けるときの処理
     */
    void MobSearch::Exit() {
    }

    //--------------------------------------------------
    // MobAlert
    //--------------------------------------------------

    /**
     * @brief MobAlert ステートに入ったときの初期化処理
     */
    void MobAlert::Enter() {
        EnemyState::Enter();
        auto mob = std::dynamic_pointer_cast<Mob>(m_Enemy);

        // 弾数・インターバル初期化
        m_BulletRemain = mob->m_BulletRemain;
        mob->m_ShotRandomInterval = mob->MAX_BALLET_INTERVAL * 0.5f;

        // エフェクトマネージャ取得
        auto stage = std::static_pointer_cast<GameStage>(m_Stage);
        m_Effect = stage ? stage->GetCreateEffect() : nullptr;
    }

    /**
     * @brief MobAlert ステートの毎フレーム実行処理
     */
    void MobAlert::Execute() {
        auto mob = std::dynamic_pointer_cast<Mob>(m_Enemy);
        float deltaTime = DeltaTime();

        // 最も近い市民探索
        Vec3 dir;
        float dist;
        auto target = FindNearest(L"Citizen", dir, dist);

        // 警戒時
        if (IsAlert()) {
            // 回避中処理
            if (mob->m_IsAvoiding) {
                mob->m_AvoidTime -= App::GetApp()->GetElapsedTime();
                if (mob->m_AvoidTime > 0.0f) {
                    mob->SetMoveDirection(mob->m_AvoidDirection);
                    return;
                }
                mob->m_IsAvoiding = false;
            }

            // 障害物回避 or プレイヤー追跡
            HandleObstacleAvoidance(mob, m_Stage.get());
            RotateTo(mob->GetDirectionToIntruderObject(m_Player));
        }
        // 市民追跡
        else if (target) {
            mob->AlartMove(target);
            RotateTo(dir);
            if (dist > mob->m_BalletRange) {
                mob->ChangeState<MobSearch>();
                return;
            }
        }
        // 探索に戻す
        else {
            mob->ChangeState<MobSearch>();
            return;
        }

        mob->SetAnim(L"Set", 0.0f);

        // 射撃 or リロード
        if (m_BulletRemain > 0) {
            UpdateShootingEffects(
                mob,
                m_Effect,
                m_EyeHandle,
                m_FlashHandle,
                mob->GetPosition(),
                mob->GetForward(),
                m_BulletRemain,
                m_BulletEffect,
                m_BulletSound
            );
        }
        else {
            // リロードアニメ
            mob->SetAnim(L"Reload", 0.0f);
            m_BulletReloadTime -= deltaTime;
            if (m_BulletReloadTime <= 0.0f) {
                mob->m_BalletInterval = mob->MAX_BALLET_INTERVAL;
                mob->m_ShotRandomInterval = 1.0f;
                m_BulletRemain = mob->m_BulletRemain;
                m_BulletEffect = m_BulletSound = false;
                m_BulletReloadTime = RELOAD_DURATION;
            }
        }

        // エフェクト速度同期
        if (m_Effect) {
            m_Effect->SetEffectSpeed(
                m_EyeHandle, GameManager::GetInstance().GetTimeRate());
            m_Effect->SetEffectSpeed(
                m_FlashHandle, GameManager::GetInstance().GetTimeRate());
        }
    }

    /**
     * @brief MobAlert ステートを抜けるときの処理
     */
    void MobAlert::Exit() {
        auto mob = std::dynamic_pointer_cast<Mob>(m_Enemy);
        mob->SetAnim(L"SetDown", 0.0f);
        m_Effect->StopEffect(m_EyeHandle);
        m_Effect->StopEffect(m_FlashHandle);
    }

} // namespace basecross