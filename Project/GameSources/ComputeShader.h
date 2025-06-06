/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

    struct CB {

    };
    DECLARE_DX11_CONSTANT_BUFFER(ConstantBufferCalcbody, CB)
    DECLARE_DX11_COMPUTE_SHADER(CSRayCast)

    template<typename T>
	class CsShader {
		ComPtr<ID3D11Buffer> m_Buffer;
		ComPtr<ID3D11Buffer> m_ResultBuffer;
		ComPtr<ID3D11UnorderedAccessView> m_AccessView;

	public:
		CsShader(){}
		~CsShader(){}

        void Init() {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();
            auto devContext = devResource->GetD3DDeviceContext();

            //エレメントバッファ
            D3D11_BUFFER_DESC bufferDesc = {};
            bufferDesc.ByteWidth = sizeof(T);
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.StructureByteStride = sizeof(T);
            ThrowIfFailed(
                pDx11Device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer),
                L"エレメントバッファ作成に失敗しました",
                L"pDx11Device->CreateBuffer()",
                L"CsShader::Init()"
            );
            //アクセスビュー
            D3D11_UNORDERED_ACCESS_VIEW_DESC accessBufferDesc = {};
            accessBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
            accessBufferDesc.Buffer.NumElements = 1;
            ThrowIfFailed(
                pDx11Device->CreateUnorderedAccessView(m_Buffer.Get(), &accessBufferDesc, &m_AccessView),
                L"アクセスビュー作成に失敗しました",
                L"pDx11Device->CreateUnorderedAccessView()",
                L"CsShader::Init()"
            );
            //リードバックバッファ
            D3D11_BUFFER_DESC resultBufferDesc = {};
            resultBufferDesc.ByteWidth = sizeof(T);
            resultBufferDesc.Usage = D3D11_USAGE_STAGING;
            resultBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            resultBufferDesc.StructureByteStride = sizeof(T);
            ThrowIfFailed(
                pDx11Device->CreateBuffer(&resultBufferDesc, nullptr, &m_ResultBuffer),
                L"リードバックバッファ作成に失敗しました",
                L"pDx11Device->CreateBuffer()",
                L"CsShader::Init()"
            );
        }
        T GetResult(T element) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();
            //コンスタントバッファ
            CB cb = {};
            
            //コンスタントバッファの設定
            ID3D11Buffer* constantBuffer = ConstantBufferCalcbody::GetPtr()->GetBuffer();
            devContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
            devContext->CSSetConstantBuffers(0, 1, &constantBuffer);
            //シェーダーに情報を送る
            devContext->UpdateSubresource(m_Buffer.Get(), 0, nullptr, &element, 0, 0);
            //コンピュータシェーダーの設定
            devContext->CSSetShader(CSRayCast::GetPtr()->GetShader(), nullptr, 0);
            //アクセスビューの設定
            devContext->CSSetUnorderedAccessViews(0, 1, m_AccessView.GetAddressOf(), nullptr);
            //実行
            devContext->Dispatch(1, 1, 1);

            //結果の取得
            D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
            devContext->CopyResource(m_ResultBuffer.Get(), m_Buffer.Get());
            if (SUCCEEDED(devContext->Map(m_ResultBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource))) {
                T result;
                memcpy(&result, mappedResource.pData, sizeof(T));
                devContext->Unmap(m_ResultBuffer.Get(), 0);

                return result;
            }

            return {};
        }
	};
}

//end basecross
