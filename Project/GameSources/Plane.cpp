#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void Plane::OnCreate()
	{
		// 板ポリを形成するための頂点データ
		std::vector<VertexPositionTexture> vertices = {
			{Vec3(-0.5f, +0.5f, 0.0f), Vec2(0.0f, 0.0f)},
			{Vec3(+0.5f, +0.5f, 0.0f), Vec2(1.0f, 0.0f)},
			{Vec3(-0.5f, -0.5f, 0.0f), Vec2(0.0f, 1.0f)},
			{Vec3(+0.5f, -0.5f, 0.0f), Vec2(1.0f, 1.0f)}
		};

		// 頂点インデックス
		std::vector<uint16_t> indices = {
			0, 1, 2, 2, 1, 3
		};

		// 上記データを適用
		auto drawComp = AddComponent<PTStaticDraw>(); // PNTだとライティングがかかるので向かない
		drawComp->SetOriginalMeshUse(true);
		drawComp->CreateOriginalMesh(vertices, indices);
	}
}
