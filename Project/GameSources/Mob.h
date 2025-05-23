/*!
@file Enemy.h
@brief
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	template <typename> class EnemyState;

	class Enemy;
	class Tube;
	class HPBar;
	class RootPointer;

	class Mob : public Enemy
	{
	public:

		int m_MobState;
		enum State {
			ALER = 0b00000001,
			WALK = 0b00000010,
			ATTACK = 0b00000100
		};
		int m_kariState;
		enum kariState {
			musi = 0b00000001,
			hakai = 0b00000010,
		};
		float m_BalletSpeed;
		float m_BalletRange;
		float m_MuzzleOffset;
		float m_BalletInterval;
		float m_KnockBackInterval;
		const float MAX_BALLET_INTERVAL;
		float m_ShotRandomInterval;
		bool m_IntervalStart;
		Vec3 m_Before = Vec3(0);
		int m_BulletRemain;
		shared_ptr<SharpFan> m_SearchFan;
		shared_ptr<HPBar> m_HpBar;
		shared_ptr<GameObject> m_NearPoint;
		vector<shared_ptr <GameObject>> m_PointData;
		shared_ptr<GameObject> m_BeforPoint;
	
		unique_ptr<EnemyState<Mob>> m_currentState;  //現在のステート
		unique_ptr<EnemyState<Mob>> m_nextState;     //次のステート

		bool m_Update;
	public:
		Mob(const shared_ptr<Stage>& stage);
		Mob(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		~Mob();
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnAfterCreate()override;
		virtual void AsyncUpdate()override;

		virtual void Dead()override;
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		void AddAnimation();
		Vec3 RootNaviGate();
		shared_ptr<Stage> GetStage();
		shared_ptr<Transform> GetTransfrom();
		float GetZoomTime() {
			return m_ZoneElapsedTime;
		}
		shared_ptr<GameObject> GetNearPoint()
		{
			return m_NearPoint;
		}
		vector<shared_ptr <GameObject>> GetPointData()
		{
			return m_PointData;
		}

		void AlartMove(shared_ptr<Object> obj);

		template <class NextState>
		void ChangeState() {
			m_currentState->Exit();
			m_currentState.reset();
			m_currentState = make_unique<NextState>(GetThis<Mob>());
			m_currentState->Enter();
		}

		const void SetAnim(wstring animname,float time = 0.0f, bool enforce = false) {
			auto draw = GetComponent<BcPNTBoneModelDraw>();
			if (draw->GetCurrentAnimation() != animname)
				if (draw->GetAnimeLoop()) draw->ChangeCurrentAnimation(animname, time);
				else
				{
					if (draw->IsTargetAnimeEnd() || enforce) draw->ChangeCurrentAnimation(animname, time);
				}
		}

	private:

		float WstrToFlt(const wstring& data) {
			if (data == L"") return NULL;
			return stof(data);
		}

		vector<int> WstrToVecInt(const wstring& data) {
			vector<wstring> vec3Str = {};
			vector<int> num = {};
			Util::WStrToTokenVector(vec3Str, data, L'_');
			for (int i = 0; i < vec3Str.size(); i++)
			{
				if (WstrToFlt(vec3Str[i]) != NULL)
				{
					num.push_back(WstrToFlt(vec3Str[i]));
				}
			}
			return num;
		}
	};
}
//end basecross
