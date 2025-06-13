/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class SkyBackGround : public GameObject
	{
	public:
		static const std::map<std::wstring, Vec3> pairs; // テクスチャキー(アセット名)と配置座標を関連図ける連想配列

	private:
		std::vector<std::shared_ptr<GameObject>> m_planes; // スカイキューブの６面を管理する

	public:
		SkyBackGround(const shared_ptr<Stage>& stage)
			: GameObject(stage)
		{
		}

		void OnCreate() override;
		void OnDraw() override; // 自分で描画を行うためオーバーライドする
	};

}
//end basecross
