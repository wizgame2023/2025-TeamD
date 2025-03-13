/*!
@file Enemy.h
@brief 敵など
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Character;
	class Enemy : public Character
	{
	protected:
		//入力ハンドラー
		Vec3 m_Position;
		Vec3 m_Rotation;
		Vec3 m_Scale;

		shared_ptr<Character> m_Intruder;
		shared_ptr<Transform> m_Transform;

		float m_ZoneElapsedTime;
		bool m_IntruderAlert;

	public:
		Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale);
		virtual ~Enemy();
		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnUpdate2();
		virtual void OnDraw();
		void OnCollisionEnter(shared_ptr<GameObject>& other);

		void SetIntruder(const shared_ptr<Character>& character) {
			m_Intruder = character;
		}
		Vec3 GetDirectionToIntruder();
		void ZoneSpeedSet();
		void SearchRange();
		Vec3 RotateVector(const Vec3& vector, double angle_degrees);
		double AngleBetweenVectors(const Vec3& v1, const Vec3& v2);

		double DotProduct(const Vec3& v1, const Vec3& v2) {
			return v1.x * v2.x + v1.z * v2.z;
		}

		double Magnitude(const Vec3& v) {
			return std::sqrt(v.x * v.x + v.z * v.z);
		}

		bool IsWithinDetectionRange(const Vec3& direction, const Vec3& target, double detection_angle_degrees);

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
	};


}
//end basecross
