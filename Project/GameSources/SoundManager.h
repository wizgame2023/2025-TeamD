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

		SoundManager() : m_SEVolume(1.0f),m_BGMVolume(1.0f) {}
	public:
		
		virtual ~SoundManager(){}

		void RegisterSounds();
		void RegisterSound(const wstring& key, const wstring& fileName);
		void PlayLoopSE(const wstring& key,const float volume = 1.0f);
		void StopLoopSE(const wstring& key);

		static SoundManager& Instance();
		shared_ptr<SoundItem> PlaySE(const wstring& key,const float volume = 1.0f);
		shared_ptr<SoundItem> PlayBGM(const wstring& key, const float volume = 1.0f);
		void StopAll();
		void StopBGM();

		void SEVolumeDown(float volume) {
			m_SEVolume -= volume;
		}
		void SEVolumeUp(float volume) {
			m_SEVolume += volume;
		}
		void BGMVolumeDown(float volume) {
			m_BGMVolume -= volume;
		}
		void BGMVolumeUp(float volume) {
			m_BGMVolume += volume;
		}
	};

}
//end basecross
