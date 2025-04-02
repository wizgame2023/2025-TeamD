/*!
@file EffectManager.cpp
@brief エフェクトなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EffectManager.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのエフェクト
	//--------------------------------------------------------------------------------------
	EfkEffect::EfkEffect() :
		ObjectInterface(),
		m_handle(-1),
		m_renderer(nullptr),
		m_Manager(nullptr)
	{
	}
	EfkEffect::~EfkEffect() {
		// 先にエフェクト管理用インスタンスを破棄
		m_Manager.Reset();
		// 次に描画用インスタンスを破棄
		m_renderer.Reset();
	}

	void EfkEffect::OnCreate() {
		CreateEffectInterface();
	}

	void EfkEffect::OnUpdate()
	{
		// エフェクトの更新処理を行う
		m_Manager->Update();

	}

	void EfkEffect::OnDraw()
	{
		// エフェクトの描画開始処理を行う。
		m_renderer->BeginRendering();

		// エフェクトの描画を行う。
		m_Manager->Draw();

		// エフェクトの描画終了処理を行う。
		m_renderer->EndRendering();
	}

	void EfkEffect::Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest)
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				dest.Values[i][j] = src(i, j);
			}
		}
	}

	void EfkEffect::SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj)
	{
		Effekseer::Matrix44 v, p;
		Mat4x4ToMatrix44(view, v);
		Mat4x4ToMatrix44(proj, p);
		m_renderer->SetCameraMatrix(v);
		m_renderer->SetProjectionMatrix(p);
	}

	void EfkEffect::PlayEffect(const wstring& Key, const bsm::Vec3& Emitter, const float freme)
	{
		int32_t Freme = freme;
		m_Effect = GetEffectResource(Key);
		m_handle = m_Manager->Play(m_Effect, ::Effekseer::Vector3D(Emitter.x, Emitter.y, Emitter.z), Freme);
	}

	void EfkEffect::CreateEffectInterface()
	{
		
		auto Dev = App::GetApp()->GetDeviceResources();
		auto pDx11Device = Dev->GetD3DDevice();
		auto pID3D11DeviceContext = Dev->GetD3DDeviceContext();
		// 描画用インスタンスの生成
		m_renderer = EffekseerRendererDX11::Renderer::Create(pDx11Device, pID3D11DeviceContext, 2000);
		// エフェクト管理用インスタンスの生成
		m_Manager = Effekseer::Manager::Create(2000);

		// 描画用インスタンスから描画機能を設定
		m_Manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
		m_Manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
		m_Manager->SetRingRenderer(m_renderer->CreateRingRenderer());
		m_Manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
		m_Manager->SetModelRenderer(m_renderer->CreateModelRenderer());

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		m_Manager->SetTextureLoader(m_renderer->CreateTextureLoader());
		m_Manager->SetModelLoader(m_renderer->CreateModelLoader());
	}

	void EfkEffect::RegisterResource(const wstring& Key, const  wstring& FileName)
	{
		try {
			if (Key == L"") {
				throw BaseException(
					L"",
					L"if(Key == L\"\")",
					L"Effect::RegisterResource()"
				);
			}
			m_Effect = Effekseer::Effect::Create(m_Manager, (const char16_t*)FileName.c_str());
			map<wstring, Effekseer::EffectRef>::iterator it;
			for (it = m_ResMap.begin(); it != m_ResMap.end(); it++) {
				if (it->second == m_Effect)
				{
					if (it->first == Key)
					{
						return;
					}
					wstring keyerr = Key;
					throw BaseException(
						L"",
						keyerr,
						L"Effect::RegisterResource()"
					);

				}
			}
			it = m_ResMap.find(Key);
			if (it != m_ResMap.end())
			{
				//指定の名前が見つかった
				//例外発生
				wstring keyerr = Key;
				throw BaseException(
					L"",
					keyerr,
					L"Effect::RegisterResource()"
				);
			}
			else {
				m_ResMap[Key] = m_Effect;

			}
		}
		catch (...) {
			throw;
		}
	}

	Effekseer::EffectRef EfkEffect::GetEffectResource(const wstring& Key) const
	{
		if (Key == L"") {
			throw BaseException(
				L"",
				L"if(Key == L\"\")",
				L"App::GetResource()"
			);
		}
		map<wstring, Effekseer::EffectRef >::const_iterator  it;
		it = m_ResMap.find(Key);
		if (it != m_ResMap.end()) {
			//指定の名前が見つかった
			return  it->second;
		}
		else {
			//見つからない
			wstring keyerr = Key;
			throw BaseException(
				L"",
				keyerr,
				L"App::GetResource()"
			);
		}

	}

	void EfkEffect::AddLocation(const bsm::Vec3& Location) {
		if (m_handle != -1) {
			m_Manager->AddLocation(m_handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z));
		}
	}


	void EfkEffect::SetRotation(const bsm::Vec3& Location, const float angle)
	{
		m_Manager->SetRotation(m_handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z), angle);
	}

	void EfkEffect::SetLocation(const bsm::Vec3& Location) {
		m_Manager->SetLocation(m_handle, Location.x, Location.y, Location.z);
	}

	void EfkEffect::SetScale(const bsm::Vec3& Scale)
	{
		m_Manager->SetScale(m_handle, Scale.x, Scale.y, Scale.z);
	}

	void EfkEffect::SetAllColor(const bsm::Col4 Color)
	{
		auto color = Col4(Color) * 255;
		m_Manager->SetAllColor(m_handle, ::Effekseer::Color(color.x, color.y, color.z, color.w));
	}

	void EfkEffect::StopEffect() {
		if (m_handle != -1) {
			m_Manager->StopEffect(m_handle);
		}
	}

}
//end basecross
