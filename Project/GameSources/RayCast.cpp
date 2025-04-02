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
	/// <param name="startPosition">発射位置</param>
	/// <param name="direction">発射方向</param>
	/// <param name="length">長さ</param>
	/// <param name="object">調べるオブジェクト</param>
	/// <param name="excludeTags">除外するタグ</param>
	/// <returns>当たったか</returns>
	bool RayCast::HitTest(RayCastHit& hit, const Vec3& startPosition, const Vec3& direction, float length, shared_ptr<GameObject>& object, const vector<wstring> excludeTags, const bool& isDebug) {
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
		Vec3 endPosition = startPosition + direction * length;
		auto draw = object->GetComponent<SmBaseDraw>(false);
		auto bcDraw = object->GetComponent<BcBaseDraw>(false);
		if (draw != nullptr) {
			isHit = draw->HitTestStaticMeshSegmentTriangles(startPosition, endPosition, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
		}
		else if (bcDraw != nullptr) {
			isHit = bcDraw->HitTestStaticMeshSegmentTriangles(startPosition, endPosition, newResult.m_HitPosition, newResult.m_Triangle, newResult.m_TriangleIndex);
		}

		if (isHit) {
			if (hit.m_Object == nullptr) {
				hit.m_Object = object;
				hit = newResult;
			}
			else if ((hit.m_HitPosition - startPosition).length() > (newResult.m_HitPosition - startPosition).length()) {

				hit.m_Object = object;
				hit = newResult;
			}
			return true;
		}
		return false;
	}
	/// <summary>
	/// 線分から点までの最短距離を求める
	/// </summary>
	/// <param name="point">点の位置</param>
	/// <param name="start">線分の始点</param>
	/// <param name="end">線分の終点</param>
	/// <returns>最短距離</returns>
	float RayCast::CalcDistancePointToLine(const Vec3& point, const Vec3& start, const Vec3& end) {
		Vec2 startToPoint = Vec2(point.x - start.x, point.z - start.z);
		Vec2 startToEnd = Vec2(end.x - start.x, end.z - start.z);
		Vec2 endToStart = Vec2(start.x - end.x, start.z - end.z);
		Vec2 endToPoint = Vec2(point.x - end.x, point.z - end.z);
		if (startToPoint.dot(startToEnd) < 0.0) return startToPoint.length();
		if (endToPoint.dot(endToStart) < 0.0) return endToPoint.length();
		return abs(startToEnd.x * startToPoint.y - startToEnd.y * startToPoint.x) / startToEnd.length();
	}
}
//end basecross
