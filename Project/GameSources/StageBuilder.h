/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class Object;
	class Instance : public GameObject{
	public:
		Instance(const shared_ptr<Stage>& stage) : GameObject(stage){}
		virtual ~Instance(){}

		virtual shared_ptr<Object> Create() {
			return nullptr;
		}
	};
	template<typename T>
	class ObjectInstance : public Instance{
		shared_ptr<Stage> m_Stage;
	public:
		ObjectInstance(const shared_ptr<Stage>& stage) :Instance(stage), m_Stage(stage){}
		~ObjectInstance(){}

		virtual shared_ptr<Object> Create() {
			return m_Stage->AddGameObject<T>();
		}
	};
	class Object : public GameObject {
	protected:
		Vec3 m_Position;
		Vec3 m_Scale;
		Vec3 m_Rotation;

		shared_ptr<Transform> m_Transform;
		shared_ptr<Stage> m_Stage;

		bool m_IsEndAsyncUpdate;
	public :
		Object(const shared_ptr<Stage>& stage,Vec3 position,Vec3 rotation, Vec3 scale) :
			GameObject(stage),m_Position(position),m_Scale(scale),m_Rotation(rotation),m_IsEndAsyncUpdate(true){}
		Object(const shared_ptr<Stage>& stage) : Object(stage,Vec3(),Vec3(1.0f),Vec3()) {}

		virtual ~Object(){}
		virtual void OnCreate()override;
		virtual void AsyncUpdate(){}

		void StartAsync() {
			m_IsEndAsyncUpdate = false;
		}
		void EndAsync() {
			m_IsEndAsyncUpdate = true;
		}
		void SetPosition(Vec3 position) {
			m_Position = position;
			m_Transform->SetPosition(position);
		}
		void SetScale(Vec3 scale) {
			m_Scale = scale;
			m_Transform->SetScale(scale);
		}
		void SetRotation(Vec3 degRotation) {
			Vec3 radRotation = Vec3(XMConvertToRadians(degRotation.x), XMConvertToRadians(degRotation.y), XMConvertToRadians(degRotation.z));
			m_Rotation = radRotation;

			m_Transform->SetRotation(radRotation);
		}
		void RotateY(float degree) {
			m_Rotation.y = XMConvertToRadians(degree);
			Quat q = Quat();
			q = q * Quat(0.0f, sin(m_Rotation.y / 2.0f), 0.0f, cos(m_Rotation.y / 2.0f));
			m_Transform->SetQuaternion(q);
		}
		Vec3 GetPosition() {
			return m_Position;
		}
		Vec3 GetScale() {
			return m_Scale;
		}
		Vec3 GetRotation() {
			return m_Rotation;
		}
	};

	class StageBuilder : public GameObject
	{
		enum InfoData {
			Name,
			Position,
			Scale,
			Rotation,
			Color,
			Hp
		};
		wstring m_CsvFileName;
		CsvFile m_Csv;
		
		shared_ptr<Stage> m_Stage;
		map<wstring, shared_ptr<Instance>> m_Builders;

		float m_Scale;

	public:
		StageBuilder(const shared_ptr<Stage>& stage,const wstring& fileName,const float scale = 1.0f) :
			GameObject(stage),m_CsvFileName(fileName),m_Scale(scale)
		{};
		virtual ~StageBuilder() {};
		virtual void OnCreate()override;

		template<typename T>
		void Register(const wstring& name) {
			m_Builders.emplace(name, ObjectFactory::Create<ObjectInstance<T>>(m_Stage));
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
