/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	IMPLEMENT_DX11_COMPUTE_SHADER(CSRayCast, App::GetApp()->GetShadersPath() + L"CSRayCast.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(ConstantBufferCalcbody)
}
//end basecross
