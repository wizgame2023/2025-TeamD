/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class HP : public Component {
		float m_Hp;		// 現在のHP
		float m_MaxHp;	// 最大HP

		function<void()> m_DeadCallBack;	// 死亡時に呼ばれるコールバック
		function<void(float)> m_DamageCallBack;	// ダメージ時に呼ばれるコールバック(引数はダメージ量)
		function<void(float)> m_HealCallBack;	// 回復時に呼ばれるコールバック(引数は回復量)


	public:
		HP(const shared_ptr<GameObject>& ptr, float hp = 1) : Component(ptr), m_Hp(hp), m_MaxHp(hp){}
		virtual ‾HP() {}

		/// <summary>
		/// 最大HPを取得
		/// </summary>
		/// <returns>現在の最大HP</returns>
		float GetMaxHP() const{
			return m_MaxHp;
		}
		/// <summary>
		/// 最大HPを設定(減っているHP量は変化しない)
		/// </summary>
		/// <param name="maxHp">設定する最大値</param>
		void SetMaxHp(float maxHp);

		/// <summary>
		/// 残りHPを取得
		/// </summary>
		/// <returns>現在の残りHP</returns>
		float GetHp() const{
			return m_Hp;
		}

		/// <summary>
		/// 残りHPの割合を取得
		/// </summary>
		/// <returns>HP割合　少数で返される(0.0〜1.0)</returns>
		float GetHpPercent()const{
			return m_Hp / m_MaxHp;
		}
		/// <summary>
		/// 残りHPを初期化
		/// </summary>
		void InitHp() {
			m_Hp = m_MaxHp;
		}

		/// <summary>
		/// HPを指定した量減らす。最低値は0
		/// </summary>
		/// <param name="amount">減らすHP量</param>
		void Damage(float amount);

		/// <summary>
		/// HPを指定した量回復。最大値は最大HP
		/// </summary>
		/// <param name="amount">回復するHP量</param>
		void Heal(float amount);

		/// <summary>
		/// 死亡状態かを判定
		/// </summary>
		/// <returns>死亡判定</returns>
		bool IsDead() const{
			return m_Hp == 0;
		}

		/// <summary>
		/// 死亡したときに呼ばれるコールバックを設定
		/// </summary>
		/// <param name="callBack">コールバック(ラムダ式やstd::function)</param>
		void SetDeadCallBack(const function<void()>& callBack) {
			m_DeadCallBack = callBack;
		}

		/// <summary>
		/// 回復したときに呼ばれるコールバックを設定
		/// </summary>
		/// <param name="callBack">コールバック(ラムダ式やstd::function)</param>
		void SetHealCallBack(const function<void()>& callBack) {
			m_DeadCallBack = callBack;
		}

		/// <summary>
		/// ダメージを食らったときに呼ばれるコールバックを設定
		/// </summary>
		/// <param name="callBack">コールバック(ラムダ式やstd::function)</param>
		void SetDamageCallBack(const function<void()>& callBack) {
			m_DeadCallBack = callBack;
		}
	};

}

//end basecross
