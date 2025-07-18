/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	void File::DrawError(const wstring& str, const wstring& title) {
		MessageBox(App::GetApp()->GetHWnd(), str.c_str(), title.c_str(), MB_OK);
	}

	vector<wstring> File::GetFileData(const wstring& csv, const wstring& folder) {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();

		auto path = mediaPath + folder;

		CsvFile csvFile;
		csvFile.SetFileName(path + csv + L".csv");
		csvFile.ReadCsv();

		auto fullData = csvFile.GetCsvVec();
		if (fullData.size() == 0) {
			DrawError(L"CSVファイルをうまく読み取れませんでした", folder.c_str());
			MessageBox(App::GetApp()->GetHWnd(), L"CSVファイルをうまく読み取れませんでした", folder.c_str(), MB_OK);
			return {};
		}

		return fullData;
	}
	void File::Load(const wstring& csv, const wstring& folder) {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();

		auto path = mediaPath + folder;

		auto fullData = GetFileData(csv, folder);
		vector<wstring> file = {};
		for (auto& data : fullData) {
			file.clear();
			Util::WStrToTokenVector(file, data, L',');

			wstring fileName = file[0];
			wstring registerName = file[1];

			if (fileName.find(L".png") != wstring::npos || fileName.find(L".jpg") != wstring::npos) {
				app->RegisterTexture(registerName, path + fileName);
			}

			else if (fileName.find(L".bmf") != wstring::npos) {
				if (file.size() < 3) {

				}
				wstring modelType = file[2];
				shared_ptr<MeshResource> mesh;
				if (modelType == L"static") {
					mesh = MeshResource::CreateStaticModelMesh(path, fileName);
				}
				else if (modelType == L"bone") {
					mesh = MeshResource::CreateBoneModelMesh(path, fileName);
				}
				else {
					DrawError(L"設定可能なモデルタイプが設定されていません", fileName);
				}
				app->RegisterResource(registerName, mesh);
			}
			else {
				DrawError(L"設定可能なファイル形式ではありません", fileName);
			}
		}

	}
	void File::LoadEfk(const wstring& csv, const wstring& folder, shared_ptr<EffectManager> manager) {
		auto& app = App::GetApp();
		auto mediaPath = app->GetDataDirWString();

		auto path = mediaPath + folder;

		auto fullData = GetFileData(csv, folder);
		vector<wstring> file = {};
		for (auto& data : fullData) {
			file.clear();
			Util::WStrToTokenVector(file, data, L',');

			wstring fileName = file[0];
			wstring registerName = file[1];

			if (fileName.find(L".efk") != wstring::npos) {
				manager->RegisterResource(registerName, path + fileName);
			}
			else {
				DrawError(L"設定可能なファイル形式ではありません", fileName);
			}
		}

	}
}
//end basecross
