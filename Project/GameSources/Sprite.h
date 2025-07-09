/*!
@file Sprite.h
@brief スプライト
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	struct SpriteAnimation {
		size_t		m_CurrentOrder;		//現在のアニメーション番号
		float		m_AnimationTime;	//ごめん。分からない
		float		m_UpdateInterval;	//更新頻度
		bool		m_IsLoop;			//ループするか
		bool		m_IsReverse;		//逆再生か
		vector<int> m_Order;			//描画順

		/// <summary>
		/// アニメーション終了処理
		/// </summary>
		void EndAnimation() {
			if (m_IsReverse) {
				m_CurrentOrder = m_IsLoop ?  m_Order.size() - 1 : 0;
			}
			else {
				m_CurrentOrder = m_IsLoop ? 0 : m_Order.size() - 1;
			}
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		SpriteAnimation() {
			m_Order = {};
			m_CurrentOrder = 0;
			m_AnimationTime = 0;
			m_UpdateInterval = 0;
			m_IsLoop = false;
			m_IsReverse = false;
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="order">描画順</param>
		/// <param name="time">不明</param>
		/// <param name="interval">更新頻度</param>
		/// <param name="isLoop">ループするか</param>
		/// <param name="isReverse">逆再生か</param>
		SpriteAnimation(vector<int> order, float time, float interval, const bool isLoop = false, const bool isReverse = false){
			m_Order = order;
			m_CurrentOrder = 0;
			m_AnimationTime = time;
			m_UpdateInterval = interval;
			m_IsLoop = isLoop;
			m_IsReverse = isReverse;
		}

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="startOrder">描画開始</param>
		/// <param name="endOrder">描画終了</param>
		/// <param name="time">不明</param>
		/// <param name="interval">更新頻度</param>
		/// <param name="isLoop">ループするか</param>
		/// <param name="isReverse">逆再生か</param>
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
		wstring m_TexKey;				//テクスチャキー
		Vec2	m_Size;					//サイズ
		Vec3	m_Pos;					//位置
		bool	m_IsUseCenterSprite;	//表示基を中心にするか
		size_t	m_UseIndex;				//使用するアニメーションのコマ数
		Vec2	m_cutUV;				//画像の切り取り数
		Vec2	m_ScreenSize;			//スクリーンサイズ

		vector<vector<Vec2>>				m_AnimationUV;	//アニメーションのUV
		vector<VertexPositionColorTexture>	m_Vertices;		//頂点

		shared_ptr<PCTSpriteDraw>	m_Draw;			//描画コンポーネント
		shared_ptr<Transform>		m_Transform;	//位置コンポーネント

		bool							m_IsAnimation;		//アニメーションがあるか
		float							m_AnimationTimer;	//アニメーション切り替え時間計測
		map<wstring, SpriteAnimation>	m_Animations;		//アニメーションマップ
		SpriteAnimation					m_CurrentAnimation;	//現在のアニメーション
		SpriteAnimation					m_BeforeAnimation;	//前のアニメーション


		////アニメーション切り替え時間
		//float m_AnimationChangeTime;
		////現在のコマ
		//size_t m_Index;

		/// <summary>
		/// アニメーション処理
		/// </summary>
		void Animation();

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="ptr">ステージ</param>
		/// <param name="texKey">テクスチャキー</param>
		/// <param name="pos">表示位置</param>
		/// <param name="size">表示サイズ</param>
		/// <param name="cutUV">切り取り数</param>
		/// <param name="useCenter">表示基が中心か</param>
		/// <param name="useIndex">初期表示番号</param>
		/// <param name="isAnimation">アニメーションするか</param>
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUV, const bool useCenter = false, const int useIndex = -1, const bool isAnimation = true) :
			GameObject(ptr),
			m_TexKey(texKey),
			m_Pos(pos), m_Size(size),
			m_cutUV(cutUV),
			m_IsUseCenterSprite(useCenter),
			/*m_Index(0),*/ m_UseIndex(useIndex),
			m_IsAnimation(isAnimation),
			/*m_AnimationChangeTime(changeTime), */m_AnimationTimer(0.0f),
			m_ScreenSize(0, 0) {
		}

	public:
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size) : Sprite(ptr, texKey, pos, size, { 1,1 }, false, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv) : Sprite(ptr, texKey, pos, size, cutUv, false, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv, int useIndex) : Sprite(ptr, texKey, pos, size, cutUv, false, useIndex, true) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, bool useCenter) : Sprite(ptr, texKey, pos, size, { 1,1 }, useCenter, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv, bool useCenter) : Sprite(ptr, texKey, pos, size, cutUv, useCenter, -1, false) {}
		Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Vec2 cutUv, int useIndex, bool useCenter) : Sprite(ptr, texKey, pos, size, cutUv, useCenter, useIndex, true) {}


		virtual ‾Sprite(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		/// <summary>
		/// 表示サイズ変更
		/// </summary>
		/// <param name="size">サイズ(Transform)</param>
		void UpdateSize(Vec3 size);

		/// <summary>
		/// 表示サイズ変更
		/// </summary>
		/// <param name="size">サイズ(uv)</param>
		void UpdateSize(Vec2 size);

		/// <summary>
		/// 表示サイズ取得
		/// </summary>
		/// <returns>サイズ</returns>
		Vec2 GetSize() {
			return m_Size;
		}

		/// <summary>
		/// 表示位置変更
		/// </summary>
		/// <param name="pos">表示位置</param>
		void SetPos(Vec3 pos);

		/// <summary>
		/// 表示位置取得
		/// </summary>
		/// <returns>表示位置</returns>
		Vec3 GetPos() {
			return m_Pos;
		}
		
		/// <summary>
		/// 色の変更
		/// </summary>
		/// <param name="color">色</param>
		void SetDiffuse(Col4 color);

		/// <summary>
		/// 色の取得
		/// </summary>
		/// <returns>色</returns>
		Col4 GetDiffuse();

		/// <summary>
		/// 頂点情報の作成・更新
		/// </summary>
		/// <param name="size">表示サイズ</param>
		/// <param name="uv">uv</param>
		void CreateVertex(Vec2 size, vector<Vec2> uv);

		/// <summary>
		/// 頂点の更新
		/// </summary>
		/// <param name="positions">位置情報</param>
		void SetVertex(vector<Vec3> positions);

		//----------------------------------------------------------
		//
		//	UV操作		
		//																																
		//----------------------------------------------------------

		/// <summary>
		/// アニメーション用UVの作成
		/// </summary>
		/// <param name="cut">切り取り数</param>
		/// <param name="maxIndex">使用するuvの最大数</param>
		/// <returns>アニメーション用UV</returns>
		vector<vector<Vec2>> CreateAnimationUV(Vec2 cut,const size_t& maxIndex = 1024);
		
		/// <summary>
		/// UVの更新
		/// </summary>
		/// <param name="uv">UV</param>
		void UpdateUV(vector<Vec2> uv);

		/// <summary>
		/// アニメーション用UVの取得(配列)
		/// </summary>
		/// <returns>アニメーション用UV</returns>
		vector<vector<Vec2>> GetUvVec() {
			return m_AnimationUV;
		}

		/// <summary>
		/// アニメーション用UVの取得(単体)
		/// </summary>
		/// <param name="index">番号</param>
		/// <returns>アニメーション用UV</returns>
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
		
		
		/// <summary>
		/// 前のアニメーションを取得
		/// </summary>
		/// <returns>アニメーション</returns>
		SpriteAnimation GetBeforeAnimation() {
			return m_BeforeAnimation;
		}

		/// <summary>
		/// 現在のアニメーションを取得
		/// </summary>
		/// <returns>アニメーション</returns>
		SpriteAnimation GetCurrentAnimation() {
			return m_CurrentAnimation;
		}

		/// <summary>
		/// 指定のアニメーションを取得
		/// </summary>
		/// <param name="key">アニメーションキー</param>
		/// <returns>アニメーション</returns>
		SpriteAnimation GetAnimation(const wstring& key) {
			if (m_Animations.find(key) != m_Animations.end()) {
				return m_Animations[key];
			}
			return SpriteAnimation();
		}

		/// <summary>
		/// 再生するアニメーションを設定
		/// </summary>
		/// <param name="key">アニメーションキー</param>
		void SetCurrentAnimation(const wstring& key) {
			if (m_Animations.find(key) != m_Animations.end()) {
				m_CurrentAnimation.EndAnimation();
				m_BeforeAnimation = m_CurrentAnimation;
				m_CurrentAnimation = m_Animations[key];
			}
		}

		/// <summary>
		/// アニメーションを追加
		/// </summary>
		/// <param name="key">設定するキー</param>
		/// <param name="startOrder">描画開始</param>
		/// <param name="endOrder">描画終了</param>
		/// <param name="time">不明</param>
		/// <param name="interval">更新頻度</param>
		/// <param name="isLoop">ループするか</param>
		/// <param name="isReverse">逆再生か</param>
		void AddAnimation(const wstring& key, int startOrder, int endOrder, float time, float interval, const bool isLoop = false, const bool isReverse = false) {
			SpriteAnimation animation = SpriteAnimation(startOrder, endOrder, time, interval, isLoop, isReverse);
			m_Animations.insert(pair<wstring, SpriteAnimation>(key, animation));
		}

		/// <summary>
		/// アニメーションを追加
		/// </summary>
		/// <param name="key">設定するキー</param>
		/// <param name="order">描画順</param>
		/// <param name="time">不明</param>
		/// <param name="interval">更新頻度</param>
		/// <param name="isLoop">ループするか</param>
		/// <param name="isReverse">逆再生か</param>
		void AddAnimation(const wstring& key, vector<int> order, float time, float interval, const bool isLoop = false, const bool isReverse = false) {
			SpriteAnimation animation = SpriteAnimation(order, time, interval, isLoop, isReverse);
			m_Animations.insert(pair<wstring, SpriteAnimation>(key, animation));
		}

		/// <summary>
		/// アニメーション情報を更新
		/// </summary>
		/// <param name="key">更新するキー</param>
		/// <param name="newAnimation">新しいアニメーション</param>
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
		virtual ‾NumberSprite(){}

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
		virtual ‾SpriteAction() {}

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
		virtual ‾SpriteFlash(){}

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
		virtual ‾SpriteScaling() {}

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
		virtual ‾SpriteFade(){}

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
		virtual ‾SpriteButton(){}

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

		template<typename T>
		using group = map<wstring, T>;

		//ボタン格納用配列
		group<vector<shared_ptr<SpriteButton>>> m_ButtonGroup;
		//選択中の番号
		group<size_t> m_SelectIndexes;
		//反映させる移動量
		group<Vec3> m_GroupMovementAmount;
		//入力
		group<vector<InputData>> m_InputDates;
		group<vector<InputData>> m_KeyboradInputDates;
		
		//決定ボタン
		group<vector<WORD>> m_AcceptButtons;
		group<vector<WORD>> m_KeyboradAcceptButtons;
		//その瞬間に押された決定ボタン
		group<WORD> m_PressedAccept;

		wstring m_UsingGroup;
		//クリック音のキー
		wstring m_ClickSound;
		//選択音のキー
		wstring m_SelectSound;
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
		virtual ‾ButtonManager(){}

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

		template<typename T>
		inline bool FindGroup(map<wstring,T> vec,const wstring& group) {
			return vec.find(group) != end(vec);
		}

		shared_ptr<Sprite> GetButtonSprite(const wstring& group, int index) {
			if (FindGroup(m_ButtonGroup,group)) {
				auto vec = m_ButtonGroup[group];
				if (vec.size() > index) {
					auto obj = vec[index]->GetGameObject();
					return static_pointer_cast<Sprite>(obj);
				}
			}
			return nullptr;
		}

		bool PressSelect(const wstring& group,InputData& data) {
			auto keyborad = App::GetApp()->GetInputDevice().GetKeyState();

			if (FindGroup(m_KeyboradInputDates, group)) {
				for (auto& inputData : m_KeyboradInputDates[group]) {
					if (keyborad.m_bPressedKeyTbl[inputData.m_Input]) {
						if (abs(inputData.m_MoveAmount) > 1) {
							if (!CheckOverIndex(inputData.m_MoveAmount)) continue;
						}
						data = inputData;
						return true;
					}
				}
			}
			if (FindGroup(m_InputDates, group)) {
				for (auto& inputData : m_InputDates[group]) {
					if (CheckMoveInput(inputData)) {
						if (abs(inputData.m_MoveAmount) > 1) {
							if (!CheckOverIndex(inputData.m_MoveAmount)) continue;
						}
						data = inputData;
						return true;
					}
				}
			}
			return false;
		}
		bool PressAccept(const wstring& group,WORD& data) {
			auto& inputState = App::GetApp()->GetInputDevice().GetControlerVec()[0];
			auto keyborad = App::GetApp()->GetInputDevice().GetKeyState();
			if (FindGroup(m_KeyboradInputDates, group)) {
				for (auto& inputData : m_KeyboradAcceptButtons[group]) {
					if (keyborad.m_bPressedKeyTbl[inputData]) {
						data = inputData;
						return true;
					}
				}
			}
			if (FindGroup(m_InputDates, group)) {
				for (auto& inputData : m_AcceptButtons[group]) {
					if (inputState.wPressedButtons & inputData) {
						data = inputData;
						return true;
					}
				}
			}
			data = 0;
			return false;
		}
		size_t GetSelectIndex(const wstring& group) {
			if (FindGroup(m_ButtonGroup, group)) {
				return m_SelectIndexes[group];
			}
			return -1;
		}
		void SetSelectIndex(const wstring& group,int index) {
			if (group == L"") {
				if (index < m_ButtonGroup[m_UsingGroup].size()) {
					m_SelectIndexes[m_UsingGroup] = index;
				}
				return;
			}

			if (FindGroup(m_ButtonGroup, group)) {
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
		size_t GetSize(const wstring& group) {
			if (FindGroup(m_InputDates, group)) {
				return m_ButtonGroup[group].size();
			}
			else {
				return 0;
			}
		}
		void SetMoveAmount(const wstring& group, Vec3 target);

		Vec3 GetMoveAmount(const wstring& group) {
			if (FindGroup(m_GroupMovementAmount, group)) {
				return m_GroupMovementAmount[group];
			}
		}
		void SetSound(const wstring& sound);
		void SetSelectSound(const wstring& sound);

		void AddAcceptButton(const wstring& group, WORD accept) {
			if (FindGroup(m_AcceptButtons, group)) {
				m_AcceptButtons[group].push_back(accept);
			}
			else {
				vector<WORD> button = {};
				button.push_back(accept);
				m_AcceptButtons.emplace(group, button);
				m_PressedAccept.emplace(group, 0);
			}
		}
		void AddKeyboradAccept(const wstring& group, WORD accept) {
			if (FindGroup(m_KeyboradAcceptButtons, group)) {
				m_KeyboradAcceptButtons[group].push_back(accept);
			}
			else {
				vector<WORD> button = {};
				button.push_back(accept);
				m_KeyboradAcceptButtons.emplace(group, button);
				//m_PressedAccept.emplace(group, 0);
			}
		}
		WORD GetPressedAccept(const wstring& group) {
			if (FindGroup(m_PressedAccept, group)) {
				return m_PressedAccept[group];
			}
			return 0;
		}
		void SetInput(const wstring& group,int input,int amount) {
			SetInput(group, InputData(input, amount));
		}
		void SetInput(const wstring& group, InputData data) {
			if (FindGroup(m_InputDates, group)) {
				m_InputDates[group].push_back(data);
			}
			else {
				vector<InputData> dates = {};
				dates.push_back(data);
				m_InputDates.emplace(group, dates);
			}
		}
		void SetKeyborad(const wstring& group, int input, int amount) {
			SetKeyborad(group, InputData(input, amount));
		}
		
		void SetKeyborad(const wstring& group, InputData data) {
			if (FindGroup(m_KeyboradInputDates, group)) {
				m_KeyboradInputDates[group].push_back(data);
			}
			else {
				vector<InputData> dates = {};
				dates.push_back(data);
				m_KeyboradInputDates.emplace(group, dates);
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
			if (FindGroup(m_ButtonGroup, group)) {
				for (auto& button : m_ButtonGroup[group]) {
					button->Open();
				}
			}
			SetActive(true);
		}
		void Close(const wstring& group) {
			if (FindGroup(m_ButtonGroup, group)) {
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
			size_t maxIndex = m_ButtonGroup[m_UsingGroup].size() - 1;
			size_t minIndex = 0;
			m_SelectIndexes[m_UsingGroup] = max(minIndex, m_SelectIndexes[m_UsingGroup]);
			m_SelectIndexes[m_UsingGroup] = min(maxIndex, m_SelectIndexes[m_UsingGroup]);
		}
		bool CheckOverIndex(int amount) {
			size_t index = m_SelectIndexes[m_UsingGroup];
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
			if (FindGroup(m_SelectIndexes, group)) {
				InitGroup(m_UsingGroup);
				//m_SelectIndexes[m_UsingGroup] = 0;
				m_UsingGroup = group;
			}
		}
		bool CompareUseGroup(const wstring& group) {
			return m_UsingGroup == group;
		}
		void Register(const wstring& group, shared_ptr<SpriteButton>& button) {
			if (FindGroup(m_ButtonGroup, group)) {
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
			button->SetOrder(static_cast<int>(m_ButtonGroup[group].size()) - 1);
		}
		void DeleteGroup(const wstring& group) {
			if (FindGroup(m_ButtonGroup, group)) {
				m_ButtonGroup.erase(group);
				m_SelectIndexes.erase(group);
			}
		}

		void AddFrontSprite(const wstring& group,int index,shared_ptr<Sprite>& sprite) {
			if (FindGroup(m_ButtonGroup, group)) {
				if (m_ButtonGroup[group].size() > index) {
					m_ButtonGroup[group][index]->AddFrontSprite(sprite);
				}
			}
		}
		void AddFunction(const wstring& group,function<void(shared_ptr<SpriteButton>&)> func) {
			if (FindGroup(m_ButtonGroup, group)) {
				for (auto& buttons : m_ButtonGroup[group]) {
					buttons->AddFunction(func);
				}
			}
		}
		void AddFunction(const wstring& group,int index, function<void(shared_ptr<SpriteButton>&)> func) {
			if (FindGroup(m_ButtonGroup, group)) {
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
		virtual ‾Board() {}

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

	class SlideInSprite : public GameObject
	{
	public:

		bool m_pushState = false;
		shared_ptr<PCTSpriteDraw> m_DrawComp;
		vector<VertexPositionColorTexture> m_Vertices;
		shared_ptr<GameObject> m_selectCharge;
		shared_ptr<GameObject> m_titleCharge;
		float m_fade = 1.0f;
		float m_time;
		float m_maxtime;
		const float m_fadeSpeed = 2.0f;
		bool m_Trace;
		Vec2 m_StartScale;
		Vec3 m_StartPos;
		wstring m_TextureKey;
		float UVCharge;

		const float windowWidth = App::GetApp()->GetGameWidth();
		const float windowHeight = App::GetApp()->GetGameHeight();

		SlideInSprite(const shared_ptr<Stage>& StagePtr, const wstring& TextureKey, bool Trace,
			const Vec2& StartScale, const Vec3& StartPos, const float& Maxtime);
		virtual ‾SlideInSprite() {}
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		void ChargeUV(const float& time);
		void UpdateProgress(float time);
		void SetDiffuse(Col4 rgba);
	};
}
//end basecross
