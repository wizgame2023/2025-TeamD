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
	EffectManeger::EffectManeger(const shared_ptr<Stage>& stage) :
		MultiParticle(stage),
		m_renderer(nullptr),
		m_Manager(nullptr)
	{
	}
	EffectManeger::~EffectManeger() {
		// 先にエフェクト管理用インスタンスを破棄
		m_Manager.Reset();
		// 次に描画用インスタンスを破棄
		m_renderer.Reset();
		
	}

	void EffectManeger::OnCreate() {
		CreateEffectInterface();
		SetDrawLayer(0);
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();
		wstring effectPath = mediaPath + L"Effekt/";
		RegisterResource(L"panchi", effectPath + L"panchi.efk");
	}

	void EffectManeger::OnUpdate()
	{
		auto elps = App::GetApp()->GetElapsedTime();
		m_TotalTime += elps;
		// エフェクトの更新処理を行う
		m_Manager->Update();
		m_renderer->SetTime(elps);

	}

	void EffectManeger::OnDraw()
	{
		// エフェクトの描画開始処理を行う。
		m_renderer->BeginRendering();
		// エフェクトの描画を行う。
		m_Manager->Draw();
		
		// エフェクトの描画終了処理を行う。
		m_renderer->EndRendering();
	}

	void EffectManeger::OnDestroy()
	{
		m_Manager->Release();
	}

	void EffectManeger::SetEffectSpeed(Effekseer::Handle& handle, const float& speed)
	{
		m_Manager->SetSpeed(handle, speed);
	}

	void EffectManeger::SetEffectPause( const bool& pause)
	{
		m_Manager->SetPausedToAllEffects(pause);
	}

	void EffectManeger::Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest)
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				dest.Values[i][j] = src(i, j);
			}
		}
	}

	void EffectManeger::SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj)
	{
		Effekseer::Matrix44 v, p;
		Mat4x4ToMatrix44(view, v);
		Mat4x4ToMatrix44(proj, p);
		m_renderer->SetCameraMatrix(v);
		m_renderer->SetProjectionMatrix(p);
	}

	void EffectManeger::PlayEffect(Effekseer::Handle& handle, const wstring& Key, const bsm::Vec3& Emitter, const float freme)
	{
		int32_t Freme = freme;
		m_Effect = GetEffectResource(Key);
		handle = m_Manager->Play(m_Effect, ::Effekseer::Vector3D(Emitter.x, Emitter.y, Emitter.z), Freme);
	}

	void EffectManeger::CreateEffectInterface()
	{
		auto Dev = App::GetApp()->GetDeviceResources();
		auto pDx11Device = Dev->GetD3DDevice();
		auto pID3D11DeviceContext = Dev->GetD3DDeviceContext();
		// 描画用インスタンスの生成
		m_renderer = EffekseerRendererDX11::Renderer::Create(pDx11Device, pID3D11DeviceContext, 8000);
		// エフェクト管理用インスタンスの生成
		m_Manager = Effekseer::Manager::Create(8000);

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
		m_Manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
		m_Manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	}

	void EffectManeger::RegisterResource(const wstring& Key, const  wstring& FileName)
	{
		try {
			if (Key == L"") {
				throw BaseException(
					L"",
					L"if(Key == L\"\")",
					L"Effect::RegisterResource()"
				);
			}
			auto Effect = Effekseer::Effect::Create(m_Manager, (const char16_t*)FileName.c_str());
			map<wstring, Effekseer::EffectRef>::iterator it;
			for (it = m_ResMap.begin(); it != m_ResMap.end(); it++) {
				if (it->second == Effect)
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
				m_ResMap[Key] = Effect;

			}
		}
		catch (...) {
			throw;
		}
	}

	Effekseer::EffectRef EffectManeger::GetEffectResource(const wstring& Key) const
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

	void EffectManeger::AddLocation(Effekseer::Handle& handle, const bsm::Vec3& Location) {
		if (handle != -1) {
			m_Manager->AddLocation(handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z));
		}
	}

	void EffectManeger::SetRotation(Effekseer::Handle& handle, const bsm::Vec3& Location, const float angle)
	{
		m_Manager->SetRotation(handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z), angle);
	}

	void EffectManeger::SetLocation(Effekseer::Handle& handle, const bsm::Vec3& Location) {
		m_Manager->SetLocation(handle, Location.x, Location.y, Location.z);
	}
	void EffectManeger::SetScale(Effekseer::Handle& handle, const bsm::Vec3& Scale)
	{
		m_Manager->SetScale(handle, Scale.x, Scale.y, Scale.z);
	}

	void EffectManeger::SetAllColor(Effekseer::Handle& handle, const bsm::Col4 Color)
	{
		auto color = Col4(Color) * 255;
		m_Manager->SetAllColor(handle, ::Effekseer::Color(color.x, color.y, color.z, color.w));
	}

	void EffectManeger::StopEffect(Effekseer::Handle& handle) {
		if (handle != -1) {
			m_Manager->StopEffect(handle);
		}
	}

	void EffectManeger::SetLayer(Effekseer::Handle& handle, int32_t layer)
	{
		m_Manager->SetLayer(handle, layer);
	}

}
//end basecross
