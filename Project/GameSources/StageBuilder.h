/*!
@file Character.h
@brief
*/

#pragma once
#include "stdafx.h"
#include"GameManager.h"
namespace basecross {
	class Object;
	class RootPointer;
	class Legion;
	/// <summary>
	/// ObjectInstanceクラスを同じクラスで管理するための仮想クラス
	/// </summary>
	class Instance : public GameObject {
	public:
		Instance(const shared_ptr<Stage>& stage) : GameObject(stage) {}
		virtual ~Instance() {}

		/// <summary>
		/// オブジェクト生成
		/// </summary>
		/// <returns>生成したObject</returns>
		virtual shared_ptr<Object> Create() {
			return nullptr;
		}

	};

	/// <summary>
	/// オブジェクトを生成するインスタンスクラス
	/// </summary>
	/// <typeparam name="T">生成するクラス</typeparam>
	template<typename T>
	class ObjectInstance : public Instance {
		shared_ptr<Stage> m_Stage;
	public:
		ObjectInstance(const shared_ptr<Stage>& stage) :Instance(stage), m_Stage(stage) {}
		~ObjectInstance() {}

		/// <summary>
		/// オブジェクト生成
		/// </summary>
		/// <returns>生成したObject</returns>
		virtual shared_ptr<Object> Create() {
			return m_Stage->AddGameObject<T>();
		}

	};
	class Object : public GameObject {
	protected:
		Vec3 m_Position;//現在の位置
		Vec3 m_Scale;//現在のサイズ
		Vec3 m_Rotation;//現在の回転

		shared_ptr<Transform> m_Transform;//位置管理
		shared_ptr<GameStage> m_Stage;//現在のステージ

		bool m_IsEndAsyncUpdate;//非同期処理管理用フラグ
		
		mutex m_Mutex;//非同期処理用

	public:
		Object(const shared_ptr<Stage>& stage, Vec3 position, Vec3 rotation, Vec3 scale) :
			GameObject(stage), m_Position(position), m_Scale(scale), m_Rotation(rotation), m_IsEndAsyncUpdate(true){
		}

		/// <summary>
		/// ステージ生成用
		/// </summary>
		/// <param name="stage">現在のステージ</param>
		Object(const shared_ptr<Stage>& stage) : Object(stage, Vec3(), Vec3(1.0f), Vec3()) {}

		virtual ~Object() {}
		virtual void OnCreate()override;

		/// <summary>
		/// StageBuilderで生成した後に呼び出される
		/// </summary>
		virtual void OnAfterCreate(){}

		/// <summary>
		/// 非同期処理
		/// </summary>
		virtual void AsyncUpdate() {}

		/// <summary>
		/// Transformへの参照
		/// </summary>
		/// <returns>Transform</returns>
		shared_ptr<Transform>& GetTransform() {
			return m_Transform;
		}

		/// <summary>
		/// ゲーム時間倍率を考慮した経過時間を取得
		/// </summary>
		/// <returns>経過時間</returns>
		float GetElpased() {
			return App::GetApp()->GetElapsedTime() * GameManager::Instance()->GetTimeRate();
		}

		/// <summary>
		/// 非同期処理開始
		/// </summary>
		void StartAsync() {
			m_IsEndAsyncUpdate = false;
		}

		/// <summary>
		/// 非同期処理終了
		/// </summary>
		void EndAsync() {
			m_IsEndAsyncUpdate = true;
		}

		/// <summary>
		/// 位置を設定
		/// </summary>
		/// <param name="position">設定する位置</param>
		void SetPosition(Vec3 position) {
			m_Position = position;
			m_Transform->SetPosition(position);
		}

		/// <summary>
		/// サイズを設定
		/// </summary>
		/// <param name="scale">設定するサイズ</param>
		void SetScale(Vec3 scale) {
			m_Scale = scale;
			m_Transform->SetScale(scale);
		}

