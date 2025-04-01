/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"


namespace basecross {
	void Object::OnCreate() {
		m_Stage = static_pointer_cast<GameStage>(GetStage());

		m_Transform = GetComponent<Transform>();
		SetPosition(Vec3(0, 1, 0));
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
			m_InfoNames.clear();
			wstring names = objInfo[objInfo.size() - 1];
			Util::WStrToTokenVector(m_InfoNames, objInfo[objInfo.size() - 1], L'_');

			if (m_Builders.find(objInfo[GetInfoIndex(L"name")]) == end(m_Builders)) continue;

			auto obj = CreateObject(objInfo);

			wstring dateType = objInfo[GetInfoIndex(L"type")];
			if (dateType == L"Pointer") {
				auto pointer = static_pointer_cast<RootPointer>(obj);
				if (obj != nullptr) {
					pointer->SetPointerNumber(objInfo[GetInfoIndex(L"connect")]);
					rootPointers.emplace(objInfo[GetInfoIndex(L"number")], pointer);
				}
			}

			if (objInfo[GetInfoIndex(L"tag")] == L"Enemy") {
				enemyCount++;
				int timeIndex = GetInfoIndex(L"time");
				int defeatIndex = GetInfoIndex(L"defeat");
				if (timeIndex != -1 && defeatIndex != -1) {
					auto boss = static_pointer_cast<BossEnemy>(obj);
					if (!boss) {
						boss->SetCondition(/*WstrToFlt(objInfo[timeIndex])*/0, WstrToFlt(objInfo[defeatIndex]));
					}
				}
			}
		}
		m_Builders.clear();
		auto gameStage = static_pointer_cast<GameStage>(GetStage());
		if (gameStage != nullptr) {
			gameStage->SetMaxEnemyCount(enemyCount);
		}
		RegisterRootPoint(rootPointers);
	}
	/// <summary>
	/// オブジェクトの生成
	/// </summary>
	/// <param name="date">オブジェクトの文字列データ</param>
	/// <returns>生成したオブジェクト</returns>
	shared_ptr<Object> StageBuilder::CreateObject(vector<wstring> date) {
		Vec3 position = WstrToVec3(date[GetInfoIndex(L"position")]);
		Vec3 scale = WstrToVec3(date[GetInfoIndex(L"scale")]);
		Vec3 rotation = WstrToVec3(date[GetInfoIndex(L"rotation")]);

		auto obj = m_Builders[date[GetInfoIndex(L"name")]]->Create();
		obj->SetPosition(position * m_Scale);
		obj->SetScale(scale * m_Scale);
		obj->SetRotation(rotation);

		return obj;
	}
	/// <summary>
	/// 経路探索用のポインターの登録
	/// </summary>
	/// <param name="pointer">登録するポインターのmap</param>
	void StageBuilder::RegisterRootPoint(map<wstring, shared_ptr<RootPointer>> pointer) {
		for (auto& point : pointer) {
			wstring numbers = point.second->GetPointerNumber();
			vector<wstring> number = {};
			Util::WStrToTokenVector(number, numbers, L'_');
			for (auto& num : number) {
				if (num != L"") {
					if (pointer.find(num) != end(pointer)) {
						point.second->AddPointer(pointer[num]);
					}
				}
			}

		}
	}

	int StageBuilder::GetInfoIndex(const wstring& infoName) {

		auto it = find(m_InfoNames.begin(), m_InfoNames.end(), infoName);
		if (it != m_InfoNames.end()) {
			int index = distance(m_InfoNames.begin(), it);
			return index;
		}
		else {
			return -1;
		}
	}
}
//end basecross
