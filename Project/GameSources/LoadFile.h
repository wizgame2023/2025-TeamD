/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"

namespace basecross {

	class File {
		static vector<wstring> GetFileData(const wstring& csv, const wstring& folder);
		static void DrawError(const wstring& str, const wstring& title);
	public:
		static void Load(const wstring& csv, const wstring& folder);
		static void LoadEfk(const wstring& csv, const wstring& folder, shared_ptr<EffectManager> manager);
	};
}

//end basecross
