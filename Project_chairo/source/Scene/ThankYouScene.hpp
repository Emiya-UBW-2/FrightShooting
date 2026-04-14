#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#include "../Util/SceneManager.hpp"
#include "../Util/Localize.hpp"

#include "../MainScene/GameRule.hpp"

class ThankYouScene : public Util::SceneBase {
	float m_Time{};
	float m_Fade{};
	bool m_Exit{};
	char		padding2[7]{};

	Sound::SoundUniqueID			m_BGMID{ InvalidID };
public:
	ThankYouScene(void) noexcept { SetID(static_cast<int>(EnumScene::ThankYou)); }
	ThankYouScene(const ThankYouScene&) = delete;
	ThankYouScene(ThankYouScene&&) = delete;
	ThankYouScene& operator=(const ThankYouScene&) = delete;
	ThankYouScene& operator=(ThankYouScene&&) = delete;
	virtual ~ThankYouScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override {
		this->m_BGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, "data/Sound/BGM/thankyou.wav", false);
	}
	void Init_Sub(void) noexcept override {
		m_Time = 0.f;
		this->m_Fade = 1.f;
		this->m_Exit = false;
		Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_BGMID)->Play(DX_PLAYTYPE_BACK, TRUE);
	}
	void Update_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->ChangeGuide(
			[]() {
				auto* Localize = Util::LocalizePool::Instance();
				auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Esc), Localize->Get(350));
			}
		);
		DxLib::SetMouseDispFlag(true);

		m_Time += DrawerMngr->GetDeltaTime();
		if (m_Time > 23.f) {
			this->m_Exit = true;
		}

		this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DrawerMngr->GetDeltaTime(), 0.f, 2.f);
		if (m_Exit) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_BGMID)->SetLocalVolume(static_cast<int>(Util::Lerp(255.f, 0.f, std::clamp(this->m_Fade, 0.f, 1.f))));
			if (this->m_Fade >= 2.f) {
				SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
				SceneBase::SetEndScene();
			}
		}
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
		auto* DrawerMngr = Draw::MainDraw::Instance();

		DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Gray50, TRUE);

		if (this->m_Fade > 0.f) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_Fade));
			DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_BGMID)->StopAll();

		GameRule::Instance()->SetNextStage("Stage0101");
		GameRule::Instance()->SetIsStartEvent(false);
		GameRule::Instance()->SetHP(100);
	}
};
