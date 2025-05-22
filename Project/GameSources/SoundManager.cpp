/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	SoundManager& SoundManager::Instance() {
		static SoundManager instance;

		return instance;
	}
	void SoundManager::RegisterSounds() {
		m_Audio = App::GetApp()->GetXAudio2Manager();
		//BGM
		RegisterSound(L"BGM_GAME_BOSS", L"BGM/Boss.wav");
		RegisterSound(L"BGM_GAME_PINCH", L"BGM/Pinch.wav");
		RegisterSound(L"BGM_TITLE", L"BGM/Title.wav");
		RegisterSound(L"BGM_SELECT", L"BGM/SelectBGM.wav");
		//SE
		RegisterSound(L"SE_GUARD", L"SE/Guard.wav");
		RegisterSound(L"SE_RUN", L"SE/Run_2.wav");
		RegisterSound(L"SE_HIT_PLAYER", L"SE/PlayerHit_3.wav");
		RegisterSound(L"SE_HIT_ENEMY", L"SE/EnemyHit_2.wav");
		RegisterSound(L"SE_SHOT", L"SE/RifleShot_3.wav");
		RegisterSound(L"SE_USE_ULT", L"SE/UltUse_3.wav");
		RegisterSound(L"SE_WALK_ENEMY", L"SE/EnemyWalk.wav");
		RegisterSound(L"SE_WALK_PLAYER", L"SE/PlayerWalk.wav");
		RegisterSound(L"SE_ATTACK_VOICE", L"SE/AttackVoice_3.wav");
		RegisterSound(L"SE_ACCEPT", L"tuika/Determinant.wav");

		RegisterSound(L"SE_EXPLODE", L"tuika/Determinant.wav");
		RegisterSound(L"SE_MISSILE", L"tuika/Rocket_Fire.wav");
		RegisterSound(L"SE_CRUSH", L"tuika/Boss_Attack_Spanking_2.wav");
		RegisterSound(L"SE_WAVE", L"tuika/Wave_Cahnge_3.wav");
	}
	void SoundManager::RegisterSound(const wstring& key, const wstring& fileName) {
		wstring path = App::GetApp()->GetDataDirWString() + L"Sounds/";

		auto audioRes = App::GetApp()->RegisterWav(key, path + fileName);
		m_SoundKeys.push_back(key);
	}
	void SoundManager::PlayLoopSE(const wstring& key, const float volume) {
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			auto se = m_Audio->Start(key, XAUDIO2_LOOP_INFINITE, volume * m_SEVolume);
			m_PlayingSE.insert(pair<wstring, shared_ptr<SoundItem>>(key, se));
		}
	}
	void SoundManager::StopLoopSE(const wstring& key) {
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			if (m_PlayingSE.find(key) != m_PlayingSE.end()) {
				m_Audio->Stop(m_PlayingSE[key]);
				m_PlayingSE.erase(key);
			}
		}
	}
	shared_ptr<SoundItem> SoundManager::PlaySE(const wstring& key, const float volume) {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			auto se = m_Audio->Start(key, 0, volume * m_SEVolume);
			return se;
		}
		else {
			int result = MessageBox(NULL, L"Key Not Found. key : ", L"ERROR", MB_OK);
		}
		return nullptr;
	}
	shared_ptr<SoundItem> SoundManager::PlayBGM(const wstring& key, const float volume) {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			if (m_Bgm != nullptr) {
				StopBGM();
			}
			m_Bgm = m_Audio->Start(key, XAUDIO2_LOOP_INFINITE, volume * m_BGMVolume);
		}
		else {
			int result = MessageBox(NULL, L"Key Not Found. key : ", L"ERROR", MB_OK);
		}
		return m_Bgm;
	}
	void SoundManager::SetBGMVolume() {
		if (m_Bgm != nullptr) {
			m_Bgm->m_SourceVoice->SetVolume(m_BGMVolume);
		}
	}
	void SoundManager::PauseBGM(bool flag) {
		if (m_Bgm != nullptr) {
			m_Bgm->m_SourceVoice->Stop(XAUDIO2_PLAY_TAILS);
		
			if(!flag){
				m_Bgm->m_SourceVoice->Start();
			}
		}
	}
	void SoundManager::StopBGM() {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		if (m_Bgm != nullptr) {
			m_Audio->Stop(m_Bgm);
			m_Bgm = nullptr;
		}
	}
	void SoundManager::StopAll() {
		StopBGM();
		for (auto se : m_PlayingSE) {
			m_Audio->Stop(se.second);
		}
		m_PlayingSE.clear();
	}

}