		/// <summary>
		/// 回転を設定
		/// </summary>
		/// <param name="rotation">設定したいラジアン角</param>
		void SetRotation(Vec3 rotation) {
			m_Rotation = rotation;

			m_Transform->SetRotation(rotation);
		}

		/// <summary>
		/// 現在位置を取得
		/// </summary>
		/// <returns>現在位置</returns>
		Vec3 GetPosition() {
			m_Position = m_Transform->GetPosition();
			return m_Position;
		}

		/// <summary>
		/// 現在のサイズを取得
		/// </summary>
		/// <returns>現在のサイズ</returns>
		Vec3 GetScale() {
			m_Scale = m_Transform->GetScale();
			return m_Scale;
		}

		/// <summary>
		/// 現在の回転を取得
		/// </summary>
		/// <returns>現在の回転</returns>
		Vec3 GetRotation() {
			m_Rotation = m_Transform->GetRotation();
			return m_Rotation;
		}

		/// <summary>
		/// 前方ベクトルを取得
		/// </summary>
		/// <returns>前方ベクトル</returns>
		Vec3 GetForward() {
			return m_Transform->GetForward();
		}

	};

	/// <summary>
	/// ステージ生成を管理するクラス
	/// </summary>
	class StageBuilder : public GameObject
	{
		wstring m_CsvFileName;//参照するcsvファイル名
		CsvFile m_Csv;//csv読み込み用

		shared_ptr<Stage> m_Stage;//現在のステージ
		map<wstring, shared_ptr<Instance>> m_Builders;//登録されているオブジェクト
		vector<wstring> m_InfoNames;//情報保存用
		map<int, shared_ptr<Legion>> m_Legions;//ウェーブ管理
		float m_Scale;//生成するステージのサイズ比

	public:
		StageBuilder(const shared_ptr<Stage>& stage, const wstring& fileName, const float scale = 1.0f) :
			GameObject(stage), m_CsvFileName(fileName), m_Scale(scale)
		{
		};
		virtual ~StageBuilder() {};
		virtual void OnCreate()override;

		/// <summary>
		/// 生成するクラスの登録
		/// </summary>
		/// <typeparam name="T">生成するクラス</typeparam>
		/// <param name="name">登録名</param>
		template<typename T>
		void Register(const wstring& name) {
			m_Builders.emplace(name, ObjectFactory::Create<ObjectInstance<T>>(m_Stage));
		}
		/// <summary>
		/// csvの読み込み
		/// </summary>
		void LoadCsv();

		/// <summary>
		/// オブジェクトの生成
		/// </summary>
		/// <param name="date">文字データ</param>
		/// <returns>生成したshared_ptr</returns>
		shared_ptr<Object> CreateObject(vector<wstring> date);

		/// <summary>
		/// 指定した情報名に対応するインデックスを取得
		/// </summary>
		/// <param name="infoName">データ配列文字列</param>
		/// <returns>インデックス</returns>
		int GetInfoIndex(const wstring& infoName);

		/// <summary>
		/// 文字列から浮動小数点へ変換
		/// </summary>
		/// <param name="data">文字列</param>
		/// <returns>浮動小数点</returns>
		float WstrToFlt(const wstring& data) {
			if (data == L"") return 0.0f;
			return stof(data);
		}

		/// <summary>
		/// 文字列から整数型へ変換
		/// </summary>
		/// <param name="data">文字列</param>
		/// <returns>整数型</returns>
		int WstrToInt(const wstring& date) {
			if (date == L"") return 0;
			return stoi(date);
		}

		/// <summary>
		/// 文字列からVec3への変換
		/// </summary>
		/// <param name="data">文字列</param>
		/// <returns>Vec3</returns>
		Vec3 WstrToVec3(const wstring& data) {
			vector<wstring> vec3Str = {};

			Util::WStrToTokenVector(vec3Str, data, L'_');
			return Vec3(WstrToFlt(vec3Str[0]), WstrToFlt(vec3Str[1]), WstrToFlt(vec3Str[2]));
		}

	};
}
//end basecross