/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void RootPointer::OnUpdate() {
		if (m_RootPointer.size() > m_Line.size()) {
			int offset = m_RootPointer.size() - m_Line.size();
			for (int i = 0; i < offset; i++) {
				auto line = m_Stage->AddGameObject<ForecastLine>(GetThis<RootPointer>(),false);
				m_Line.push_back(line);
			}
		}

		for (int i = 0; i < m_RootPointer.size(); i++) {
			Vec3 direction = m_RootPointer[i]->GetPosition() - GetPosition();
			m_Line[i]->SetLine(direction.normalize(), GetPosition(), direction.length());
		}
	}
}
//end basecross
