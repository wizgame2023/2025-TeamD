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
	bool RayCast::HitTest(RayCastHit& hit, const Line& line, shared_ptr<GameObject>& object, const vector<wstring> excludeTags, const bool& isDebug) {
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
		/*auto draw = object->GetComponent<SmBaseDraw>(false);
		auto bcDraw = object->GetComponent<BcBaseDraw>(false);
		if (draw != nullptr) {

			isHit = draw->HitTestStaticMeshSegmentTriangles(line.m_Start, line.m_End, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
		}
		else if (bcDraw != nullptr) {
			isHit = bcDraw->HitTestStaticMeshSegmentTriangles(line.m_Start, line.m_End, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
		}*/

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
	//float RayCast::CalcDistancePointToLine(const Vec3& point, const Line& line) {
	//	Vec2 startToPoint = Vec2(point.x - line.m_Start.x, point.z - line.m_Start.z);
	//	Vec2 startToEnd = Vec2(line.m_End.x - line.m_Start.x, line.m_End.z - line.m_Start.z);
	//	Vec2 endToStart = Vec2(line.m_Start.x - line.m_End.x, line.m_Start.z - line.m_End.z);
	//	Vec2 endToPoint = Vec2(point.x - line.m_End.x, point.z - line.m_End.z);
	//	if (startToPoint.dot(startToEnd) < 0.0) return startToPoint.length();
	//	if (endToPoint.dot(endToStart) < 0.0) return endToPoint.length();
	//	return abs(startToEnd.x * startToPoint.y - startToEnd.y * startToPoint.x) / startToEnd.length();
	//}
	float RayCast::CalcDistancePoi(const Vec3& point, const Line& line) {
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
