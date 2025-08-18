/*!
 @file PlayerAttack.cpp
 @brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Player.h"

namespace basecross {

    // BaseHitObject: ヒット判定オブジェクトの基底クラス
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
        // オブジェクト破棄時にメインエフェクトを停止してリソース解放
        m_Effect->StopEffect(m_MainHandle);
    }

    void BaseHitObject::OnCreate() {
        // Transformコンポーネントの追加と初期位置・スケール設定
        auto t = AddComponent<Transform>();
        t->SetPosition(m_Position);
        t->SetScale(m_Scale);

        // ステージからエフェクトマネージャを取得
        m_Effect = static_pointer_cast<GameStage>(GetStage())->GetCreateEffect();
    }

    void BaseHitObject::OnUpdate() {
        // 毎フレーム、ゲーム速度に応じてエフェクト再生速度を同期
        float speed = GameManager::GetInstance().GetGameSpeed();
        m_Effect->SetEffectSpeed(m_MainHandle, speed);
        m_Effect->SetEffectSpeed(m_HitHandle, speed);

        // 派生クラスで移動や寿命管理を行う
    }

    void BaseHitObject::OnCollisionEnter(shared_ptr<GameObject>& other) {
        // 敵タグを持つオブジェクトとの当たり判定のみ処理
        if (!other || !other->FindTag(L"Enemy")) return;

        // Characterクラスへのキャストチェック
        auto enemy = dynamic_pointer_cast<Character>(other);
        if (!enemy) return;

        // ダメージ適用とヒットエフェクト再生
        ApplyHit(enemy);
        if (m_Effect && m_HitHandle != -1) {
            m_Effect->PlayEffect(m_HitHandle, L"HitEffect", enemy->GetPosition(), 0.0f);
            float rot = atan2f(enemy->GetRotation().x, enemy->GetRotation().z);
            m_Effect->SetRotation(m_HitHandle, Vec3(0, 1, 0), rot);
        }

        // コントローラ振動と停止イベントを予約
        XINPUT_VIBRATION vib{ 65535, 65535 };
        XInputSetState(0, &vib);
        PostEvent(0.25f, nullptr, GetStage(), L"StopVibration");
    }


    // ---------------------------------------------------------------------------
    // HitSphere: 前方に飛ばす通常攻撃の当たり判定オブジェクト
    // ---------------------------------------------------------------------------
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
        // ZONE中は飛行時間を長めに設定、それ以外は短く設定
        bool inZone = (static_pointer_cast<Player>(m_Player)->GetStates()
            & Player::PlayerState::ZONE) != 0;
        m_FlyingTime = inZone ? 1.0f : 0.1f;
        // 飛行距離を飛行時間で割って速度を求める
        m_Speed = length / m_FlyingTime;
    }

    void HitSphere::ApplyHit(shared_ptr<Character> enemy) {
        // ヒット時にプレイヤーのチャージを回復し、ダメージを与える
        auto player = static_pointer_cast<Player>(m_Player);
        player->SetCharge(0.1f);
        enemy->Damage(player->GetAttackDamage(), false);
    }

    void HitSphere::OnCreate() {
        BaseHitObject::OnCreate();

        // 衝突判定用の球コリジョンを追加
        auto col = AddComponent<CollisionSphere>();
        col->SetDrawActive(GameManager::GetInstance().IsDebug());
        col->SetFixed(false);
        col->SetAfterCollision(AfterCollision::None);

        // ジャッジ用タグを設定
        AddTag(L"HitJudge");

        // ZONE中はメインエフェクトを再生
        bool inZone = (static_pointer_cast<Player>(m_Player)->GetStates()
            & Player::PlayerState::ZONE) != 0;
        if (!inZone) return;

        m_Effect->PlayEffect(m_MainHandle, L"Panchi", m_Position, 20.0f);
        float rot = atan2f(m_Direction.x, m_Direction.z);
        m_Effect->SetRotation(m_MainHandle, Vec3(0, 1, 0), rot);
        m_Effect->SetScale(m_MainHandle, m_Scale + 0.5f);
    }

    void HitSphere::OnUpdate() {
        BaseHitObject::OnUpdate();

        // 飛行時間中は前進、終了後にステージから除去
        float dt = App::GetApp()->GetElapsedTime()
            * GameManager::GetInstance().GetGameSpeed();
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


    // ---------------------------------------------------------------------------
    // ChargeHitSphere: 拡大しながらダメージを与えるチャージ攻撃用オブジェクト
    // ---------------------------------------------------------------------------
    ChargeHitSphere::ChargeHitSphere(
        const shared_ptr<Stage>& stage,
        const Vec3& position,
        const Vec3& scale,
        const shared_ptr<GameObject>& player,
        float chargeTime
    ) :
        BaseHitObject(stage, position, scale, player),
        m_TotalTime(1.0f),
        // チャージ時間をスケール増加率に変換
        m_ChargeRate(chargeTime * 10.0f)
    {
    }

    void ChargeHitSphere::ApplyHit(shared_ptr<Character> enemy) {
        // 通常より少ないダメージを与え、Boss以外は追加ヒットを除外
        auto player = static_pointer_cast<Player>(m_Player);
        player->SetCharge(0.1f);
        enemy->Damage(player->GetAttackDamage() / 4, false);

        if (!enemy->FindTag(L"Boss")) {
            GetComponent<CollisionCapsule>()->AddExcludeCollisionGameObject(enemy);
        }
    }

    void ChargeHitSphere::OnCreate() {
        BaseHitObject::OnCreate();

        // カプセルコリジョンを使用
        auto col = AddComponent<CollisionCapsule>();
        col->SetDrawActive(GameManager::GetInstance().IsDebug());
        col->SetFixed(false);
        col->SetAfterCollision(AfterCollision::None);

        AddTag(L"CaargeHitJudge");
    }

    void ChargeHitSphere::OnUpdate() {
        BaseHitObject::OnUpdate();

        // 生成からの経過時間で寿命を管理し、スケールを徐々に増加
        float dt = App::GetApp()->GetElapsedTime()
            * GameManager::GetInstance().GetGameSpeed();
        m_TotalTime -= dt;

        if (m_TotalTime <= 0.0f) {
            GetStage()->RemoveGameObject<ChargeHitSphere>(GetThis<ChargeHitSphere>());
            return;
        }

        m_Scale += Vec3(m_ChargeRate * dt, 0.01f, m_ChargeRate * dt);
        GetComponent<Transform>()->SetScale(m_Scale);
    }


    // ---------------------------------------------------------------------------
    // CounterHitSphere: プレイヤーに付着して攻撃を反撃するカウンターオブジェクト
    // ---------------------------------------------------------------------------
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
        m_ChargeRate(chargeRate),
        m_CounterHandle(-1)
    {
    }

    CounterHitSphere::‾CounterHitSphere()
    {
        // カウンター用エフェクトも停止
        m_Effect->StopEffect(m_CounterHandle);
    }

    void CounterHitSphere::ApplyHit(shared_ptr<Character> enemy) {
        // 通常攻撃ダメージを与え、カウンター停止イベントを発行
        auto player = static_pointer_cast<Player>(m_Player);
        player->SetCharge(0.1f);
        enemy->Damage(player->GetAttackDamage(), false);
        PostEvent(0.0f, GetThis<ObjectInterface>(), m_Stage, L"ContorStop");
    }

    void CounterHitSphere::OnCreate() {
        BaseHitObject::OnCreate();

        // プレイヤー自身やAttackタグのオブジェクトは除外
        auto col = AddComponent<CollisionSphere>();
        col->SetDrawActive(GameManager::GetInstance().IsDebug());
        col->SetAfterCollision(AfterCollision::None);
        col->AddExcludeCollisionGameObject(m_Player);
        col->AddExcludeCollisionTag(L"Attack");

        AddTag(L"CounterHitJudge");

        // 初期位置をプレイヤー＋オフセットに設定
        auto t = GetComponent<Transform>();
        Vec3 playerPos = m_Player->GetComponent<Transform>()->GetPosition();
        t->SetPosition(playerPos + m_LocalOffset);

        // カウンターエフェクトをプレイヤー前方に再生
        auto forward = m_Player->GetComponent<Transform>()->GetForward();
        float rotate = atan2f(-forward.x, -forward.z);
        m_Effect->PlayEffect(m_CounterHandle, L"Counter",
            Vec3(
                m_Position.x + (forward.x * 5),
                m_Position.y + 0.25f,
                m_Position.z + (forward.z * 5)
            ),
            0.0f);
        m_Effect->SetRotation(m_CounterHandle, Vec3(0.0f, 1.0f, 0.0f), rotate);
        m_Effect->SetScale(m_CounterHandle,
            m_Player->GetComponent<Transform>()->GetScale() / 5);
    }

    void CounterHitSphere::OnUpdate() {
        // ベースでエフェクト速度を同期
        BaseHitObject::OnUpdate();

        float dt = App::GetApp()->GetElapsedTime()
            * GameManager::GetInstance().GetGameSpeed();
        m_Elapsed += dt;

        // 付着時間が経過したらオブジェクトを削除
        if (m_Elapsed >= m_AttachDuration) {
            GetStage()->RemoveGameObject<CounterHitSphere>(GetThis<CounterHitSphere>());
            return;
        }

        // プレイヤー位置＋オフセットに追従
        Vec3 playerPos = m_Player->GetComponent<Transform>()->GetPosition();
        Vec3 newPos = playerPos + m_LocalOffset;
        Vec3 forward = m_Player->GetComponent<Transform>()->GetForward();
        GetComponent<Transform>()->SetPosition(newPos.x, newPos.y, newPos.z);
        m_Effect->SetLocation(m_CounterHandle, newPos + forward);

        // 時間経過で半径（スケール）を徐々に拡大
        float deltaR = m_ChargeRate * dt;
        m_Scale += Vec3(deltaR);
        GetComponent<Transform>()->SetScale(m_Scale);
    }

} // namespace basecross