/*!
@file Character.h
@brief ƒLƒƒƒ‰ƒNƒ^[‚È‚Ç
*/

#pragma once
#include "stdafx.h"
namespace basecross {
	class Bullet;
	class LineCube : public GameObject {
	public:
		LineCube(shared_ptr<Stage>& stage) : GameObject(stage) {}
		virtual ~LineCube() {}

		virtual void OnCreate()override;

		shared_ptr<BcPNTStaticDraw> m_Draw;
		shared_ptr<Transform> m_Transform;
	};
	class ForecastLine : public GameObject
	{
		weak_ptr<GameObject> m_Launcher;

		shared_ptr<LineCube> m_BalletLine;
		shared_ptr<LineCube> m_Forecast;
		weak_ptr<Bullet> m_Bullet;
		weak_ptr<GameObject> m_NearestHitObject;
		//CRITICAL_SECTION m_CriticalSection;
		//HANDLE m_hThread;


		Vec3 m_Direction;
		Vec3 m_StartPosition;
		Col4 m_DefaultColor;
		float m_Length;
		bool m_IsLaunched;
		bool m_IsRay;
	public:
		ForecastLine(const shared_ptr<Stage>& stage, const shared_ptr<GameObject>& launcher, const bool& isRay = true) :
			GameObject(stage),
			m_Direction(Vec3()), m_StartPosition(Vec3()), m_Length(0), m_IsLaunched(false), m_Launcher(launcher), m_IsRay(isRay),m_DefaultColor(Col4(1,0,0,1))
		{
		};
		virtual ~ForecastLine() {};

		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		bool CheckRayCast(Vec3& hitPoint);
		bool CheckDistanceToObject(Vec3 position);
		void SetLine(const Vec3& direction, const Vec3& startPosition, const float maxLength,const Col4& color = Col4(1,0,0,1));
		bool CheckHitObjectTag(const wstring& tag) {
			shared_ptr<GameObject> obj = m_NearestHitObject.lock();
			if (obj != nullptr) {
				return obj->FindTag(tag);
			}
			return false;
		}
		void SetBallet(const shared_ptr<Bullet> ballet) {
			m_Bullet = ballet;
		}
		void Destroy();
	};

	struct RayCastHit {
		shared_ptr<GameObject> m_Object;
		Vec3 m_HitPosition;
		TRIANGLE m_Triangle;
		size_t m_TriangleIndex;

		RayCastHit() : m_HitPosition(Vec3()), m_Triangle(TRIANGLE()), m_TriangleIndex(0) {}

		RayCastHit& RayCastHit::operator =(const RayCastHit& other)
		{
			if (this != &other) {
				m_HitPosition = other.m_HitPosition;
				m_Triangle = other.m_Triangle;
				m_TriangleIndex = other.m_TriangleIndex;
			}
			return *this;
		}
	};
	class RayCast {
		
	public:
		static bool HitTest(RayCastHit& hit,const Vec3& startPosition, const Vec3& direction, float length, shared_ptr<GameObject>& object, const vector<wstring> excludeTags = {},const bool& isDebug = false);

	};
}
//end basecross
