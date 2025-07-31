/*!
 @file   MobState.h
 @brief  Mob 用ステート管理
*/

#pragma once
#include "stdafx.h"
#include "Mob.h"
#include <cfloat>
#include <vector>

namespace basecross {
    static constexpr float AVOID_DURATION = 0.8f;
    static constexpr float RELOAD_DURATION = 3.0f;
    static constexpr float EFFECT_THRESHOLD1 = 0.4f;
    static constexpr float EFFECT_THRESHOLD2 = 0.2f;

    //―――――――――――――――――――――――――――
    // EnemyState（Mob 用ステートの基底）
    //―――――――――――――――――――――――――――
    template<typename T>
    class EnemyState {
    protected:
        std::shared_ptr<T>        m_Enemy;
        std::shared_ptr<Stage>    m_Stage;
        std::shared_ptr<Transform> m_Transform;
        std::shared_ptr<Character> m_Player;

        // 経過秒（タイムレート込み）
        float DeltaTime() const {
            return App::GetApp()->GetElapsedTime()
                * GameManager::GetInstance().GetTimeRate();
        }

        // 指定グループの最接近オブジェクト探索
        std::shared_ptr<Object> FindNearest(
            const std::wstring& groupName,
            Vec3& outDir,
            float& outDist)
        {
            auto grp = m_Stage->GetSharedObjectGroup(groupName)
                ->GetGroupVector();
            Vec3 myPos = m_Enemy->GetPosition();
            outDist = FLT_MAX;
            std::shared_ptr<Object> nearest;

            for (auto& w : grp) {
                if (auto s = w.lock()) {
                    Vec3 d = s->GetComponent<Transform>()
                        ->GetPosition() - myPos;
                    float dist = d.length();
                    if (dist < outDist) {
                        outDist = dist;
                        outDir = d;
                        nearest = std::dynamic_pointer_cast<Object>(s);
                    }
                }
            }
            return nearest;
        }

        // Y軸回転
        void RotateTo(const Vec3& dir) {
            float ang = atan2f(dir.x, dir.z);
            m_Transform->SetRotation({ 0, ang, 0 });
        }

        // プレイヤー検知フラグ
        bool IsAlert() const {
            return m_Enemy->GetIntruderAlert();
        }

    public:
        EnemyState(std::shared_ptr<T>& enemy)
            : m_Enemy(enemy)
        {
        }

        virtual ‾EnemyState() = default;

        virtual void Enter() {
            m_Stage = m_Enemy->GetStage();
            m_Transform = m_Enemy->GetComponent<Transform>();
            m_Player = m_Enemy->m_Intruder;
        }
        virtual void Execute() = 0;
        virtual void Exit() = 0;
    };

    //―――――――――――――――――――――――――――
    // MobSearch
    //―――――――――――――――――――――――――――
    class MobSearch : public EnemyState<Mob> {
        std::vector<Vec3> m_Path;

    public:
        MobSearch(std::shared_ptr<Mob>& enemy)
            : EnemyState<Mob>(enemy)
        {
        }

        void Enter()   override;
        void Execute() override;
        void Exit()    override;
    };

    //―――――――――――――――――――――――――――
    // MobAlert
    //―――――――――――――――――――――――――――
    class MobAlert : public EnemyState<Mob> {
        int   m_BulletRemain = 0;
        float m_BulletReloadTime = RELOAD_DURATION;
        bool  m_BulletEffect = false;
        bool  m_BulletSound = false;

        std::shared_ptr<EffectManager> m_Effect;
        Effekseer::Handle              m_EyeHandle = -1;
        Effekseer::Handle              m_FlashHandle = -1;

    public:
        MobAlert(std::shared_ptr<Mob>& enemy)
            : EnemyState<Mob>(enemy)
        {
        }

        void Enter()   override;
        void Execute() override;
        void Exit()    override;
    };

} // namespace basecross