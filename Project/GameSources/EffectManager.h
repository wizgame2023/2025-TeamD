/*!
@file EffectManager.h
@brief Effectなど
*/

#pragma once
#include "stdafx.h"

#pragma once

#include "stdafx.h"

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>


#pragma comment(lib, "Effekseer.lib" )
#pragma comment(lib, "EffekseerRendererDX11.lib" )


namespace basecross {

	class EfkInterface;

	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのエフェクト
	//--------------------------------------------------------------------------------------
	class EfkEffect : public ObjectInterface {
		wstring m_FileName;
		wstring m_Key;
		Effekseer::ManagerRef m_manager;
		weak_ptr<EfkInterface> m_EfkInterface;
		map<wstring, shared_ptr<EfkEffect> > m_ResMap;		///< キーとリソースを結び付けるマップ
		Effekseer::Handle m_handle;
		Effekseer::ManagerRef m_Manager;

		friend class EfkPlay;
	public:
		Effekseer::EffectRef m_Effect;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスラトクタ
		@param[in]	iface	インターフェイス
		@param[in]	filename	エフェクトファイル名
		*/
		//--------------------------------------------------------------------------------------
		EfkEffect(const  wstring& Key, const wstring& FileName, const Effekseer::EffectRef effect);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EfkEffect();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() override;

		void PlayEffect(const wstring& Key, const bsm::Vec3& Emitter, const float freme);

		void RegisterResource(const wstring& Key, const shared_ptr<EfkEffect>& ResObj);
		shared_ptr<EfkEffect> RegisterEffect(const wstring& Key, const wstring& EffectFileName, const wstring& EffectType);
		shared_ptr<EfkEffect> CreateEffectResource(const wstring& FileName, const wstring& TexType);

		shared_ptr<EfkEffect> GetEffectResource(const wstring& Key) const;

		void AddLocation(const bsm::Vec3& Location);
		void SetLocation(const bsm::Vec3& Location);
		void SetRotation(const bsm::Vec3& Rotation, const float angle);
		void SetScale(const bsm::Vec3& Scale);
		void SetAllColor(const bsm::Col4 color);
		void StopEffect();

	};

	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのインターフェイス
	//--------------------------------------------------------------------------------------
	class EfkInterface : public ObjectInterface, public ShapeInterface {
		Effekseer::ManagerRef m_Manager;
		EffekseerRendererDX11::RendererRef m_renderer;
		friend class EfkPlay;
		friend class EfkEffect;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスラトクタ
		*/
		//--------------------------------------------------------------------------------------
		EfkInterface();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
+		*/
//--------------------------------------------------------------------------------------
		virtual ~EfkInterface();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ビューと射影行列を設定する
		@param[in]	view	ビュー行列
		@param[in]	proj	射影行列
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	描画処理。
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDraw()override;

	};



}
//end basecross
