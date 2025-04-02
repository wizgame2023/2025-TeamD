/*!
@file Character.h
@brief 
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	struct Line {
		Vec3 m_Start;
		Vec3 m_End;

		Line(Vec3 start, Vec3 end) : m_Start(start), m_End(end) {}
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
		static bool HitTest(RayCastHit& hit, const Line& line, shared_ptr<GameObject>& object, const vector<wstring> excludeTags = {}, const bool& isDebug = false);
		static float CalcDistancePointToLine(const Vec3& point, const Line& line);
		static float CalcDistancePoi(const Vec3& point, const Line& line);

	};
}
//end basecross
