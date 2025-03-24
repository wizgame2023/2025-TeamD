/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"


namespace basecross{
	void Object::OnCreate() {
		m_Stage = GetStage();

		m_Transform = GetComponent<Transform>();
		SetPosition(Vec3(0,1,0));
		SetScale(m_Scale);
		SetRotation(m_Rotation);

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
		map<wstring, shared_ptr<RootPointer>> rootPointers;
		int enemyCount = 0;
		for (auto& info : csvVec) {
			objInfo.clear();
			Util::WStrToTokenVector(objInfo, info, L',');

			if (m_Builders.find(objInfo[InfoData::Name]) == end(m_Builders)) continue;

			Vec3 position = WstrToVec3(objInfo[InfoData::Position]);
			Vec3 scale = WstrToVec3(objInfo[InfoData::Scale]);
			Vec3 rotation = WstrToVec3(objInfo[InfoData::Rotation]);
			
			auto obj = m_Builders[objInfo[InfoData::Name]]->Create();
			obj->SetPosition(position * m_Scale);
			obj->SetScale(scale * m_Scale);
			obj->SetRotation(rotation);

			wstring dateType = objInfo[objInfo.size() - 1];
			if (dateType == L"Pointer") {
				auto pointer = static_pointer_cast<RootPointer>(obj);
				if (obj != nullptr) {
					pointer->SetPointerNumber(objInfo[5]);
					rootPointers.emplace(objInfo[4], pointer);
				}
			}

			if (objInfo[InfoData::Name] == L"mob") {
				enemyCount++;
			}
		}
		m_Builders.clear();
		auto gameStage = static_pointer_cast<GameStage>(GetStage());
		if (gameStage != nullptr) {
			gameStage->SetMaxEnemyCount(enemyCount);
		}

		for (auto& pointer : rootPointers) {
			wstring numbers = pointer.second->GetPointerNumber();
			vector<wstring> number = {};
			Util::WStrToTokenVector(number, numbers, L'_');
			for (auto& num : number) {
				if (num != L"") {
					if (rootPointers.find(num) != end(rootPointers)) {
						pointer.second->AddPointer(rootPointers[num]);
					}
				}
			}

		}
	}
}
//end basecross
