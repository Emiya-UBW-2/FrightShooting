#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#include "../OptionWindow.hpp"
#include "../TitleUI.hpp"

#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Util.hpp"

#include "../Draw/MainDraw.hpp"

class TitleScene : public Util::SceneBase {
	OptionWindow	m_OptionWindow;
	TitleUI			m_TitleUI;
	EndUI			m_EndUI;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
		this->m_OptionWindow.Init();

		this->m_TitleUI.Init();
		this->m_TitleUI.SetEvent(0, [this]() {
			this->m_TitleUI.SetEnd();
			});
		this->m_TitleUI.SetEvent(1, []() {
			});
		this->m_TitleUI.SetEvent(2, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_TitleUI.SetEvent(3, []() {
			});

		this->m_EndUI.Init();
		this->m_EndUI.SetEvent([this]() {
			Util::SceneBase::SetEndGame();
			});
		this->m_EndUI.SetActive(false);

		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->SetGuideFlip();
	}
	void Update_Sub(void) noexcept override {
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->ChangeGuide(
			[]() {
				auto* Localize = Util::LocalizePool::Instance();
				auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Esc), Localize->Get(340));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::UP), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::DOWN), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::LEFT), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::RIGHT), Localize->Get(332));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Diside), Localize->Get(330));
			}
		);
		DxLib::SetMouseDispFlag(true);
		this->m_EndUI.Update();
		if (this->m_EndUI.IsActive()) { return; }
		this->m_TitleUI.SetActive(!this->m_OptionWindow.IsActive());
		this->m_TitleUI.Update();
		if (this->m_TitleUI.IsEnd()) {
			this->m_OptionWindow.SetActive(false);
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		if (this->m_TitleUI.IsActive() || this->m_OptionWindow.IsActive()) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Esc)) {
				//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
				this->m_EndUI.SetActive(true);
			}
		}
		this->m_OptionWindow.Update();
	}
	void BGDraw_Sub(void) noexcept override {}
	void SetShadowDrawRigid_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) noexcept override {}
	void DrawFront_Sub(void) noexcept override {}
	void DepthDraw_Sub(void) noexcept override {}
	void ShadowDrawFar_Sub(void) noexcept override {}
	void ShadowDraw_Sub(void) noexcept override {}
	void UIDraw_Sub(void) noexcept override {
		this->m_TitleUI.Draw();
		this->m_OptionWindow.Draw();
		this->m_EndUI.Draw();
	}
	void Dispose_Sub(void) noexcept override {
		this->m_TitleUI.Dispose();
		this->m_OptionWindow.Dispose();
		this->m_EndUI.Dispose();
	}
};
