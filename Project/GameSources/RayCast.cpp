/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <chrono>
namespace basecross {
	vector<RayCast> RayCast::m_RayCasts = {};
	vector<shared_ptr<LineCube>> RayCast::m_DebugRay = {};
	int RayCast::count = 0;

	/// <summary>
	/// レイキャスト処理
	/// </summary>
	/// <param name="hit">結果</param>
	/// <param name="line">線分</param>
	/// <param name="object">調べるオブジェクト</param>
	/// <param name="excludeTags">除外するタグ</param>
	/// <returns>当たったか</returns>
	bool RayCast::HitTest(RayCastHit& hit, const Line& line, shared_ptr<GameObject>& object, const vector<wstring> excludeTags) {
		if (object == nullptr) return false;
		for (auto& tag : excludeTags) {
			if (object->FindTag(tag)) {
				return false;
			}
		}
		RayCastHit newResult = RayCastHit();
		if (HitTestMeshRayCast(line, newResult, object)) {
			if (hit.m_Object == nullptr) {
				hit.m_Object = object;
				hit = newResult;
			}
			else if ((hit.m_HitPosition - line.m_Start).length() > (newResult.m_HitPosition - line.m_Start).length()) {

				hit.m_Object = object;
				hit = newResult;
			}
			return true;
		}
		return false;
	}
	bool RayCast::HitTestVec(RayCastHit& hit, const Line& line, vector<shared_ptr<GameObject>>& vec, const vector<wstring>& excludeTags, const shared_ptr<GameObject>& excludeObject) {
		bool isHit = false;

		for (auto& obj : vec) {
			if (obj == excludeObject) {
				continue;
			}
			auto trans = obj->GetComponent<Transform>();
			Vec3 position = trans->GetPosition();
			Vec3 scale = trans->GetScale() / 2.0f;
			float distance = CalcDistancePointToLine(position, line);
			if (distance * distance > scale.lengthSqr()) {
				continue;
			}

			if (RayCast::HitTest(hit, line, obj, excludeTags)) {
				isHit = true;
			}
		}
		return isHit;
	}
	bool RayCast::HitTestMeshRayCast(const Line& line, RayCastHit& hit, const shared_ptr<GameObject>& object) {
		vector<Vec3> tempPositions;
		tempPositions.reserve(70000);
		auto smDraw = object->GetComponent<SmBaseDraw>(false);
		auto bcDraw = object->GetComponent<BcBaseDraw>(false);
		if (!smDraw && !bcDraw) return false;
		if (object->GetComponent<Collision>(false) == nullptr) return false;

		if (smDraw) {
			smDraw->GetStaticMeshWorldPositions(tempPositions);
		}
		else if (bcDraw) {
			bcDraw->GetStaticMeshWorldPositions(tempPositions);
		}
		vector<TRIANGLE> triangles;
		triangles.reserve(tempPositions.size() / 3);
		for (size_t i = 0, size = tempPositions.size(); i < size; i += 3) {
			TRIANGLE triangle;
			triangle.m_A = tempPositions[i];
			triangle.m_B = tempPositions[i + 1];
			triangle.m_C = tempPositions[i + 2];
			if (!triangle.IsValid()) {
				//三角形が無効なら次にうつる
				continue;
			}
			Vec3 center = (triangle.m_A + triangle.m_B + triangle.m_C) / 3.0f;

			float distance = CalcDistancePointToLine(center, line);
			float radius = max((center - triangle.m_A).lengthSqr(), max((center - triangle.m_B).lengthSqr(), (center - triangle.m_C).lengthSqr()));
			if (distance * distance > radius) {
				continue;
			}
			bsm::Vec3 hitPosition;
			float triangleIndex;
			if (HitTest::SEGMENT_TRIANGLE(line.m_Start, line.m_End, triangle, hitPosition, triangleIndex)) {
				auto Len = line.GetLength();
				Len *= triangleIndex;
				auto Nomal = line.GetDirection();
				Nomal.normalize();
				Nomal *= Len;
				hit.m_HitPosition = line.m_Start + Nomal;
				hit.m_Triangle = triangle;
				hit.m_TriangleIndex = i / 3;
				
				return true;
			}
		}
		//auto node = BVH::BuildBVH(triangles, 0, 8);

		return false;//BVH::HitRayCast(node, hit, line);
	}
	float RayCast::CalcDistancePointToLine(const Vec3& point, const Line& line) {
		Vec3 direction = line.GetDirection();
		Vec3 pointToStart = point - line.m_Start;
		float abLengthSquared = dot(direction, direction);

		if (abLengthSquared == 0.0) {
			return length(pointToStart);
		}

		float t = dot(pointToStart, direction) / abLengthSquared;
		t = static_cast<float>(max(0.0, min(1.0, t)));

		Vec3 closestPoint = { line.m_Start.x + t * direction.x, line.m_Start.y + t * direction.y,line.m_Start.z + t * direction.z };
		return length(point - closestPoint);
	}
	float RayCast::CalcDistancePoint(const Vec3& point, const Line& line) {
		Vec3 startToPoi = Vec3(point.x - line.m_Start.x, point.y - line.m_Start.y, point.z - line.m_Start.z);//始点
		Vec3 startToE = Vec3(line.m_End.x - line.m_Start.x, line.m_End.y - line.m_Start.y, line.m_End.z - line.m_Start.z);//終点
		Vec3 endToStr = Vec3(line.m_Start.x - line.m_End.x, line.m_Start.y - line.m_End.y, line.m_Start.z - line.m_End.z);
		Vec3 endToPoi = Vec3(point.x - line.m_End.x, point.y - line.m_End.y, point.z - line.m_End.z);
		if (startToPoi.dot(startToE) < 0.0) return startToPoi.length();
		if (endToPoi.dot(endToStr) < 0.0) return endToPoi.length();
		//return abs(startToE.x * startToPoi.y * startToPoi.z - startToE.y * startToPoi.x * startToPoi.z) / startToE.length();//外積
		//return abs(startToPoi.x * startToE.x + startToPoi.y *  startToE.y  + startToPoi.z * startToE.z);//内積
		Vec3 product;
		product.z = (startToE.x * startToPoi.y - startToE.y * startToPoi.x) / startToE.length();
		product.y = (startToE.z * startToPoi.x - startToE.x * startToPoi.z) / startToE.length();
		product.x = (startToE.y * startToPoi.z - startToE.z * startToPoi.y) / startToE.length();
		return abs(product.x + product.y + product.z);
	}

