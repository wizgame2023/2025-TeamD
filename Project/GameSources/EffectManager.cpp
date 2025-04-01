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
	EfkEffect::EfkEffect(const  wstring& Key, const wstring& FileName, const Effekseer::EffectRef effect):
		m_Key(Key),
		m_FileName(FileName),
		m_Effect(effect)	{
	}
	EfkEffect::~EfkEffect() {
	}

	void EfkEffect::OnCreate() {
	}

	void EfkEffect::PlayEffect(const wstring& Key, const bsm::Vec3& Emitter, const float fremeconst)
	{
		int32_t Freme = fremeconst;
		shared_ptr<EfkEffect> effect = GetEffectResource(Key);
		m_handle = m_Manager->Play(effect->m_Effect, ::Effekseer::Vector3D(Emitter.x, Emitter.y, Emitter.z), Freme);
	}

	void EfkEffect::RegisterResource(const wstring& Key, const shared_ptr<EfkEffect>& ResObj)
	{
		try {
			if (Key == L"") {
				throw BaseException(
					L"",
					L"if(Key == L\"\")",
					L"Effect::RegisterResource()"
				);
			}
			if (!ResObj) {
				throw BaseException(
					L"",
					L"if(!pResObj)",
					L"Effect::RegisterResource()"
				);
			}

			map<wstring, shared_ptr<EfkEffect>>::iterator it;
			for (it = m_ResMap.begin(); it != m_ResMap.end(); it++) {
				if (it->second == ResObj)
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
				m_ResMap[Key] = ResObj;
			}
		}
		catch (...) {
			throw;
		}
	}


	shared_ptr<EfkEffect> EfkEffect::RegisterEffect(const wstring& Key, const wstring& EffectFileName, const wstring& EffectType)
	{
		
		auto ptrEffect = CreateEffectResource(EffectFileName, EffectType);
		RegisterResource(Key, ptrEffect);
		return ptrEffect;
	}

	shared_ptr<EfkEffect> EfkEffect::CreateEffectResource(const wstring& FileName, const wstring& TexType)
	{
		try {
			auto Effect = ::Effekseer::Effect::Create(m_manager, (const char16_t*)FileName.c_str());
			return ObjectFactory::Create<EfkEffect>(TexType, FileName, Effect);
		}
		catch (...) {
			throw;
		}
	}

	shared_ptr<EfkEffect> EfkEffect::GetEffectResource(const wstring& Key) const
	{
		if (Key == L"") {
			throw BaseException(
				L"",
				L"if(Key == L\"\")",
				L"App::GetResource()"
			);
		}
		map<wstring, shared_ptr<EfkEffect> >::const_iterator  it;
		it = m_ResMap.find(Key);
		if (it != m_ResMap.end()) {
			//指定の名前が見つかった
			shared_ptr<EfkEffect> pT = dynamic_pointer_cast<EfkEffect>(it->second);
			if (pT) {
				return pT;
			}
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

	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのインターフェイス
	//--------------------------------------------------------------------------------------
	EfkInterface::EfkInterface() :
		ObjectInterface(),
		m_Manager(nullptr),
		m_renderer(nullptr)
	{}
	EfkInterface::~EfkInterface() {
		// 先にエフェクト管理用インスタンスを破棄
		m_Manager.Reset();
		// 次に描画用インスタンスを破棄
		m_renderer.Reset();
	}

	void EfkInterface::OnCreate() {
		//デバイスの取得
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

	void  EfkInterface::OnUpdate() {
		// エフェクトの更新処理を行う
		m_Manager->Update();
	}

	void EfkInterface::OnDraw() {
		// エフェクトの描画開始処理を行う。
		m_renderer->BeginRendering();

		// エフェクトの描画を行う。
		m_Manager->Draw();

		// エフェクトの描画終了処理を行う。
		m_renderer->EndRendering();

	}

	void Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				dest.Values[i][j] = src(i, j);
			}
		}
	}

	void  EfkInterface::SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj) {
		Effekseer::Matrix44 v, p;
		Mat4x4ToMatrix44(view, v);
		Mat4x4ToMatrix44(proj, p);
		m_renderer->SetCameraMatrix(v);
		m_renderer->SetProjectionMatrix(p);
	}
}
//end basecross
