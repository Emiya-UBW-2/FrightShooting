#pragma warning(disable:5259)
#include "Sound.hpp"
#include "Option.hpp"

// シングルトンの実態定義
const Sound::SoundPool* Util::SingletonBase<Sound::SoundPool>::m_Singleton = nullptr;

namespace Sound {
	// 保持している音声すべての音量を設定
	void Soundhave::FlipVolume(void) noexcept {
		//BGMかSEかに合わせて取得するボリュームを変更
		auto* pOption = Util::OptionParam::Instance();
		float Vol = 1.0f;
		switch (this->m_SoundType) {
		case SoundType::VOICE:		//Vol = OptionParts->GetParamFloat(EnumSaveParam::VOICE);			break;
			break;
		case SoundType::SE:
			Vol = static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::SEVolume))->GetSelect()) / 100.f;
			Vol *= static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::MasterVolume))->GetSelect()) / 100.f;
			break;
		case SoundType::BGM:
			Vol = static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::BGMVolume))->GetSelect()) / 100.f;
			Vol *= static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::MasterVolume))->GetSelect()) / 100.f;
			break;
		default:
			break;
		}
		this->m_Handles->SetVolAll(static_cast<int>(Vol * static_cast<float>(this->m_LocalVolume)));
	}
};
