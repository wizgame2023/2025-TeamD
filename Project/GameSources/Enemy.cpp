/*!
@file Enemy.cpp
@brief 敵など実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	Enemy::Enemy(const shared_ptr<Stage>& stage, const Vec3& position, const Vec3& scale) :
		Character(stage, position, Vec3(), scale) {
	}
	Enemy::~Enemy()
	{
	}
	void Enemy::OnCreate()
	{
		Character::OnCreate();
		m_HP = 3;

		//CollisionSphere衝突判定を付ける
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);
		//描画設定
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//重力をつける
		auto ptrGra = AddComponent<Gravity>();


		//影をつける（シャドウマップを描画する）
		auto shadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		auto& group = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		group->IntoGroup(GetThis<Enemy>());

		m_Line = m_Stage->AddGameObject<ForecastLine>(GetThis<Enemy>());
	}

	void Enemy::OnUpdate()
	{
		ZoneSpeedSet();
		SearchRange();
		if (m_HP <= 0)
		{
			Dead();
		}
	}


	Vec3 Enemy::GetDirectionToIntruder() {
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = m_Intruder->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		offset = offset.normalize();
		return offset;
	}
	float Enemy::GetDistanceToIntruder() {
		Vec3 position = m_Transform->GetPosition();
		Vec3 intruderPosition = m_Intruder->GetComponent<Transform>()->GetPosition();

		Vec3 offset = intruderPosition - position;
		return offset.length();
	}

	void Enemy::ZoneSpeedSet()
	{
		auto player = m_Stage->GetSharedGameObject<Player>(L"Player");
		int state = player->GetStates();
		if ((state & Player::PlayerState::ZONE) == 0) {
			m_ZoneElapsedTime = 1.0f;
		}
		else {
			m_ZoneElapsedTime = 0.2f;
		}
	}

	void Enemy::SearchRange()
	{
		m_Line->SetLine(GetDirectionToIntruder(), m_Transform->GetPosition(), 10.0f);
		m_Line->CheckRayCast(Vec3());
		Vec3 target = m_Intruder->GetComponent<Transform>()->GetPosition();
		Vec3 forword = m_Transform->GetForword();
		Vec3 position = m_Transform->GetPosition();
		forword.normalize();
		float searchDistance = 10.0f;
		if ((position - target).length() < searchDistance)
		{
			if (IsWithinDetectionRange(forword, target - position, 45.0)) {
				//プレイヤーの方向をゆっくり向く
				if (m_Line->CheckHitObjectTag(L"Player")) {
					m_IntruderAlert = true;
				}
				else {
					m_IntruderAlert = false;
				}
			}
			else {
				m_IntruderAlert = false;
			}
		}
		else {
			m_IntruderAlert = false;
		}
	}

	Vec3 Enemy::GetPosition()
	{
		return m_Transform->GetPosition();
	}

	bool Enemy::GetIntruderAlert()
	{
		return m_IntruderAlert;
	}

	void Enemy::Dead() {
		m_Line->Destroy();

		auto gameStage = static_pointer_cast<GameStage>(m_Stage);
		if (gameStage != nullptr) {
			gameStage->EliminateEnemy();
		}
		m_Stage->RemoveGameObject<Enemy>(GetThis<Enemy>());
	}
	void Enemy::OnCollisionEnter(shared_ptr<GameObject>& other)
	{
		if (other->FindTag(L"HitJudge"))
		{
			m_HP -= 1;
		}
	}

	//--------------------------------------------------------------------------------------
	//	class LineObject : public GameObject; //線を描画するオブジェクト
	//--------------------------------------------------------------------------------------
	LineObject::LineObject(const shared_ptr<Stage>& stage
	) :
		LineObject(stage, nullptr, nullptr)
	{
	}
	LineObject::LineObject(const shared_ptr<Stage>& stage,
		const shared_ptr<Character>& player,
		const shared_ptr<Character>& object
	) :
		GameObject(stage),
		m_MainObject(player),
		m_Object(object),
		m_Direc(0.0f),
		m_StartPos(Vec3(0.0f)),
		m_EndPos(Vec3(0.0f)),
		m_StartColor(Col4(1.0f)),
		m_EndColor(Col4(1.0f)),
		m_StartSize(Vec2()),
		m_EndSize(Vec2())
	{
	}
	void LineObject::OnCreate() {

		//線を構成する2点
		m_Vertices = {
			{m_StartPos, m_StartColor},
			{m_EndPos, m_EndColor}
		};
		//始点と終点をつなぐインデックス
		m_Indices = {
			0,1
		};

		//描画
		m_Draw = AddComponent<PCStaticDraw>(); //位置と色のみ
		m_Draw->SetOriginalMeshUse(true); //自作したメッシュを使用
		m_Draw->CreateOriginalMesh(m_Vertices, m_Indices); //メッシュの作成
		auto meshResoure = m_Draw->GetMeshResource(); //メッシュリソースを取得し、プリミティブポロジー（頂点利用方法）を変更する
		meshResoure->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); //ポリゴンではなく稜線を表示
	}
	void LineObject::OnUpdate() {
		auto player = m_MainObject.lock();
		if (!player) return;
		auto enemy = m_Object.lock();
		if (!enemy) return;

		SetLinePosition(player->GetComponent<Transform>()->GetPosition(), LinePos(Vec3(0.0f, 3.0f, 0.0f)));

	}
	Vec3 LineObject::LinePos(Vec3 pos) {
		auto enemy = m_Object.lock();
		if (enemy) {
			Vec3 enemyPos = enemy->GetComponent<Transform>()->GetPosition();
			Vec3 forward = enemy->GetComponent<Transform>()->GetForward();
			float face = atan2f(forward.z, forward.x);
			Vec3 linePos;
			linePos.x = (cosf(face) * pos.x) - (sinf(face) * pos.z);
			linePos.y = pos.y;
			linePos.z = (cosf(face) * pos.z) + (sinf(face) * pos.x);
			return enemyPos + linePos;
		}
	}

	//頂点の更新
	void LineObject::VerticesUpdate() {
		m_Vertices = {
			{m_StartPos,m_StartColor},
			{m_EndPos,m_EndColor}
		};
		m_Draw->UpdateVertices(m_Vertices);
	}

	//頂点の設定
	void LineObject::SetLinePosition(const Vec3& startPos, const Vec3& endPos) {
		m_StartPos = startPos;
		m_EndPos = endPos;
		m_Direc = endPos - startPos;
		m_Direc = m_Direc.normalize();

		VerticesUpdate();
	}

	//線の色の設定
	void LineObject::SetLineColor(const Col4& startColor, const Col4& endColor) {
		m_StartColor = startColor;
		m_EndColor = endColor;

		VerticesUpdate();
	}

	Vec3 LineObject::GetStartPos() {
		return m_StartPos;
	}
	Vec3 LineObject::GetEndPos() {
		return m_EndPos;
	}
	Vec3 LineObject::GetDirec() {
		return m_Direc;
	}

}
//end basecross
