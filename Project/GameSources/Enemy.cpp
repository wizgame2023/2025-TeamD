/*!
@file Enemy.cpp
@brief 謨ｵ縺ｪ縺ｩ螳滉ｽ
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

		
		auto ptrColl = AddComponent<CollisionSphere>();
		ptrColl->SetDrawActive(true);//debug
		ptrColl->SetFixed(false);
		ptrColl->AddExcludeCollisionTag(L"Mob");
		
		auto ptrDraw = AddComponent<BcPNTStaticDraw>();
		ptrDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//驥榊鴨繧偵▽縺代ｋ
		auto ptrGra = AddComponent<Gravity>();


		
		auto shadowPtr = AddComponent<Shadowmap>();
		
		shadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		auto& group = GetStage()->GetSharedObjectGroup(L"EnemyGroup");
		group->IntoGroup(GetThis<Enemy>());

		m_Line = m_Stage->AddGameObject<ForecastLine>(GetThis<Enemy>(), false);

		//auto navi = AddComponent<Navigate>();
	}

	void Enemy::OnUpdate()
	{
		ZoneSpeedSet();

		if (m_HP <= 0)
		{
			Dead();
		}
	}
	void Enemy::AsyncUpdate() {
		SearchRange();
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
		m_Line->SetLine(GetDirectionToIntruder(), GetPosition(), 10.0f);
		float searchDistance = 10.0f;
		if (GetDistanceToIntruder() < searchDistance) {
			m_Line->CheckRayCast(Vec3());
		}

		Vec3 target = m_Intruder->GetComponent<Transform>()->GetPosition();
		Vec3 forword = m_Transform->GetForword();
		Vec3 position = m_Transform->GetPosition();
		forword.normalize();
		auto& device = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (device.bConnected) {
			if (device.wPressedButtons & XINPUT_GAMEPAD_Y) {
				m_Position = GetPosition();
				m_Position -= forword * 0.05f;
				SetPosition(m_Position);
			}
		}
		if ((position - target).length() < searchDistance)
		{

			if (IsWithinDetectionRange(forword, GetDirectionToIntruder(), 45.0)) {
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
	//	class LineObject : public GameObject; //邱壹ｒ謠冗判縺吶ｋ繧ｪ繝悶ず繧ｧ繧ｯ繝
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

		//邱壹ｒ讒区�縺吶ｋ2轤ｹ
		m_Vertices = {
			{m_StartPos, m_StartColor},
			{m_EndPos, m_EndColor}
		};
		//蟋狗せ縺ｨ邨らせ繧偵▽縺ｪ縺舌う繝ｳ繝�ャ繧ｯ繧ｹ
		m_Indices = {
			0,1
		};

		//謠冗判
		m_Draw = AddComponent<PCStaticDraw>(); //菴咲ｽｮ縺ｨ濶ｲ縺ｮ縺ｿ
		m_Draw->SetOriginalMeshUse(true); //閾ｪ菴懊＠縺溘Γ繝�す繝･繧剃ｽｿ逕ｨ
		m_Draw->CreateOriginalMesh(m_Vertices, m_Indices); //繝｡繝�す繝･縺ｮ菴懈�
		auto meshResoure = m_Draw->GetMeshResource();
		meshResoure->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); //繝昴Μ繧ｴ繝ｳ縺ｧ縺ｯ縺ｪ縺冗ｨ懃ｷ壹ｒ陦ｨ遉ｺ
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

	void LineObject::VerticesUpdate() {
		m_Vertices = {
			{m_StartPos,m_StartColor},
			{m_EndPos,m_EndColor}
		};
		m_Draw->UpdateVertices(m_Vertices);
	}

	void LineObject::SetLinePosition(const Vec3& startPos, const Vec3& endPos) {
		m_StartPos = startPos;
		m_EndPos = endPos;
		m_Direc = endPos - startPos;
		m_Direc = m_Direc.normalize();

		VerticesUpdate();
	}

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
