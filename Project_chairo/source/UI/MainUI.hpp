#pragma once

#pragma warning(disable:4464)

#include "OptionWindow.hpp"
#include "../UI/PauseUI.hpp"

#include "../Draw/KeyGuide.hpp"

class MainUI {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsExit{ false };
	char		padding[5]{};
	Sound::SoundUniqueID			m_OKID{ InvalidID };
public:
	MainUI(void) noexcept {}
	MainUI(const MainUI&) = delete;
	MainUI(MainUI&&) = delete;
	MainUI& operator=(const MainUI&) = delete;
	MainUI& operator=(MainUI&&) = delete;
	virtual ~MainUI(void) noexcept {}
public:
	bool IsPauseActive(void) const noexcept { return m_IsPauseActive; }
	bool IsExit(void) const noexcept { return m_IsExit; }
public:
	void Init() noexcept {
		this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

		this->m_IsSceneEnd = false;
		this->m_IsPauseActive = false;
		this->m_IsExit = false;

		this->m_OptionWindow.Init();
		this->m_PauseUI.Init();

		this->m_PauseUI.SetEvent(0, [this]() {
			this->m_IsSceneEnd = true;
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});
		this->m_PauseUI.SetEvent(1, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_PauseUI.SetEvent(2, [this]() {
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});
	}
	void Update() noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
			this->m_IsPauseActive ^= 1;
			KeyGuideParts->SetGuideFlip();
		}
		this->m_PauseUI.SetActive(this->m_IsPauseActive && !this->m_OptionWindow.IsActive());
		if (!this->m_IsPauseActive) {
			this->m_OptionWindow.SetActive(false);
		}
		this->m_PauseUI.Update();
		if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
			this->m_IsExit = true;
		}
		this->m_OptionWindow.Update();
	}
	void Draw() noexcept {
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}
	void Dispose() noexcept {
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
