/*!
@file Scene.cpp
@brief
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	
	void ResultCamera::OnCreate() {
		SetEye(m_Eye);
		SetAt(m_At);
		m_Direction = m_Eye - m_At;
		m_Direction = m_Direction.normalize();
	}

	void ResultCamera::OnUpdate() {
		float elpased = App::GetApp()->GetElapsedTime();
		Vec3 at = GetAt();
		Vec3 eye = GetEye();

		Vec3 target = m_Player->GetComponent<Transform>()->GetForward() + Vec3(0, 0.1f, 0);
		Vec3 side = cross(target, Vec3(0, 1, 0));
		Vec3 offset = target - m_Direction;
		if (offset.length() < 0.05f) {
			return;
		}
		offset = offset.normalize();
		Vec3 atOffset = (m_At - side * 0.5f) - at;
		atOffset = atOffset.normalize();

		m_Direction += offset * elpased;
		at += atOffset * elpased;

		eye = at + m_Direction * 2.0f;
		SetEye(eye);
		SetAt(at);
	}
}
