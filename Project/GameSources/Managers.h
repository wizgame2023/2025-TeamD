/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	template<class T>
	class Manager {
	protected:
		Manager() = default;
	public:
		‾Manager(){}

		static T& GetInstance() {
			static T instance;
			return instance;
		}

	private:
		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;

	};

}
//end basecross
