/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	struct DissolvePsCbData {
		float dissolveAmount;
		float pad[3];
		Col4 dissolveColor;
	};
	struct DissolveGsCbData {

	};

	DECLARE_DX11_CONSTANT_BUFFER(DissolvePsCB, DissolvePsCbData)
	DECLARE_DX11_CONSTANT_BUFFER(DissolveGsCB, DissolveGsCbData)

	DECLARE_DX11_PIXEL_SHADER(PSDissolve)
	DECLARE_DX11_GEOMETRY_SHADER(GSDissolve)

	enum class BoneState {
		Static,
		Bone
	};

	class DissolveDraw : public SmBaseDraw
	{
		DissolvePsCbData	m_PsCBData;		//ピクセルシェーダーコンスタントバッファ用
		DissolveGsCbData	m_GsCBData;		//ジオメトリシェーダーコンスタントバッファ用
		float				m_DissolveSpeed;//溶解進行速度
		bool				m_IsDissolve;	//溶解するか
		BoneState			m_BoneState;	//ボーン状態(static・bone)

		shared_ptr<TextureResource> m_NoiseTexture;	//ノイズテクスチャ
	public:
		DissolveDraw(const shared_ptr<GameObject>& ptr) : SmBaseDraw(ptr),m_DissolveSpeed(0.0f),m_IsDissolve(false),m_BoneState(BoneState::Static){}

		virtual ‾DissolveDraw() {}
		
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;

		void SetMeshResource(const shared_ptr<MeshResource>& MeshRes) {
			SmBaseDraw::SetMeshResource(MeshRes);
			if (m_BoneState == BoneState::Bone) {
				BoneInit();
			}
		}

		void SetMeshResource(const wstring& MeshKey) {
			DissolveDraw::SetMeshResource(App::GetApp()->GetResource<MeshResource>(MeshKey));
		}


		void SetNoiseTextureResource(const wstring& key) {
			SetNoiseTextureResource(App::GetApp()->GetResource<TextureResource>(key));
		}
		void SetNoiseTextureResource(const shared_ptr<TextureResource>& textureResource) {
			m_NoiseTexture = textureResource;
		}
		
		void SetDissolveSpeed(float speed) {
			m_DissolveSpeed = speed;
		}
		void StopDissolve() {
			SetDissolveSpeed(0.0f);
			SetDissolveActive(false);
		}
		void ResetDissolve() {
			m_PsCBData.dissolveAmount = 0.0f;
		}
		float GetDissolveAmount() {
			return m_PsCBData.dissolveAmount;
		}

		void SetDissolveColor(const Col4& color) {
			m_PsCBData.dissolveColor = color;
		}
		void SetDissolve(float amount) {
			m_PsCBData.dissolveAmount = amount;
		}
		void SetBoneState(BoneState state) {
			m_BoneState = state;
			if (m_BoneState == BoneState::Bone) {
				BoneInit();
			}
		}
		void SetDissolveActive(bool active) {
			m_IsDissolve = active;
		}
		bool GetDissolveActive() {
			return m_IsDissolve;
		}

		/// <summary>
		/// 頂点シェーダーの初期化
		/// </summary>
		/// <typeparam name="VS">頂点シェーダー</typeparam>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		template<typename VS>
		void InitVertexShader(ID3D11DeviceContext2* devContext) {
			//頂点シェーダ
			devContext->VSSetShader(VS::GetPtr()->GetShader(), nullptr, 0);
			//インプットレイアウトの設定
			devContext->IASetInputLayout(VS::GetPtr()->GetInputLayout());
		}

		/// <summary>
		/// バッファなどの初期設定
		/// </summary>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		/// <param name="renderState">レンダーステート</param>
		/// <param name="data">メッシュデータ</param>
		void InitData(ID3D11DeviceContext2* devContext,shared_ptr<RenderState>& renderState,const MeshPrimData& data) {
			//ストライドとオフセット
			UINT stride = data.m_NumStride;
			UINT offset = 0;
			//描画方法のセット
			devContext->IASetPrimitiveTopology(data.m_PrimitiveTopology);
			//頂点バッファのセット
			devContext->IASetVertexBuffers(0, 1, data.m_VertexBuffer.GetAddressOf(), &stride, &offset);
			//インデックスバッファのセット
			devContext->IASetIndexBuffer(data.m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			//各レンダリングステートの設定
			//ブレンドステート
			renderState->SetBlendState(devContext, GetBlendState());
			//デプスステンシルステート
			renderState->SetDepthStencilState(devContext, GetDepthStencilState());
		}

		/// <summary>
		/// ラスタライザーの初期化
		/// </summary>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		/// <param name="renderState">レンダーステート</param>
		/// <param name="material">マテリアル</param>
		void InitRasterizerState(ID3D11DeviceContext2* devContext, shared_ptr<RenderState>& renderState,const MaterialEx& material) {
			const auto& rasterizerState = GetRasterizerState();
			//ラスタライザステートと描画
			if (rasterizerState == RasterizerState::DoubleDraw) {
				//透明処理用
				//ラスタライザステート(裏描画)
				devContext->RSSetState(renderState->GetCullFront());
				//描画
				devContext->DrawIndexed(material.m_IndexCount, material.m_StartIndex, 0);
				//ラスタライザステート（表描画）
				devContext->RSSetState(renderState->GetCullBack());
				//描画
				devContext->DrawIndexed(material.m_IndexCount, material.m_StartIndex, 0);
			}
			else {
				renderState->SetRasterizerState(devContext, rasterizerState);
				//描画
				devContext->DrawIndexed(material.m_IndexCount, material.m_StartIndex, 0);
			}
		}

		/// <summary>
		/// シャドウマップの初期化
		/// </summary>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		/// <param name="mapPtr">シャドウマップターゲット</param>
		/// <param name="renderState">レンダーステート</param>
		void InitShadowMap(ID3D11DeviceContext2* devContext,shared_ptr<ShadowMapRenderTarget>& mapPtr,shared_ptr<RenderState>& renderState) {
			//シャドウマップのレンダラーターゲット
			ID3D11ShaderResourceView* pShadowSRV = mapPtr->GetShaderResourceView();
			devContext->PSSetShaderResources(1, 1, &pShadowSRV);
			//シャドウマップサンプラー
			ID3D11SamplerState* pShadowSampler = renderState->GetComparisonLinear();
			devContext->PSSetSamplers(1, 1, &pShadowSampler);
		}

		/// <summary>
		/// コンスタントバッファーの初期化
		/// </summary>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		/// <param name="cb">コンスタントバッファ</param>
		void InitConstantBaffur(ID3D11DeviceContext2* devContext,const SimpleConstants& cb) {
			//コンスタントバッファの更新
			devContext->UpdateSubresource(CBSimple::GetPtr()->GetBuffer(), 0, nullptr, &cb, 0, 0);
			devContext->UpdateSubresource(DissolvePsCB::GetPtr()->GetBuffer(), 0, nullptr, &m_PsCBData, 0, 0);
			//コンスタントバッファの設定
			ID3D11Buffer* pVsConstantBuffer = CBSimple::GetPtr()->GetBuffer();
			ID3D11Buffer* pPsConstantBuffer[] = { CBSimple::GetPtr()->GetBuffer() ,DissolvePsCB::GetPtr()->GetBuffer() };
			//ID3D11Buffer* pGsConstantBuffer[] = { CBSimple::GetPtr()->GetBuffer() ,DissolveGsCB::GetPtr()->GetBuffer() };
			ID3D11Buffer* pNullConstantBuffer = nullptr;
			//頂点シェーダに渡す
			devContext->VSSetConstantBuffers(0, 1, &pVsConstantBuffer);
			//ピクセルシェーダに渡す
			devContext->PSSetConstantBuffers(0, 2, pPsConstantBuffer);
			//ジオメトリシェーダーに渡す
			//devContext->GSSetConstantBuffers(0, 2, pGsConstantBuffer);
		}


		/// <summary>
		/// テクスチャをシェーダーに登録
		/// </summary>
		/// <param name="devContext">デバイスコンテキストのポインター</param>
		/// <param name="cb">コンスタントバッファ</param>
		/// <param name="material">マテリアル</param>
		void SetShaderTexture(ID3D11DeviceContext2* devContext,SimpleConstants& cb, const MaterialEx& material) {
			//テクスチャの選択
			if (IsModelTextureEnabled()) {
				ID3D11ShaderResourceView* pTextureSRV[] = { material.m_TextureResource->GetShaderResourceView().Get(),m_NoiseTexture->GetShaderResourceView().Get() };
				//モデルのテクスチャが有効
				devContext->PSSetShaderResources(0, 2, pTextureSRV);
			}
			else {
				//モデルのテクスチャが無効
				auto shTex = GetTextureResource();
				if (shTex) {
					//コンポーネントにテクスチャがある
					devContext->PSSetShaderResources(0, 1, shTex->GetShaderResourceView().GetAddressOf());
				}
				else {
					//コンポーネントにテクスチャがない
					cb.ActiveFlg.x = 0;
				}
			}
		}

		/// <summary>
		/// ディゾルブありの描画
		/// </summary>
		/// <typeparam name="VS">頂点シェーダー</typeparam>
		/// <typeparam name="PS">ピクセルシェーダー</typeparam>
		/// <param name="data">メッシュデータ</param>
		template<typename VS, typename PS>
		void DrawDissolve(const MeshPrimData& data) {
			auto Dev = App::GetApp()->GetDeviceResources();
			auto pD3D11DeviceContext = Dev->GetD3DDeviceContext();
			auto RenderState = Dev->GetRenderState();
			//NULLのシェーダリソースの準備
			ID3D11ShaderResourceView* pNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
			//サンプラーの準備
			ID3D11SamplerState* pSamplerClamp = RenderState->GetLinearClamp();
			ID3D11SamplerState* pSamplerWrap = RenderState->GetLinearWrap();
			ID3D11SamplerState* pNullSR[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
			//各オブジェクト共通処理
			//シェーダの設定
			InitVertexShader<VS>(pD3D11DeviceContext);
			//ピクセルシェーダ
			pD3D11DeviceContext->PSSetShader(PS::GetPtr()->GetShader(), nullptr, 0);
			//ジオメトリシェーダー
			//pD3D11DeviceContext->GSSetShader(GSDissolve::GetPtr()->GetShader(), nullptr, 0);
			
			InitData(pD3D11DeviceContext, RenderState, data);
			

			//影とサンプラー
			if (GetOwnShadowActive()) {
				//シャドウマップのレンダラーターゲット
				InitShadowMap(pD3D11DeviceContext, Dev->GetShadowMapRenderTarget(), RenderState);
			}
			//個別処理
			SimpleConstants SmCb;
			//コンスタントバッファの作成
			SetConstants(SmCb, data);
			//メッシュ内のマテリアルの配列
			auto& MatVec = data.m_MaterialExVec;
			size_t MatIndex = 0;
			for (auto& m : MatVec) {
				//テクスチャの有無
				SmCb.ActiveFlg.x = m.m_TextureResource ? 1 : 0;
				
				if (IsModelDiffusePriority()) {
					SmCb.Diffuse = m.m_Diffuse;
				}
				if (IsModelEmissivePriority()) {
					bsm::Col4 Em4 = m.m_Emissive;
					Em4.w = 0.0f;
					SmCb.Emissive = Em4;
				}
				InitConstantBaffur(pD3D11DeviceContext, SmCb);
				if (SmCb.ActiveFlg.x) {
					//シェーダにテクスチャの設定がされている
					//サンプラーの設定
					RenderState->SetSamplerState(pD3D11DeviceContext, GetSamplerState(), 0);
					SetShaderTexture(pD3D11DeviceContext, SmCb, m);
				}
				else {
					//シェーダーリソースもクリア
					pD3D11DeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pNull);
					//サンプラーもクリア
					RenderState->SetSamplerAllClear(pD3D11DeviceContext);
				}
				InitRasterizerState(pD3D11DeviceContext, RenderState, m);
				MatIndex++;
			}
		}
	};
}
//end basecross
