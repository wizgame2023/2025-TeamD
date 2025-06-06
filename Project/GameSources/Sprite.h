/*!
@file Sprite.h
@brief スプライト
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	struct SpriteAnimation {
		int m_CurrentOrder;
		float m_AnimationTime;
		float m_UpdateInterval;
		bool m_IsLoop;
		bool m_IsReverse;

		vector<int> m_Order;
		void EndAnimation() {
			if (m_IsReverse) {
				m_CurrentOrder = m_Order.size() - 1;
			}
			else {
				m_CurrentOrder = 0;
			}
		}
		SpriteAnimation() {
			m_Order = {};
			m_CurrentOrder = 0;
			m_AnimationTime = 0;
			m_UpdateInterval = 0;
			m_IsLoop = false;
			m_IsReverse = false;
		}
		SpriteAnimation(vector<int> order, float time, float interval, const bool isLoop = false, const bool isReverse = false){
			m_Order = order;
			m_CurrentOrder = 0;
			m_AnimationTime = time;
			m_UpdateInterval = interval;
			m_IsLoop = isLoop;
			m_IsReverse = isReverse;
		}

		SpriteAnimation(int startOrder,int endOrder,float time,float interval,const bool isLoop = false,const bool isReverse = false){
			vector<int> order;
			for (int i = startOrder;i <= endOrder;i++) {
				order.push_back(i);
			}
			m_Order = order;
			m_CurrentOrder = 0;
			m_AnimationTime = time;
			m_UpdateInterval = interval;
			m_IsLoop = isLoop;
			m_IsReverse = isReverse;
		}
	};
	//----------------------------------------------------------
	//
	//	画像表示クラス						
	//																																
	//----------------------------------------------------------
	class Sprite : public GameObject {
		//テクスチャキー
		wstring m_TexKey;
		//サイズ
		Vec2 m_Size;
		//位置
		Vec3 m_Pos;
		//表示基を中心にするか
		bool m_IsUseCenterSprite;
		//アニメーションがあるか
		bool m_IsAnimation;
		//アニメーション切り替え時間
		float m_AnimationChangeTime;
		//アニメーション切り替え時間計測
		float m_AnimationTimer;
		//使用するアニメーションのコマ数
		int m_UseIndex;
		//現在のコマ
		int m_Index;
		//画像の切り取り数
		Vec2 m_cutUV;
		//アニメーションのUV
		vector<vector<Vec2>> m_AnimationUV;
		//頂点
		vector<VertexPositionColorTexture> m_Vertices;
		//描画コンポーネント
		shared_ptr<PCTSpriteDraw> m_Draw;
		//位置コンポーネント
		shared_ptr<Transform> m_Transform;
		//スクリーンサイズ
		Vec2 m_ScreenSize;
		//アニメーションマップ
		map<wstring, SpriteAnimation> m_Animations;
		//現在のアニメーション
		SpriteAnimation m_CurrentAnimation;
		//前のアニメーション
		SpriteAnimation m_BeforeAnimation;

		//アニメーション処理
		void Animation();
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUV, const bool useCenter = false, const float changeTime = 0.5f, const int useIndex = -1, const bool isAnimation = true) :
			GameObject(ptr),
			m_TexKey(texKey),
			m_Pos(pos), m_Size(size),
			m_cutUV(cutUV),
			m_IsUseCenterSprite(useCenter),
			m_Index(0), m_UseIndex(useIndex),
			m_IsAnimation(isAnimation),
			m_AnimationChangeTime(changeTime), m_AnimationTimer(0.0f),
			m_ScreenSize(0, 0) {
		}

	public:
		Sprite(const shared_ptr<Stage>& ptr,const wstring& texKey,Vec3 pos,Vec2 size) : Sprite(ptr, texKey, pos, size, { 1,1 }, false, 1, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv) : Sprite(ptr, texKey, pos, size, cutUv, false, 0, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv, int useIndex) : Sprite(ptr, texKey, pos, size, cutUv, false, 0, useIndex, true) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, bool useCenter) : Sprite(ptr, texKey, pos, size, { 1,1 }, useCenter, 0, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv, bool useCenter) : Sprite(ptr, texKey, pos, size, cutUv, useCenter, 0, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv,int useIndex, bool useCenter) : Sprite(ptr, texKey, pos, size, cutUv, useCenter, 0, useIndex, true) {}


		virtual ~Sprite(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		//サイズの切り替え
		void UpdateSize(Vec3 size);
		void UpdateSize(Vec2 size);
		Vec2 GetSize() {
			return m_Size;
		}
		//位置の切り替え
		void SetPos(Vec3 pos);
		Vec3 GetPos() {
			return m_Pos;
		}
		
		void SetDiffuse(Col4 color);
		Col4 GetDiffuse();

		void CreateVertex(Vec2 size, vector<Vec2> uv);
		void SetVertex(vector<Vec3> positions);
		//----------------------------------------------------------
		//
		//	UV操作		
		//																																
		//----------------------------------------------------------
		vector<vector<Vec2>> CreateAnimationUV(Vec2 cut,const int& maxIndex = 1024);
		//UVの切り替え
		void UpdateUV(vector<Vec2> uv);

		vector<vector<Vec2>> GetUvVec() {
			return m_AnimationUV;
		}
		vector<Vec2> GetUv(int index) {
			if (m_AnimationUV.size() <= index) {
				return {};
			}
			return m_AnimationUV[index];
		}

		//----------------------------------------------------------
		//
		//	アニメーション操作		
		//																																
		//----------------------------------------------------------
		
		
		//前のアニメーションを取得
		SpriteAnimation GetBeforeAnimation() {
			return m_BeforeAnimation;
		}
		//現在のアニメーションを取得
		SpriteAnimation GetCurrentAnimation() {
			return m_CurrentAnimation;
		}
		//アニメーションを取得
		SpriteAnimation GetAnimation(const wstring& key) {
			if (m_Animations.find(key) != m_Animations.end()) {
				return m_Animations[key];
			}
			return SpriteAnimation();
		}
		//再生するアニメーションを設定
		void SetCurrentAnimation(const wstring& key) {
			if (m_Animations.find(key) != m_Animations.end()) {
				m_CurrentAnimation.EndAnimation();
				m_BeforeAnimation = m_CurrentAnimation;
				m_CurrentAnimation = m_Animations[key];
			}
		}
		//アニメーションを追加
		void AddAnimation(const wstring& key, int startOrder, int endOrder, float time, float interval, const bool isLoop = false, const bool isReverse = false) {
			SpriteAnimation animation = SpriteAnimation(startOrder, endOrder, time, interval, isLoop, isReverse);
			m_Animations.insert(pair<wstring, SpriteAnimation>(key, animation));
		}
		void AddAnimation(const wstring& key, vector<int> order, float time, float interval, const bool isLoop = false, const bool isReverse = false) {
			SpriteAnimation animation = SpriteAnimation(order, time, interval, isLoop, isReverse);
			m_Animations.insert(pair<wstring, SpriteAnimation>(key, animation));
		}
		//アニメーション情報を更新
		void UpdateAnimationData(const wstring& key, SpriteAnimation newAnimation) {
			if (m_Animations.find(key) != m_Animations.end()) {
				m_Animations[key] = newAnimation;
			}
		}
		//----------------------------------------------------------
		//
		//	位置設定テンプレート		
		//																																
		//----------------------------------------------------------
		void ScreenCenter(const Vec2 diff = Vec2(0,0));
		void ScreenTop(const Vec2 diff = Vec2(0, 0));
		void ScreenTopLeft(const Vec2 diff = Vec2(0, 0));
		void ScreenTopRight(const Vec2 diff = Vec2(0, 0));
		void ScreenBottom(const Vec2 diff = Vec2(0, 0));
		void ScreenBottomLeft(const Vec2 diff = Vec2(0, 0));
		void ScreenBottomRight(const Vec2 diff = Vec2(0, 0));
		void ScreenLeft(const Vec2 diff = Vec2(0, 0));
		void ScreenRight(const Vec2 diff = Vec2(0, 0));
	};


	//----------------------------------------------------------
	//																																
	//	数値表示クラス																												
	//																																
	//----------------------------------------------------------
	class NumberSprite : public GameObject {
		vector<shared_ptr<Sprite>> m_Numbers;
		int m_CutNum;
		int m_DisplayNumber;
		int m_DisplayDigit;

		wstring m_TexKey;
		Vec3 m_Pos;
		Vec2 m_Size;

		vector<Vec2> GetUV(int displayDigit);
	public:
		NumberSprite(const shared_ptr<Stage>& ptr,const wstring& texKey) : NumberSprite(ptr,texKey,Vec3(0,0,0),Vec2(200,100),8){}
		NumberSprite(const shared_ptr<Stage>& ptr,const wstring& texKey,Vec3 pos,Vec2 size,int displayDigit) : NumberSprite(ptr, texKey, pos, size, displayDigit,10) {}
		NumberSprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, int displayDigit,int cutNum) : 
			GameObject(ptr) ,
			m_TexKey(texKey),
			m_Pos(pos),m_Size(size),
			m_DisplayDigit(displayDigit),m_DisplayNumber(1234), m_CutNum(cutNum)
		{}
		virtual ~NumberSprite(){}

		virtual void OnCreate();
		virtual void OnUpdate();
		
		void UpdateNumber(int number);
		void SetDiffuse(Col4 color) {
			for (auto& number : m_Numbers) {
				number->SetDiffuse(color);
			}
		}
		void SetActive(bool flag) {
			for (auto& number : m_Numbers) {
				number->SetDrawActive(flag);
			}
		}
		void Destroy();
	};

	//----------------------------------------------------------
	//																																
	//	Sprite操作コンポーネントクラス
	// 
	//	< 使い方 >
	//  AddComponentで使いたい派生クラスを設定
	//  Play() : 更新
	//  Stop() : 停止																				
	//																																
	//----------------------------------------------------------
	class SpriteAction : public Component {
		bool m_IsPlay;
	protected:
		shared_ptr<SpriteBaseDraw> m_Draw;
		shared_ptr<Transform> m_Trans;
	public:
		SpriteAction(const shared_ptr<GameObject>& ptr) : Component(ptr),m_IsPlay(true){}
		virtual ~SpriteAction() {}

		virtual void OnCreate()override;
		virtual void OnDraw()override {}

		virtual void Reset(){}
		void Play() {
			m_IsPlay = true;
		}
		void Stop() {
			m_IsPlay = false;
		}

		bool IsPlay() {
			return m_IsPlay;
		}
	};
	//----------------------------------------------------------
	//																																
	//	Sprite操作 : 点滅																								
	//																																
	//----------------------------------------------------------
	class SpriteFlash : public SpriteAction {
		float m_FlashSpeed;
	public:
		SpriteFlash(const shared_ptr<GameObject>& ptr,float flashSpeed) : SpriteAction(ptr),
			m_FlashSpeed(flashSpeed){}
		virtual ~SpriteFlash(){}

		virtual void OnUpdate()override;
		virtual void Reset()override;
		void SetFlashSpeed(float flashSpeed) {
			m_FlashSpeed = flashSpeed;
		}

	};
	//----------------------------------------------------------
	//																																
	//	Sprite操作 : 拡大縮小																								
	//																																
	//----------------------------------------------------------
	class SpriteScaling : public SpriteAction {
		float m_ScalingSpeed;
		Vec3 defaultSize;
		float m_Ratio;
		float m_MaxRatio;
		float m_MinRatio;

	public:
		SpriteScaling(const shared_ptr<GameObject>& ptr, float scalingSpeed, float max, float min) : SpriteAction(ptr), 
			m_ScalingSpeed(scalingSpeed), m_MaxRatio(max), m_MinRatio(min),defaultSize(0,0,0),m_Ratio(1.0f) {}
		virtual ~SpriteScaling() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetScalingSpeed(float scalingSpeed) {
			m_ScalingSpeed = scalingSpeed;
		}
		void SetMax(float max) {
			m_MaxRatio = max;
		}
		void SetMin(float min) {
			m_MinRatio = min;
		}

	};
	//----------------------------------------------------------
	//																																
	//	Sprite操作 : フェードイン・フェードアウト																								
	//																																
	//----------------------------------------------------------
	class SpriteFade : public SpriteAction {
		float m_FadeSpeed;
		bool m_IsFadeOut;
		bool m_IsFinished;
	public:
		SpriteFade(const shared_ptr<GameObject>& ptr,float fadeSpeed) : SpriteAction(ptr),m_FadeSpeed(fadeSpeed),m_IsFadeOut(true),m_IsFinished(false){}
		virtual ~SpriteFade(){}

		virtual void OnUpdate()override;

		bool IsFadeOut() {
			return m_IsFadeOut;
		}
		void FadeOut() {
			m_IsFadeOut = true;
			m_IsFinished = false;
			m_Draw->SetDiffuse(Col4(1, 1, 1, 0));
		}
		void FadeIn() {
			m_IsFadeOut = false;
			m_IsFinished = false;
			m_Draw->SetDiffuse(Col4(1, 1, 1, 1));
		}
		bool IsFinish() {
			return m_IsFinished;
		}
	};
	//----------------------------------------------------------
	//																																
	//	ボタン																		
	//																																
	//----------------------------------------------------------
	class SpriteButton : public SpriteAction {
		shared_ptr<SpriteBaseDraw> m_SpriteDraw;
		shared_ptr<SpriteAction> m_Effect;
		function<void(shared_ptr<ObjectInterface>&)> m_Function;
		function<void(shared_ptr<SpriteButton>&)> m_AddFunction;
		shared_ptr<ObjectInterface> m_ArgmentObject;
		bool m_IsSelect;
		bool m_IsActive;
		wstring m_BelongGroup;

		vector<shared_ptr<Sprite>> m_FrontSprite;

		wstring m_UnSelectTexture;
		wstring m_SelectedTexture;
		Col4 m_UnSelectColor;
		Col4 m_SelectedColor;

		int m_GroupOrder;
		SpriteButton(shared_ptr<GameObject>& ptr, const wstring& defaultTexture, const wstring& group, const wstring& selectedTexture,Col4 selectedColor) :
			SpriteAction(ptr),
			m_BelongGroup(group),
			m_UnSelectTexture(defaultTexture),m_UnSelectColor(Col4()),
			m_SelectedTexture(selectedTexture), m_SelectedColor(selectedColor),
			m_IsSelect(false),m_IsActive(true),
			m_GroupOrder(0)
		{}
	public:

		SpriteButton(shared_ptr<GameObject>& ptr, const wstring& defaultTexture,const wstring& group,const wstring& selectedTexture) :
			SpriteButton(ptr,defaultTexture,group,selectedTexture,Col4(0,0,0,0))
		{}
		SpriteButton(shared_ptr<GameObject>& ptr, const wstring& defaultTexture, const wstring& group, Col4 selectedColor) :
			SpriteButton(ptr,defaultTexture,group,L"",selectedColor)
		{}
		virtual ~SpriteButton(){}

		virtual void OnCreate()override;

		virtual void OnUpdate()override;

		template<class Comp,typename... params>
		void AddEffect(params&&... param) {
			m_Effect = GetGameObject()->AddComponent<Comp>(param...);
		}
		shared_ptr<SpriteAction> GetEffect() {
			return m_Effect;
		}
		void AddFunction(function<void(shared_ptr<SpriteButton>&)> func) {
			m_AddFunction = func;
		}
		vector<shared_ptr<Sprite>> GetFrontSprite() {
			return m_FrontSprite;
		}
		shared_ptr<Sprite> GetFrontSprite(int index) {
			if (m_FrontSprite.size() <= index) {
				return nullptr;
			}
			return m_FrontSprite[index];
		}
		void AddFrontSprite(shared_ptr<Sprite>& sprite) {
			sprite->GetComponent<Transform>()->SetParent(GetGameObject());
			m_FrontSprite.push_back(sprite);
		}
		void SetFunction(function<void(shared_ptr<ObjectInterface>&)> func,const shared_ptr<ObjectInterface>& object) {
			m_Function = func;
			if (object == nullptr) {
				m_ArgmentObject = GetStage();
			}
			else {
				m_ArgmentObject = object;
			}
		}
		void Func() {
			m_Function(m_ArgmentObject);
		}
		void Select() {
			m_IsSelect = true;
		}
		void UnSelect() {
			m_IsSelect = false;
		}
		void Open() {
			SetUpdateActive(true);
			m_SpriteDraw->SetDrawActive(true);
		}
		void Close() {
			SetUpdateActive(false);
			m_SpriteDraw->SetDrawActive(false);
			UnSelect();
		}
		void SetOrder(int order) {
			m_GroupOrder = order;
		}
		int GetOrder() {
			return m_GroupOrder;
		}
		void SetActive(bool flag) {
			m_IsActive = flag;
		}
		bool GetActive() {
			return m_IsActive;
		}
	};
	//----------------------------------------------------------
	//																																
	//	ボタン入力データ																							
	//																																
	//----------------------------------------------------------
	enum class InputMode {
		Button,
		Stick
	};
	enum class StickMode {
		LX,
		LY,
		RX,
		RY
	};
	struct InputData {
		int m_Input;
		int m_MoveAmount;

		InputMode m_Mode;
		StickMode m_StickMode;

		bool m_BeforeStickState;
		float m_StickDeadZone;

		InputData(int input,int amount) : m_Input(input),m_MoveAmount(amount),m_Mode(InputMode::Button),m_StickMode(StickMode::LX), m_BeforeStickState(false), m_StickDeadZone(0.0f) {}
		InputData(StickMode mode, int amount,float deadZone) : m_Input(0), m_MoveAmount(amount), m_Mode(InputMode::Stick),m_StickMode(mode), m_BeforeStickState(false), m_StickDeadZone(deadZone) {}

		bool CheckInput(WORD input){
			return input & m_Input;
		}
		bool CheckInput(float input) {
			if (m_BeforeStickState) {
				if (abs(input) <= m_StickDeadZone) {
					m_BeforeStickState = false;
				}
				return false;
			}
			if (abs(input) > m_StickDeadZone) {
				if (input < 0) {
					m_MoveAmount = -abs(m_MoveAmount);
				}
				else {
					m_MoveAmount = abs(m_MoveAmount);
				}
				m_BeforeStickState = true;
				return true;
			}
			return false;

		}
	};
	
	//----------------------------------------------------------
	//																																
	//	ボタン用マネージャー																							
	//																																
	//----------------------------------------------------------

	class ButtonManager : public GameObject{
		//ボタン格納用配列
		map<wstring, vector<shared_ptr<SpriteButton>>> m_ButtonGroup;
		//選択中の番号
		map<wstring, int> m_SelectIndexes;
		//反映させる移動量
		map<wstring, Vec3> m_GroupMovementAmount;
		//入力
		map<wstring, vector<InputData>> m_InputDates;
		//決定ボタン
		map<wstring, vector<WORD>> m_AcceptButtons;
		//その瞬間に押された決定ボタン
		map<wstring, WORD> m_PressedAccept;
		wstring m_UsingGroup;
		//クリック音のキー
		wstring m_ClickSound;
		//Updateさせるか
		bool m_IsActive;

		shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);
		
		template<typename Comp, typename... params>
		shared_ptr<Sprite> CreateComp(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func, params&&... param) {
			auto sprite = stage->AddGameObject<Sprite>(defaultTex, pos, size, true);
			sprite->AddTag(L"Button");
			shared_ptr<SpriteButton> button = sprite->AddComponent<SpriteButton>(defaultTex, group, L"");
			button->SetFunction(func,object);
			button->AddEffect<Comp>(param...);

			return sprite;
		}

		void InitGroup(const wstring& group);
	public:
		static shared_ptr<ButtonManager> instance;

		ButtonManager(const shared_ptr<Stage>& ptr) : 
			GameObject(ptr),
			m_IsActive(true),m_UsingGroup(L""),m_ClickSound(L"")
		{}
		virtual ~ButtonManager(){}

		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size,function<void(shared_ptr<ObjectInterface>&)> func);
		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size,function<void(shared_ptr<ObjectInterface>&)> func);

		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size,const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);
		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func);

		template<class Comp, typename... params>
		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func, params&&... param) {
			return ButtonManager::instance->CreateComp<Comp>(stage, group, defaultTex, pos, size, nullptr, func, param...);
		}
		template<class Comp, typename... params>
		static shared_ptr<Sprite> Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func, params&&... param) {
			return ButtonManager::instance->CreateComp<Comp>(stage, group, defaultTex, pos, size, object, func, param...);
		}
		

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDestroy()override;

		bool CheckUpdate();
		bool CheckMoveInput(InputData& input);

		shared_ptr<Sprite> GetButtonSprite(const wstring& group, int index) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				auto vec = m_ButtonGroup[group];
				if (vec.size() > index) {
					auto obj = vec[index]->GetGameObject();
					return static_pointer_cast<Sprite>(obj);
				}
			}
			return nullptr;
		}

		int GetSelectIndex(const wstring& group) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				return m_SelectIndexes[group];
			}
		}
		void SetSelectIndex(const wstring& group,int index) {
			if (group == L"") {
				if (index < m_ButtonGroup[m_UsingGroup].size()) {
					m_SelectIndexes[m_UsingGroup] = index;
				}
				return;
			}

			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				if (index < m_ButtonGroup[group].size()) {
					m_SelectIndexes[group] = index;
				}
			}
		}
		bool ExistOpenGroup() {
			for (auto& buttons : m_ButtonGroup) {
				if (buttons.second[0]->GetDrawActive()) {
					return true;
				}
			}
			return false;
		}
		int GetSize(const wstring& group) {
			if (m_InputDates.find(group) != end(m_InputDates)) {
				return m_ButtonGroup[group].size();
			}
			else {
				return 0;
			}
		}
		void SetMoveAmount(const wstring& group, Vec3 target);

		Vec3 GetMoveAmount(const wstring& group) {
			if (m_GroupMovementAmount.find(group) != end(m_GroupMovementAmount)) {
				return m_GroupMovementAmount[group];
			}
		}
		void SetSound(const wstring& sound);

		void AddAcceptButton(const wstring& group, WORD accept) {
			if (m_AcceptButtons.find(group) != end(m_AcceptButtons)) {
				m_AcceptButtons[group].push_back(accept);
			}
			else {
				vector<WORD> button = {};
				button.push_back(accept);
				m_AcceptButtons.emplace(group, button);
				m_PressedAccept.emplace(group, 0);
			}
		}
		WORD GetPressedAccept(const wstring& group) {
			if (m_PressedAccept.find(group) != end(m_PressedAccept)) {
				return m_PressedAccept[group];
			}
			return 0;
		}
		void SetInput(const wstring& group,int input,int amount) {
			SetInput(group, InputData(input, amount));
		}
		void SetInput(const wstring& group, InputData data) {
			if (m_InputDates.find(group) != end(m_InputDates)) {
				m_InputDates[group].push_back(data);
			}
			else {
				vector<InputData> dates = {};
				dates.push_back(data);
				m_InputDates.emplace(group, dates);
			}
		}
		void OpenAll() {
			for (auto& buttons : m_ButtonGroup) {
				Open(buttons.first);
			}
		}
		void CloseAll() {
			for (auto& buttons : m_ButtonGroup) {
				Close(buttons.first);
			}
		}
		void OpenAndUse(const wstring& group) {
			Open(group);
			UseGroup(group);
		}
		void Open(const wstring& group) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				for (auto& button : m_ButtonGroup[group]) {
					button->Open();
				}
			}
			SetActive(true);
		}
		void Close(const wstring& group) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				for (auto& button : m_ButtonGroup[group]) {
					button->Close();
				}
				m_SelectIndexes[group] = 0;
				SetActive(false);
			}
		}
		void SetActive(bool flag) {
			m_IsActive = flag;
		}
		bool GetActive() {
			return m_IsActive;
		}
		void LimitIndex() {
			int maxIndex = m_ButtonGroup[m_UsingGroup].size() - 1;
			int minIndex = 0;
			m_SelectIndexes[m_UsingGroup] = max(minIndex, m_SelectIndexes[m_UsingGroup]);
			m_SelectIndexes[m_UsingGroup] = min(maxIndex, m_SelectIndexes[m_UsingGroup]);
		}
		bool CheckOverIndex(int amount) {
			int index = m_SelectIndexes[m_UsingGroup];
			index += amount;
			if (index >= m_ButtonGroup[m_UsingGroup].size() || index < 0) {
				return false;
			}
			return true;
		}
		wstring GetUseGroup() {
			return m_UsingGroup;
		}
		void UseGroup(const wstring& group) {
			if (m_SelectIndexes.find(group) != end(m_SelectIndexes)) {
				InitGroup(m_UsingGroup);
				//m_SelectIndexes[m_UsingGroup] = 0;
				m_UsingGroup = group;
			}
		}
		bool CompareUseGroup(const wstring& group) {
			return m_UsingGroup == group;
		}
		void Register(const wstring& group, shared_ptr<SpriteButton>& button) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				m_ButtonGroup[group].push_back(button);
			}
			else {
				if (m_ButtonGroup.size() == 0) {
					m_UsingGroup = group;
				}
				vector<shared_ptr<SpriteButton>> buttons = {};
				buttons.push_back(button);
				m_ButtonGroup.emplace(group, buttons);
				m_SelectIndexes.emplace(group, 0);
				m_GroupMovementAmount.emplace(group, Vec3());
			}
			button->SetOrder(m_ButtonGroup[group].size() - 1);
		}
		void DeleteGroup(const wstring& group) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				m_ButtonGroup.erase(group);
				m_SelectIndexes.erase(group);
			}
		}

		void AddFrontSprite(const wstring& group,int index,shared_ptr<Sprite>& sprite) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				if (m_ButtonGroup[group].size() > index) {
					m_ButtonGroup[group][index]->AddFrontSprite(sprite);
				}
			}
		}
		void AddFunction(const wstring& group,function<void(shared_ptr<SpriteButton>&)> func) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				for (auto& buttons : m_ButtonGroup[group]) {
					buttons->AddFunction(func);
				}
			}
		}
		void AddFunction(const wstring& group,int index, function<void(shared_ptr<SpriteButton>&)> func) {
			if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
				if (index < 0 || index >= m_ButtonGroup[group].size()) return;
				m_ButtonGroup[group][index]->AddFunction(func);
			}
		}
		
	};

	//----------------------------------------------------------
	//																																
	//	板ポリ用																					
	//																																
	//----------------------------------------------------------

	class Board : public GameObject {
		shared_ptr<PNTStaticDraw> m_Draw;
		shared_ptr<Transform> m_Trans;

		Vec3 m_StartPos;
		Vec3 m_Size;
		Vec3 m_Offset;
		wstring m_TexKey;
		bool m_IsBillBoard;

		vector<VertexPositionNormalTexture> m_Vertices;
	public:
		Board(shared_ptr<Stage>& ptr, const wstring& key,Vec3 pos, Vec3 size,const bool& isBillBoard = true) : 
			GameObject(ptr),
			m_TexKey(key), m_StartPos(pos),m_Offset(Vec3()), m_Size(size), m_IsBillBoard(isBillBoard) { }
		virtual ~Board() {}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void SetOffset(Vec3 offset) {
			m_Offset = offset;
		}
		shared_ptr<PNTStaticDraw> GetDraw() {
			return m_Draw;
		}
		void SetColor(Col4 color) {
			m_Draw->SetDiffuse(color);
		}
		vector<VertexPositionNormalTexture> GetVertices() {
			return m_Vertices;
		}
	};
}
//end basecross
