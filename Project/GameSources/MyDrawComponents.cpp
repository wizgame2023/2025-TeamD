/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	IMPLEMENT_DX11_CONSTANT_BUFFER(DissolvePsCB)
	IMPLEMENT_DX11_CONSTANT_BUFFER(DissolveGsCB)

	IMPLEMENT_DX11_PIXEL_SHADER(PSDissolve, App::GetApp()->GetShadersPath() + L"PSDissolve.cso")
	IMPLEMENT_DX11_GEOMETRY_SHADER(GSDissolve, App::GetApp()->GetShadersPath() + L"GSDissolve.cso")
}
namespace basecross {

	//初期化
	void DissolveDraw::OnCreate()
	{
		m_PsCBData.dissolveAmount = 0.0f; // デフォルトの溶解量
		m_PsCBData.dissolveColor = Col4(1.0f, 1.0f, 1.0f, 1.0f); // デフォルトの溶解色
	}
	void DissolveDraw::OnUpdate()
	{
		if(!m_IsDissolve) {
			return;
		}
		auto& app = App::GetApp();
		float elpased = app->GetElapsedTime();

		m_PsCBData.dissolveAmount += m_DissolveSpeed * elpased; // 溶解量の更新
		m_PsCBData.dissolveAmount = min(1.0f, max(0.0f, m_PsCBData.dissolveAmount));
	}
	void DissolveDraw::OnDraw()
	{
		auto& object = GetGameObject();
		if (object->GetAlphaActive()) {
			if (!(GetBlendState() == BlendState::AlphaBlend || GetBlendState() == BlendState::Additive)) {
				SetBlendState(BlendState::AlphaBlend);
			}
			SetRasterizerState(RasterizerState::DoubleDraw);
		}
		//メッシュリソースの取得
		auto PtrMeshResource = GetMeshResource();
		if (PtrMeshResource) {
			bool isShadowActive = GetOwnShadowActive();
			auto shadowMap = object->GetComponent<Shadowmap>(false);
			auto& meshData = PtrMeshResource->GetMashData();
			switch (m_BoneState) {
			case BoneState::Bone:
				if (m_IsDissolve) {
					DrawDissolve<VSPNTBone, PSDissolve>(meshData);
					break;
				}
				if (isShadowActive) {
					if (shadowMap) {
						DrawModel<VSPNTBoneShadow, PSPNTStaticShadow2>(meshData);
					}
					else {
						DrawModel<VSPNTBoneShadow, PSPNTStaticShadow>(meshData);
					}
				}
				else {
					DrawModel<VSPNTBone, PSPNTStatic>(meshData);
				}
				break;
			case BoneState::Static:
				if (m_IsDissolve) {
					DrawDissolve<VSPNTStatic, PSDissolve>(meshData);
					break;
				}
				if (isShadowActive) {
					if (shadowMap) {
						DrawStatic<VSPNTStaticShadow, PSPNTStaticShadow2>(meshData);
					}
					else {
						DrawStatic<VSPNTStaticShadow, PSPNTStaticShadow>(meshData);
					}
				}
				else {
					DrawStatic<VSPNTStatic, PSPNTStatic>(meshData);
				}
				break;
			}
		}
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->InitializeStates();
	}
}
//end basecross

