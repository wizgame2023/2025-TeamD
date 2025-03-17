/*!
@file Character.h
@brief ƒLƒƒƒ‰ƒNƒ^[‚È‚Ç
*/

#pragma once
#include "stdafx.h"
namespace basecross{
	class Bullet;
	class LineCube : public GameObject {
	public:
		LineCube(shared_ptr<Stage>& stage) : GameObject(stage){}
		virtual ~LineCube(){}
		
		virtual void OnCreate()override;

		shared_ptr<BcPNTStaticDraw> m_Draw;
		shared_ptr<Transform> m_Transform;
	};
	class ForecastLine : public GameObject
	{

		weak_ptr<GameObject> m_Launcher;

		shared_ptr<LineCube> m_BalletLine;
		shared_ptr<LineCube> m_Forecast;

		Vec3 m_Direction;
		Vec3 m_StartPosition;
		float m_Length;
		bool m_IsLaunched;
	public:
		ForecastLine(const shared_ptr<Stage>& stage,const shared_ptr<GameObject>& launcher) :
			GameObject(stage),m_Direction(Vec3()),m_StartPosition(Vec3()),m_Length(0),m_IsLaunched(false),m_Launcher(launcher)
		{};
		virtual ~ForecastLine() {};

		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		void SetLine(const Vec3& direction, const Vec3& startPosition, const float maxLength);

		void SetBallet(const shared_ptr<Bullet> ballet) {
			m_Ballet = ballet;
		}
		void Destroy();
	};
}
//end basecross
