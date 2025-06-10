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
		Line() : m_Start(Vec3()),m_End(Vec3()){}
		Line(Vec3 start, Vec3 end) : m_Start(start), m_End(end) {}
		Line(Vec3 start, Vec3 direction, float distance) : m_Start(start) {
			m_End = start + direction * distance;
		}
		Vec3 GetDirection() const{
			return m_End - m_Start;
		}
		float GetLength() const{
			return GetDirection().length();
		}
		void SetMaxLength(float length) {
			if (GetLength() > length) {
				m_End = m_Start + GetDirection() * length;
			}
		}
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
	struct RayCastElement {
		Vec3 startLine;
		float pad;
		Vec3 endLine;
	};
	class RayCast {
		static vector<RayCast> m_RayCasts;
		
		void Init(){}
		static bool HitTestMeshRayCast(const Line& line, RayCastHit& hit, const shared_ptr<GameObject>& object);
	public:
		static int count;
		static bool HitTest(RayCastHit& hit, const Line& line, shared_ptr<GameObject>& object, const vector<wstring> excludeTags = {});
		static bool HitTestVec(RayCastHit& hit, const Line& line, vector<shared_ptr<GameObject>>& vec, const vector<wstring>& excludeTags = {},const shared_ptr<GameObject>& excludeObject = nullptr);

		static float CalcDistancePointToLine(const Vec3& point, const Line& line);
		static float CalcDistancePoint(const Vec3& point, const Line& line);

		static void CreateRayCast(int size);
	};



	struct BVHNode {
		AABB bounds;
		shared_ptr<BVHNode> leftNode;
		shared_ptr<BVHNode> rightNode;

		vector<TRIANGLE> triangles;

		bool isIntersect = true;

		BVHNode() {
			bounds = AABB();
		}
		bool IsLeaf() {
			return triangles.size() > 0;
		}
		bool IsIntersect(const Line& line) {
			if (!isIntersect) return false;

			isIntersect = HitTest::SEGMENT_AABB(line.m_Start, line.m_End, bounds);
			return isIntersect;
		}
	};
	class BVH {
	public:
		static AABB TriangleBounds(vector<TRIANGLE> triangle);
		static bool HitRayCast(shared_ptr<BVHNode>& node,RayCastHit& hit, const Line& line);
		static shared_ptr<BVHNode> BuildBVH(vector<TRIANGLE> triangle, int depth, int max);
	};
}
//end basecross
