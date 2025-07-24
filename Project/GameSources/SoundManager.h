/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	class SoundManager{
		vector<wstring> m_SoundKeys;
		shared_ptr<XAudio2Manager> m_Audio;
		shared_ptr<SoundItem> m_Bgm;
		
		map<wstring, shared_ptr<SoundItem>> m_PlayingSE;

		float m_SEVolume;
		float m_BGMVolume;

		void SetBGMVolume();

		SoundManager() : m_SEVolume(1.0f),m_BGMVolume(1.0f) {}
	public:
		
		virtual ‾SoundManager(){}

		void RegisterSounds();
		void RegisterSound(const wstring& key, const wstring& fileName);
		void PlayLoopSE(const wstring& key,const float volume = 1.0f);
		void StopLoopSE(const wstring& key);

		static SoundManager& Instance();
		shared_ptr<SoundItem> PlaySE(const wstring& key,const float volume = 5.0f);
		shared_ptr<SoundItem> PlayBGM(const wstring& key, const float volume = 1.0f);
		void StopAll();
		void StopBGM();
		void PauseBGM(bool flag);

		float GetSEVolume() {
			return m_SEVolume;
		}
		float GetBGMVolume() {
			return m_BGMVolume;
		}
		void SEVolumeDown(float volume) {
			m_SEVolume -= volume;
			m_SEVolume = max(m_SEVolume, 0.0f);
		}
		void SEVolumeUp(float volume) {
			m_SEVolume += volume;
			m_SEVolume = min(m_SEVolume, 1.0f);
		}
		void BGMVolumeDown(float volume) {
			m_BGMVolume -= volume;
			m_BGMVolume = max(m_BGMVolume, 0);
			SetBGMVolume();
		}
		void BGMVolumeUp(float volume) {
			m_BGMVolume += volume;
			m_BGMVolume = min(m_BGMVolume, 1.0f);
			SetBGMVolume();
		}
	};

}
//end basecross
