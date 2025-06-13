/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	void BonePosition::OnUpdate() {
		for (int i = 0, size = m_Bones.size(); i < size; i++) {
			Mat4x4 mat;
			auto boneMat = m_ObjectDraw->GetVecLocalBones()[i];
			mat.translation(m_Bones[i]->GetInitPosition());

			mat *= boneMat;
			mat *= m_ObjectDraw->GetMeshToTransformMatrix();
			mat *= m_ObjectTransform->GetWorldMatrix();

			auto transform = m_Bones[i]->GetComponent<Transform>();
			transform->SetPosition(mat.transInMatrix());
			transform->SetQuaternion(mat.quatInMatrix());
		}
	}
	void BonePosition::CreateBone() {
		auto path = App::GetApp()->GetDataDirWString() + L"Models/Bone/";
		ifstream file(path + m_FileName);
		vector<Vec3> initPositions;
		string line;
		m_ObjectDraw = GetGameObject()->GetComponent<BcPNTBoneModelDraw>(false);
		m_ObjectTransform = GetGameObject()->GetComponent<Transform>(false);
		if (!m_ObjectDraw || !m_ObjectTransform) {
			return;
		}

		int size = m_ObjectDraw->GetVecLocalBones().size();
		while (getline(file, line)) {
			istringstream ss(line);
			string date;
			Vec3 position;
			getline(ss, date, ',');
			position.x = stof(date);
			getline(ss, date, ',');
			position.y = stof(date);
			getline(ss, date, ',');
			position.z = stof(date);

			initPositions.push_back(position);
		}
		for (int i = 0; i < size; i++) {
			auto bone = ObjectFactory::Create<Bone>(GetStage(), GetGameObject(), initPositions[i], i);//AddGameObject<Bone>(GetGameObject(), initPositions[i], i);
			m_Bones.push_back(bone);
		}
	}
	void Bone::OnCreate() {
		m_Transform = GetComponent<Transform>();
		m_Transform->SetScale(Vec3(0.1f));
	}
}
//end basecross
