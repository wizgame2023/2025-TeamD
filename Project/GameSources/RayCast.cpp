/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	/// <summary>
	/// レイキャスト処理
	/// </summary>
	/// <param name="hit">結果</param>
	/// <param name="line">線分</param>
	/// <param name="object">調べるオブジェクト</param>
	/// <param name="excludeTags">除外するタグ</param>
	/// <returns>当たったか</returns>
	bool RayCast::HitTest(RayCastHit& hit, const Line& line, shared_ptr<GameObject>& object, const vector<wstring> excludeTags) {
		RayCastHit newResult = RayCastHit();
		if (object == nullptr) return false;
		bool isExclude = false;
		for (auto& tag : excludeTags) {
			if (object->FindTag(tag)) {
				isExclude = true;
				break;
			}
		}
		if (isExclude) return false;
		bool isHit = false;
		isHit = HitTestMeshRayCast(line, newResult, object);
		if (isHit) {
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
	bool RayCast::HitTestVec(RayCastHit& hit, const Line& line, vector<shared_ptr<GameObject>>& vec, const vector<wstring>& excludeTags) {
		bool isHit = false;
		for (auto& obj : vec) {
			/*auto smDraw = obj->GetComponent<SmBaseDraw>(false);
			auto bcDraw = obj->GetComponent<BcBaseDraw>(false);
			if (smDraw) {
				smDraw->SetDiffuse(Col4(1, 1, 1, 1));
			}
			else if (bcDraw) {
				bcDraw->SetDiffuse(Col4(1, 1, 1, 1));
			}*/

			bool isExclude = false;
			for (auto& tag : excludeTags) {
				if (obj->FindTag(tag)) {
					isExclude = true;
					break;
				}
			}
			if (isExclude) continue;
			auto transform = obj->GetComponent<Transform>();
			Vec3 position = transform->GetPosition();
			Vec3 scale = transform->GetScale();
			float distance = RayCast::CalcDistancePointToLine(position,line);
			Vec3 halfScale = Vec3(scale.x, scale.y, scale.z) / 2.0f;
			if (distance < halfScale.length()) {
				/*if (smDraw) {
					smDraw->SetDiffuse(Col4(1, 0, 0, 1));
				}
				else if (bcDraw) {
					bcDraw->SetDiffuse(Col4(1, 0, 0, 1));
				}*/
				if (RayCast::HitTest(hit, line, obj)) {
					isHit = true;
				}
			}
		}
		return isHit;
	}
	bool RayCast::HitTestMeshRayCast(const Line& line, RayCastHit& hit, const shared_ptr<GameObject>& object) {
		vector<Vec3> tempPositions;
		auto smDraw = object->GetComponent<SmBaseDraw>(false);
		auto bcDraw = object->GetComponent<BcBaseDraw>(false);
		if (smDraw) {
			smDraw->GetStaticMeshWorldPositions(tempPositions);
		}
		else if (bcDraw) {
			bcDraw->GetStaticMeshWorldPositions(tempPositions);
		}
		if (object->FindTag(L"Player")) {
			int a = 10;
		}
		for (size_t i = 0, size = tempPositions.size(); i < size; i += 3) {
			TRIANGLE triangle;
			triangle.m_A = tempPositions[i];
			triangle.m_B = tempPositions[i + 1];
			triangle.m_C = tempPositions[i + 2];
			if (!triangle.IsValid()) {
				//三角形が無効なら次にうつる
				continue;
			}
			/*Vec3 center = triangle.m_A + triangle.m_B + triangle.m_C;
			center /= 3.0f;

			float length = (center - triangle.m_A).length();

			float distance = RayCast::CalcDistance3DPointToLine(center, line);
			if (distance > length) {
				continue;
			}*/

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
		return false;
	}
	float RayCast::CalcDistancePointToLine(const Vec3& point, const Line& line) {
		Vec3 ab = line.GetDirection();
		Vec3 ap = point - line.m_Start;
		float abLengthSquared = dot(ab, ab);

		if (abLengthSquared == 0.0) {
			// a と b が同じ点の場合
			return length(ap);
		}

		float t = dot(ap, ab) / abLengthSquared;
		t = max(0.0, min(1.0, t));

		Vec3 closestPoint = { line.m_Start.x + t * ab.x, line.m_Start.y + t * ab.y,line.m_Start.z + t * ab.z };
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
}
//end basecross
