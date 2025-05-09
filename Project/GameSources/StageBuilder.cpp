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
		SetPosition(m_Position);
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
					wstring number = objInfo[GetInfoIndex(L"number")];
					pointer->SetNumber(stoi(number));
					rootPointers.emplace(number, pointer);
				}
			}
			if (dateType == L"Chara") {
				auto chara = static_pointer_cast<Character>(obj);
				if (chara != nullptr) {
					float hp = WstrToFlt(objInfo[GetInfoIndex(L"hp")]);
					chara->InitHP(hp);
				}
			}
			if (objInfo[GetInfoIndex(L"tag")] == L"Enemy") {
				enemyCount++;
				int timeIndex = GetInfoIndex(L"time");
				int defeatIndex = GetInfoIndex(L"defeat");
				if (timeIndex != -1 && defeatIndex != -1) {
					auto boss = static_pointer_cast<BossEnemy>(obj);
					if (boss) {
						boss->SetCondition(WstrToFlt(objInfo[timeIndex]), WstrToFlt(objInfo[defeatIndex]));
					}
				}
				auto enemy = static_pointer_cast<Enemy>(obj);
				int regionIndex = GetInfoIndex(L"region");
				if (regionIndex != -1) {
					int region = WstrToFlt(objInfo[regionIndex]);
					//軍隊番号は1以上
					if (region > 0) {
						if (m_Legions.find(region) == end(m_Legions)) {
							auto regionObject = m_Stage->AddGameObject<Legion>();
							//敵を登録
							regionObject->IntoEnemyGruop(enemy);
							//regionObject->SetLegionNumber(region);
							m_Legions.emplace(region, regionObject);
						}
						else {
							auto& regionObject = m_Legions[region];
							//敵を登録
							regionObject->IntoEnemyGruop(enemy);
						}
					}
				}
			}
		}
		m_Builders.clear();
		ScoreManager::Instance()->SetMaxEnemyCount(enemyCount);
		RegisterRootPoint(rootPointers);
	}
	void StageBuilder::LoadCsv2() {
		auto& csvVec = m_Csv.GetCsvVec();

		vector<wstring> objInfo = {};
		auto spawner = m_Stage->AddGameObject<Spawner>();
		m_Stage->SetSharedGameObject(L"Spawner", spawner);

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
			
			if (dateType == L"Stage") {

			}
			if (dateType == L"Player") {
				wstring hpStr = objInfo[GetInfoIndex(L"hp")];
				auto player = static_pointer_cast<Player>(obj);
				if (player) {
					player->InitHP(WstrToFlt(hpStr));
				}
			}
			if (dateType == L"Wave") {
				wstring maxStr = objInfo[GetInfoIndex(L"max")];
				wstring intervalStr = objInfo[GetInfoIndex(L"interval")];
				auto legion = static_pointer_cast<Legion>(obj);
				legion->SetMaxCount(WstrToFlt(maxStr));
				legion->SetPopInterval(WstrToFlt(intervalStr));
				spawner->AddLegion(legion);
			}
			if (dateType == L"Enemy") {
				wstring waveStr = objInfo[GetInfoIndex(L"wave")];
				wstring hpStr = objInfo[GetInfoIndex(L"hp")];
				auto enemy = static_pointer_cast<Enemy>(obj);
				if (enemy) {
					enemy->InitHP(WstrToFlt(hpStr));
					spawner->AddEnemy(WstrToFlt(waveStr), enemy);
				}
				enemyCount++;
			}
			if (dateType == L"Boss") {
				wstring hpStr = objInfo[GetInfoIndex(L"hp")];
				auto enemy = static_pointer_cast<BossEnemy>(obj);
				if (enemy) {
					enemy->InitHP(WstrToFlt(hpStr));
					spawner->SetBoss(enemy);
				}
				enemyCount++;
			}
		}
		m_Builders.clear();
		ScoreManager::Instance()->SetMaxEnemyCount(enemyCount);
	}
	/// <summary>
	/// オブジェクトの生成
	/// </summary>
	/// <param name="date">オブジェクトの文字列データ</param>
	/// <returns>生成したオブジェクト</returns>
	shared_ptr<Object> StageBuilder::CreateObject(vector<wstring> date) {
		Vec3 position = Vec3(), scale = Vec3(1), rotation = Vec3();
		int index = GetInfoIndex(L"position");
		if (index != -1) {
			position = WstrToVec3(date[index]);
		}

		index = GetInfoIndex(L"scale");
		if (index != -1) {
			scale = WstrToVec3(date[index]);
		}

		index = GetInfoIndex(L"rotation");
		if (index != -1) {
			rotation = WstrToVec3(date[index]);
		}

		auto obj = m_Builders[date[GetInfoIndex(L"name")]]->Create();
		obj->SetPosition(position * m_Scale);
		obj->SetScale(scale * m_Scale);
		obj->SetRotation(Vec3(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)));
		obj->OnAfterCreate();
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
