/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"


namespace basecross{
	void Object::OnCreate() {
		m_Transform = GetComponent<Transform>();
		AddTag(L"Object");
	}
	void StageBuilder::OnCreate() {
		m_Stage = GetStage();
		wstring path = App::GetApp()->GetDataDirWString() + L"Levels/";
		m_Csv.SetFileName(path + m_CsvFileName);
		m_Csv.ReadCsv();
	}
	void StageBuilder::LoadCsv() {
		auto& csvVec = m_Csv.GetCsvVec();

		vector<wstring> objInfo = {};
		for (auto& info : csvVec) {
			objInfo.clear();
			Util::WStrToTokenVector(objInfo, info, L',');

			if (m_Builders.find(objInfo[InfoData::Name]) == end(m_Builders)) continue;

			Vec3 position = WstrToVec3(objInfo[InfoData::Position]);
			Vec3 scale = WstrToVec3(objInfo[InfoData::Scale]);
			Vec3 rotation = WstrToVec3(objInfo[InfoData::Rotation]);
			
			auto obj = m_Builders[objInfo[InfoData::Name]]->Create();
			obj->SetPosition(position);
			obj->SetScale(scale);
			obj->SetRotation(rotation);
		}
	}
}
//end basecross
