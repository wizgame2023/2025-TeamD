/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class Object : public GameObject {
	protected:
		Vec3 m_Position;
		Vec3 m_Scale;
		Vec3 m_Rotation;

		shared_ptr<Transform> m_Transform;
	public :
		Object(const shared_ptr<Stage>& stage) : GameObject(stage) {}

		virtual ~Object(){}
		virtual void OnCreate()override;
		virtual shared_ptr<Object> Create() {
			return nullptr;
		}

		void SetPosition(Vec3 position) {
			m_Position = position;
			m_Transform->SetPosition(position);
		}
		void SetScale(Vec3 scale) {
			m_Scale = scale;
			m_Transform->SetScale(scale);
		}
		void SetRotation(Vec3 rotation) {
			Vec3 radRotation = Vec3(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
			m_Transform->SetRotation(radRotation);
		}
	};

	class StageBuilder : public GameObject
	{
		enum InfoData {
			Name,
			Position,
			Scale,
			Rotation,
			Color
		};
		wstring m_CsvFileName;
		CsvFile m_Csv;

		shared_ptr<Stage> m_Stage;
		map<wstring, shared_ptr<Object>> m_Builders;
	public:
		StageBuilder(const shared_ptr<Stage>& stage,const wstring& fileName) :
			GameObject(stage),m_CsvFileName(fileName)
		{};
		virtual ~StageBuilder() {};
		virtual void OnCreate()override;

		template<typename T>
		void Register(const wstring& name) {
			m_Builders.emplace(name, ObjectFactory::Create<FixedBox>(m_Stage));
		}

		void LoadCsv();

		float WstrToFlt(const wstring& data) {
			if (data == L"") return 0.0f;
			return stoi(data);
		}

		Vec3 WstrToVec3(const wstring& data) {
			vector<wstring> vec3Str = {};

			Util::WStrToTokenVector(vec3Str, data, L'_');
			return Vec3(WstrToFlt(vec3Str[0]), WstrToFlt(vec3Str[1]), WstrToFlt(vec3Str[2]));
		}
	};
}
//end basecross
