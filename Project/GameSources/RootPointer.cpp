/*!
@file Character.cpp
@brief キャラクターなど実体
*/
#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void RootPointer::OnCreate() {
		Object::OnCreate();
		auto group = m_Stage->GetSharedObjectGroup(L"PointerGroup");
		group->IntoGroup(GetThis<RootPointer>());
	}
	void RootPointer::OnUpdate() {
		/*if (m_RootPointer.size() > m_Line.size()) {
			int offset = m_RootPointer.size() - m_Line.size();
			for (int i = 0; i < offset; i++) {
				auto line = m_Stage->AddGameObject<LineCube>(0.04f,Col4(1,0,0,1));
				m_Line.push_back(line);
			}
		}

		for (int i = 0; i < m_RootPointer.size(); i++) {
			Vec3 direction = m_RootPointer[i]->GetPosition() - GetPosition();
			float length = direction.length();
			m_Line[i]->SetLine(Line(GetPosition(),direction.normalize(), length));
		}*/
	}
}
//end basecross
