/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	template<class T>
	class SingletonBase {
	protected:
		SingletonBase() = default;
	public:
		‾SingletonBase(){}

		static T& GetInstance() {
			static T instance;
			return instance;
		}

	private:
		SingletonBase(const SingletonBase&) = delete;
		SingletonBase& operator=(const SingletonBase&) = delete;

	};

}
//end basecross