	void RayCast::InitRay(int size) {
		for (int i = 0; i < m_DebugRay.size(); i++) {
			if (i >= size) {
				m_DebugRay[i]->Destroy();
			}
			else {
				m_DebugRay[i]->SetLine(Line());
			}
		}
		if (m_DebugRay.size() >= size) {
			m_DebugRay.erase(m_DebugRay.begin() + size, m_DebugRay.end());
		}
	}
	void RayCast::DebugRay(const Line& line,Col4& color, shared_ptr<Stage>& stage) {
		for (auto& ray : m_DebugRay) {
			if (!ray->GetDrawActive()) {
				ray->SetLine(line);
				return;
			}
		}
		auto ray = stage->AddGameObject<LineCube>(0.02f, color);
		m_DebugRay.push_back(ray);
	}
	void RayCast::CreateRayCast(int size) {
		m_RayCasts.clear();
		m_RayCasts.reserve(size);
		for (int i = 0; i < size; ++i) {
			m_RayCasts.emplace_back(RayCast());
		}
	}



	bool BVH::HitRayCast(shared_ptr<BVHNode>& node, RayCastHit& hit, const Line& line) {
		if (!node->IsIntersect(line)) return false;

		if (node->IsLeaf()) {
			for (const auto& tri : node->triangles) {
				
				bsm::Vec3 hitPosition;
				float triangleIndex;
				if (HitTest::SEGMENT_TRIANGLE(line.m_Start, line.m_End, tri, hitPosition, triangleIndex)) {
					auto Len = line.GetLength();
					Len *= triangleIndex;
					auto Nomal = line.GetDirection();
					Nomal.normalize();
					Nomal *= Len;
					hit.m_HitPosition = line.m_Start + Nomal;
					hit.m_Triangle = tri;
					hit.m_TriangleIndex = triangleIndex;

					return true;
				}
			}
			return false;
		}

		return HitRayCast(node->leftNode, hit, line) || HitRayCast(node->rightNode, hit, line);
	}

	shared_ptr<BVHNode> BVH::BuildBVH(vector<TRIANGLE> triangle, int depth, int max) {
		RayCast::count++;
		shared_ptr<BVHNode> node = make_shared<BVHNode>(BVHNode());
		node->bounds = TriangleBounds(triangle);
		const int maxDepth = 20;
		if (triangle.size() <= max || depth >= maxDepth) {
			node->triangles = triangle;
			return node;
		}
		Vec3 length = node->bounds.m_Max - node->bounds.m_Min;
		int axis = (length.x > length.y && length.x > length.z) ? 0 : (length.y > length.z ? 1 : 2);

		sort(triangle.begin(), triangle.end(), [axis](const TRIANGLE& tri1, const TRIANGLE tri2) {
			float aCenter = (tri1.m_A[axis] + tri1.m_B[axis] + tri1.m_C[axis]) / 3.0f;
			float bCenter = (tri2.m_A[axis] + tri2.m_B[axis] + tri2.m_C[axis]) / 3.0f;

			return aCenter < bCenter;
			});
		/*float split = 0.0f;
		for (auto& tri : triangle) {
			split += (tri.m_A[axis] + tri.m_B[axis] + tri.m_C[axis]) / 3.0f;
		}

		split /= triangle.size();*/
		size_t mid = triangle.size() / 2;
		vector<TRIANGLE> left(triangle.begin(), triangle.begin() + mid);
		vector<TRIANGLE> right(triangle.begin() + mid, triangle.end());
		/*for (auto& tri : triangle) {
			float center = (tri.m_A[axis] + tri.m_B[axis] + tri.m_C[axis]) / 3.0f;
			if (split < center) {
				left.push_back(tri);
			}
			else {
				right.push_back(tri);
			}
		}*/
		int diff = left.size() - right.size();
		if (left.empty() || right.empty() || abs(diff) > triangle.size() * 0.75f) {
			node->triangles = triangle;
			return node;
		}
		node->leftNode = BuildBVH(left, depth + 1, max);
		node->rightNode = BuildBVH(right, depth + 1, max);

		return node;
	}

	AABB BVH::TriangleBounds(vector<TRIANGLE> triangle) {
		Vec3 minPos = Vec3(INT_MAX), maxPos = Vec3(-INT_MAX);
		for (auto& tri : triangle) {
			vector<Vec3> indeces = { tri.m_A,tri.m_B ,tri.m_C };
			for (int i = 0; i < 3; i++) {
				minPos.x = min(minPos.x, indeces[i].x);
				minPos.y = min(minPos.y, indeces[i].y);
				minPos.z = min(minPos.z, indeces[i].z);

				maxPos.x = max(maxPos.x, indeces[i].x);
				maxPos.y = max(maxPos.y, indeces[i].y);
				maxPos.z = max(maxPos.z, indeces[i].z);
			}
		}

		return AABB(minPos, maxPos);
	}
}
//end basecross
