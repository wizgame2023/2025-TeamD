/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {

	class Character : public Object
	{
	public:
		int m_HP;
		Character(const shared_ptr<Stage>& stage, Vec3 position, Vec3 rotation, Vec3 scale) :
			Object(stage, position, rotation, scale), m_HP(0) {
		}
		Character(const shared_ptr<Stage>& stage) :
			Character(stage, Vec3(), Vec3(), Vec3(1.0f))
		{
		};
		virtual ‾Character() {};

		virtual void OnCreate() override;

		virtual void Dead() {}

		double AngleBetweenVectors(const Vec3& v1, const Vec3& v2);
		bool IsWithinDetectionRange(const Vec3& direction, const Vec3& target, double angle);
		double DotProduct(const Vec3& v1, const Vec3& v2);
		double Magnitude(const Vec3& v);
	};

	class FixedBox : public Object
	{
		bool Wicth_FixedBox;
	public:
		FixedBox(const shared_ptr<Stage>& stage);
		‾FixedBox();
		virtual void OnCreate() override;
	};

	class Wall : public GameObject
	{
		bool Wicth_Wall;
	public:
		Wall(const shared_ptr<Stage>& stage);
		‾Wall();
		virtual void OnCreate() override;

	};
}

//end basecross
