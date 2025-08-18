/*!
 @file Enemy.cpp
 @brief Mob（敵キャラクター）クラス実装
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

    //----------------------------------------------------------------------  
    // コンストラクタ／デストラクタ  
    //----------------------------------------------------------------------  

    // デフォルトコンストラクタ:
    // ステージのみ指定し、位置とスケールはデフォルト値(Vec3(), Vec3(1.0f))で初期化
    Mob::Mob(const shared_ptr<Stage>& stage)
        : Mob(stage, Vec3(), Vec3(1.0f))
    {
    }

    // 引数付きコンストラクタ:
    // Enemy基底クラスを初期化しつつ各メンバ変数を設定
    Mob::Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale)
        : Enemy(stage, position, scale),
        m_BalletInterval(0.5f),
        MAX_BALLET_INTERVAL(1.0f),
        m_ShotRandomInterval(1.0f),
        m_BalletSpeed(50.0f),
        m_MuzzleOffset(1.5f),
        m_BalletRange(10.0f),
        m_IntervalStart(false),
        m_KnockBackInterval(2.0f),
        m_NearPoint(nullptr),
        m_BulletRemain(10),
        m_Update(true)
    {
    }

    // デストラクタ: 必要ならリソース解放を実装
    Mob::‾Mob() {}

    //----------------------------------------------------------------------  
    // OnCreate()  
    //----------------------------------------------------------------------  

    void Mob::OnCreate()
    {
        // 基底クラスの初期化処理を呼び出し
        Enemy::OnCreate();

        // 初期ステートを設定（仮破壊ステート）
        m_kariState = kariState::hakai;

        // シーン内のPlayerオブジェクトを検索してターゲットに登録
        auto player = m_Stage->GetSharedGameObject<Player>(L"Player", false);
        if (player != nullptr) {
            SetIntruder(player);
        }

        // DissolveDraw コンポーネントを追加して描画設定
        auto ptrDraw = AddComponent<DissolveDraw>();
        Mat4x4 meshMat;
        meshMat.affineTransformation(
            Vec3(0.25f),                // スケーリング
            Vec3(0.0f, 90.0f, 0.0f),    // 回転（度）
            Vec3(0.0f, XM_PI, 0.0f),    // 回転（ラジアン）
            Vec3(0.0f, -0.95f, 0.0f)    // 平行移動
        );
        ptrDraw->SetMeshResource(L"MOB");                      // メッシュリソース指定
        ptrDraw->SetMeshToTransformMatrix(meshMat);            // モデルとTransform行列を紐付け
        ptrDraw->SetBlendState(BlendState::AlphaBlend);        // ブレンドモード設定
        ptrDraw->SetBoneState(BoneState::Bone);                // ボーンアニメーション有効化
        ptrDraw->SetDissolveColor(Col4(0.0f, 0.5f, 1.0f, 1.0f)); // 解ける色設定
        ptrDraw->SetDissolve(1.0f);                            // 初期ディゾルブ量
        ptrDraw->SetNoiseTextureResource(L"NOISE");            // ノイズテクスチャ指定
        ptrDraw->SetDissolveActive(false);                     // 解ける演出はオフ

        // 重力コンポーネント追加
        auto ptrGra = AddComponent<Gravity>(Vec3());

        // シャドウマップ用コンポーネント追加
        auto shadowPtr = AddComponent<Shadowmap>();
        shadowPtr->SetMeshResource(L"MOB");
        shadowPtr->SetMeshToTransformMatrix(meshMat);

        // アニメーションシーケンスの登録
        AddAnimation();

        // 状態マシン（MobSearch）の生成→開始
        m_currentState = make_unique<MobSearch>(GetThis<Mob>());
        m_currentState->Enter();

        // タグを追加してグルーピング
        AddTag(L"Mob");

        // 透明度レンダリングを有効化
        SetAlphaActive(true);
    }

    //----------------------------------------------------------------------  
    // OnAfterCreate()  
    //----------------------------------------------------------------------  

    void Mob::OnAfterCreate()
    {
        // HPバーを生成し、最大HP・現在HPを設定
        m_HpBar = m_Stage->AddGameObject<HPBar>(
            GetThis<Mob>(),
            Vec3(GetScale().x * 0.25f, GetScale().y * 1.5f, 0)
        );
        m_HpBar->SetMaxHp(3);
        m_HpBar->SetCurrentHp(m_HP);
    }

    //----------------------------------------------------------------------  
    // OnUpdate()  
    //----------------------------------------------------------------------  

    void Mob::OnUpdate()
    {
        // 基底クラスの更新
        Enemy::OnUpdate();

        if (m_Update)
        {
            // ゲーム経過時間取得
            float elapsed = GetGameElapsed();

            // 非同期状態更新
            AsyncUpdate();

            // DissolveDraw のアニメーション更新
            auto draw = GetComponent<DissolveDraw>();
            draw->UpdateAnimation(elapsed);

            // 攻撃インターバル or ノックバックインターバルをカウントダウン
            if (m_IntervalStart)
            {
                m_BalletInterval -= elapsed * m_ZoneElapsedTime;
            }
            else
            {
                m_KnockBackInterval -= elapsed * m_ZoneElapsedTime;
                if (m_KnockBackInterval < 0)
                {
                    // ノックバック完了 → 攻撃モードへ移行
                    m_BalletInterval = 0.5f;
                    m_KnockBackInterval = 2.0f;
                    m_IntervalStart = true;
                }
            }

            // 攻撃タイミング到来時の乱数インターバル更新
            if (m_BalletInterval < 0)
            {
                m_BalletInterval = 0;
                m_ShotRandomInterval -= elapsed * m_ZoneElapsedTime;
                if (m_ShotRandomInterval <= 0)
                {
                    m_ShotRandomInterval = 0;
                }
            }

            // HPバーの表示更新
            m_HpBar->SetCurrentHp(m_HP);
        }
        else
        {
            // 更新停止中でもディゾルブアニメのみ再生
            float elapsed = GetGameElapsed();
            auto draw = GetComponent<DissolveDraw>();
            draw->UpdateAnimation(elapsed);
        }
    }

    //----------------------------------------------------------------------  
    // OnSpawn()  
    //----------------------------------------------------------------------  

    void Mob::OnSpawn()
    {
        // 基底クラス呼び出し（フェードイン演出など）
        Enemy::OnSpawn();

        // DisolveDraw を逆再生設定してフェードイン開始
        auto draw = GetComponent<DissolveDraw>();
        draw->SetDissolveSpeed(-1.0f);
        draw->SetDissolveActive(true);
    }

    //----------------------------------------------------------------------  
    // AsyncUpdate()  
    //----------------------------------------------------------------------  

    void Mob::AsyncUpdate()
    {
        StartAsync();                     // 非同期処理開始
        Vec3 none = Vec3(0);
        Vec3 currntPosition = m_Transform->GetPosition();

        // 基底クラスの非同期更新
        Enemy::AsyncUpdate();

        // 現在ステートの Execute 呼び出し
        m_currentState->Execute();

        EndAsync();                       // 非同期処理終了
    }

    //----------------------------------------------------------------------  
    // Dead()  
    //----------------------------------------------------------------------  

    void Mob::Dead()
    {
        // 更新停止フラグ
        m_Update = false;

        // DissolveDraw の取得
        auto draw = GetComponent<DissolveDraw>();

        // 探索エフェクトとHPバーを破棄
        m_Stage->RemoveGameObject<SharpFan>(m_SearchFan);
        m_HpBar->Destroy();

        // ノックバック中はDownアニメ再生＋押し戻し処理
        float elapsedTime = GetGameElapsed();
        m_KnockBackTime -= elapsedTime;
        if (m_KnockBackTime > 0.0f)
        {
            SetAnim(L"Down", 0.0f);
            KnockBackTime();
        }
        else if (draw->IsTargetAnimeEnd())
        {
            // アニメ完了後フェードアウト設定・当たり判定削除
            draw->SetDissolveSpeed(2.0f);
            draw->SetDissolveActive(true);
            RemoveComponent<CollisionCapsule>();
            RemoveComponent<Gravity>();
        }

        // フェードアウト完了時のクリーンアップ
        if (draw->GetDissolveAmount() >= 1.0f)
        {
            // スコア加算
            ScoreManager::Instance()->AddEliminateEnemyCount();

            // EnemyGroup から自身を除去
            auto group = m_Stage->GetSharedObjectGroup(L"EnemyGroup");
            auto& groupVec = group->GetGroupVectors();
            for (int i = 0; i < groupVec.size(); i++)
            {
                auto obj = groupVec[i].lock();
                if (obj && obj == GetThis<GameObject>())
                {
                    groupVec.erase(groupVec.begin() + i);
                    break;
                }
            }

            // Spawner に EnemyDead イベントを送信
            auto spawner = m_Stage->GetSharedGameObject<Spawner>(L"Spawner", false);
            if (spawner) {
                PostEvent(0.0f, GetThis<ObjectInterface>(), spawner, L"EnemyDead");
            }

            // ステージから自身を削除
            m_Stage->RemoveGameObject<Enemy>(GetThis<Enemy>());
        }
    }

    //----------------------------------------------------------------------  
    // OnCollisionEnter()  
    //----------------------------------------------------------------------  

    void Mob::OnCollisionEnter(shared_ptr<GameObject>& other)
    {
        // 弾丸やヒット判定タグと、インターバル中のみダメージ処理
        if ((other->FindTag(L"Bullet") ||
            other->FindTag(L"HitJudge") ||
            other->FindTag(L"CaargeHitJudge") ||
            other->FindTag(L"CounterHitJudge"))
            && m_IntervalStart)
        {
            // ダメージアニメ再生＋基底クラスコール＋ノックバックモードへ
            SetAnim(L"Damage", 0.0f, true);
            Enemy::OnCollisionEnter(other);
            m_IntervalStart = false;
        }
    }

    //----------------------------------------------------------------------  
    // AddAnimation()  
    //----------------------------------------------------------------------  

    void Mob::AddAnimation()
    {
        // 各アニメーションシーケンスを登録
        auto ptrDraw = GetComponent<DissolveDraw>();
        float anim_fps = 60.0f;
        ptrDraw->AddAnimation(L"Walk", 20, 170, true, anim_fps);
        ptrDraw->AddAnimation(L"SetUp", 288, 72, false, anim_fps);
        ptrDraw->AddAnimation(L"Set", 318, 30, true, anim_fps);
        ptrDraw->AddAnimation(L"SetDown", 361, 50, false, anim_fps);
        ptrDraw->AddAnimation(L"Down", 557, 93, false, anim_fps);
        ptrDraw->AddAnimation(L"Damage", 557, 25, false, anim_fps);
        ptrDraw->AddAnimation(L"Reload", 661, 103, false, anim_fps);
    }

    //----------------------------------------------------------------------  
    // RootNaviGate()  
    //----------------------------------------------------------------------  

    Vec3 Mob::RootNaviGate()
    {
        // 指定ポイント群からランダムで移動先を選択し返却
        auto pointerGroup = GetStage()->GetSharedObjectGroup(L"PointerGroup");
        auto pointers = pointerGroup->GetGroupVector();

        // 前回と異なるポイントを選ぶまでループ
        for (auto point : pointers)
        {
            int rndIndex = static_cast<int>(Util::RandZeroToOne() * (pointers.size() - 1));
            auto candidate = m_PointData[rndIndex];
            if (point.lock() == candidate) {
                continue;
            }
            m_NearPoint = candidate;
            break;
        }

        // 次回比較用に記憶
        m_BeforPoint = m_NearPoint;

        // ポイントのワールド位置を返却
        return m_NearPoint->GetComponent<Transform>()->GetPosition();
    }

    //----------------------------------------------------------------------  
    // ユーティリティ関数  
    //----------------------------------------------------------------------  

    // ステージへの shared_ptr を返却
    shared_ptr<Stage> Mob::GetStage()
    {
        return m_Stage;
    }

    // Transform への shared_ptr を返却
    shared_ptr<Transform> Mob::GetTransfrom()
    {
        return m_Transform;
    }

    // 指定オブジェクトを向くようにY軸回転を設定
    void Mob::AlartMove(shared_ptr<Object> obj)
    {
        Vec3 target = obj->GetComponent<Transform>()->GetPosition();
        Vec3 position = m_Transform->GetPosition();
        Vec3 dir = target - position;
        dir.normalize();
        float rotateY = atan2f(dir.x, dir.z);
        SetRotation(Vec3(0, rotateY, 0));
    }

    // direction 方向に速度×経過時間分だけ移動
    void Mob::SetMoveDirection(const Vec3& direction)
    {
        // 無効な方向なら処理しない
        if (direction.length() < 0.001f)
            return;

        // 正規化済み移動ベクトル
        Vec3 targetDir = direction;
        // 経過時間×タイムレートを取得
        float deltaTime = App::GetApp()->GetElapsedTime()
            * GameManager::GetInstance().GetTimeRate();
        Vec3 pos = m_Transform->GetPosition();
        pos += targetDir * m_Speed * deltaTime;
        m_Transform->SetPosition(pos);
    }

} // namespace basecross