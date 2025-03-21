/*!
@file Enemy.h
@brief 敵など
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class ForecastLine;
	class EnemyState;

	class Enemy : public Character
	{
	protected:
		shared_ptr<Character> m_Intruder;

		float m_ZoneElapsedTime;
		bool m_IntruderAlert;

		unique_ptr<EnemyState> m_currentState;  //現在のステート
		unique_ptr<EnemyState> m_nextState;     //次のステート

	public:
		Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		virtual ~Enemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void Dead();
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		void SetIntruder(const shared_ptr<Character>& character) {
			m_Intruder = character;
		}
		Vec3 GetDirectionToIntruder();
		float GetDistanceToIntruder();
		void ZoneSpeedSet();
		void SearchRange();

		Vec3 GetPosition();
		bool GetIntruderAlert();
	private:

	};

	//--------------------------------------------------------------------------------------
	//	class LineObject : public GameObject; //線を描画するオブジェクト
	//--------------------------------------------------------------------------------------
	class LineObject : public GameObject {
	private:
		Vec3 m_StartPos;
		Vec3 m_EndPos;
		Vec3 m_Direc;

		Col4 m_StartColor;
		Col4 m_EndColor;

		vector<VertexPositionColor> m_Vertices;
		vector<uint16_t> m_Indices;
		shared_ptr<PCStaticDraw> m_Draw;

		weak_ptr<Character> m_MainObject;
		weak_ptr<Character> m_Object;

		Vec2 m_StartSize;
		Vec2 m_EndSize;

	public:
		LineObject(const shared_ptr<Stage>& stage);
		LineObject(const shared_ptr<Stage>& stage, const shared_ptr<Character>& player, const shared_ptr<Character>& enemy);
		virtual ~LineObject() {}
		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		void SetLinePosition(const Vec3& startPos, const Vec3& endPos);
		void SetLineColor(const Col4& startColor, const Col4& endColor);
		void VerticesUpdate();
		Vec3 GetStartPos();
		Vec3 GetEndPos();
		Vec3 GetDirec();
		Vec3 LinePos(Vec3 pos);

		void SetStartSize(Vec2 size) {
			m_StartSize = size;
		}
		void SetEndSize(Vec2 size) {
			m_EndSize = size;
		}
	};


}
//end basecross
