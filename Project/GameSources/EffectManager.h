/*!
@file EffectManager.h
@brief Effectなど
*/

#pragma once
#include "stdafx.h"


#include <Effekseer.h>
#include <EffekseerRendererDX11.h>


#pragma comment(lib, "Effekseer.lib" )
#pragma comment(lib, "EffekseerRendererDX11.lib" )


namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのエフェクト
	//--------------------------------------------------------------------------------------
	class EffectManeger : public ObjectInterface, public ShapeInterface {

		map<wstring, Effekseer::EffectRef> m_ResMap;		
		Effekseer::Handle handle;
		Effekseer::ManagerRef m_Manager;
		EffekseerRendererDX11::RendererRef m_renderer;
		float m_TotalTime;


		Effekseer::EffectRef m_Effect;
	public:

		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンスラトクタ
		@param[in]	iface	インターフェイス
		@param[in]	filename	エフェクトファイル名
		*/
		//--------------------------------------------------------------------------------------
		EffectManeger();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EffectManeger();
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
		virtual void OnDestroy()override;

		void Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest);
		void SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj);

		void SetEffectSpeed(Effekseer::Handle& handle, const float& speed);
		void SetEffectPause(const bool& pause);
		void PlayEffect(Effekseer::Handle& handle, const wstring& Key, const bsm::Vec3& Emitter, const float freme);
		void CreateEffectInterface();
		void RegisterResource(const wstring& Key, const  wstring& FileName);
		Effekseer::ManagerRef GetEffectInterface()
		{
			return m_Manager;
		}

		Effekseer::EffectRef GetEffectResource(const wstring& Key) const;
		void AddLocation(Effekseer::Handle& handle, const bsm::Vec3& Location);
		void SetLocation(Effekseer::Handle& handle, const bsm::Vec3& Location);
		void SetRotation(Effekseer::Handle& handle, const bsm::Vec3& Rotation, const float angle);
		void SetScale(Effekseer::Handle& handle, const bsm::Vec3& Scale);
		void SetAllColor(Effekseer::Handle& handle, const bsm::Col4 color);
		void StopEffect(Effekseer::Handle& handle);

	};
}
//end basecross
