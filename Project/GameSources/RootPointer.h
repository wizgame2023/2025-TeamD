/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "StageBuilder.h"
namespace basecross {
	class ForecastLine;

	class RootPointer : public Object {
		wstring m_RootPointerNumber;
		vector<shared_ptr<ForecastLine>> m_Line;
	public:
		vector<shared_ptr<RootPointer>> m_RootPointer;
		RootPointer(const shared_ptr<Stage>& stage) : Object(stage){}
		virtual ~RootPointer(){}
		void SetPointerNumber(const wstring& number) {
			m_RootPointerNumber = number;
		}
		wstring GetPointerNumber() {
			return m_RootPointerNumber;
		}
		void AddPointer(const shared_ptr<RootPointer> pointer) {
			m_RootPointer.push_back(pointer);
		}

		vector<shared_ptr<RootPointer>> GetRootPointer()
		{
			return m_RootPointer;
		}
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
}

//end basecross
