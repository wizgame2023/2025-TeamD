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
				wstring difficulty = objInfo[GetInfoIndex(L"difficulty")];
				if (difficulty == L"easy") {
					GameManager::Instance()->SetDifficulty(Difficulty::Easy);
				}else if (difficulty == L"normal") {
					GameManager::Instance()->SetDifficulty(Difficulty::Normal);
				}else if (difficulty == L"hard") {
					GameManager::Instance()->SetDifficulty(Difficulty::Hard);
				}

				GameManager::Instance()->SetDifficulty(Difficulty::Hard);
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

			obj->OnAfterCreate();
		}
		m_Builders.clear();
		spawner->OnAfterCreate();
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
		return obj;
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
