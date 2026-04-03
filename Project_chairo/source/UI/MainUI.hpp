#pragma once

#pragma warning(disable:4464)

#include "OptionWindow.hpp"
#include "../UI/PauseUI.hpp"
#include "../Draw/KeyGuide.hpp"
#include "../MainScene/GameRule.hpp"

class MainUI {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsExit{ false };
	char		padding[5]{};
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	float							m_HPPrev{};
	float							m_HPChangeTime{};
	float							m_HPRe{};
	float							m_HPRe2{};
	float							m_Timer{};
	char		padding2[4]{};
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

		auto& Watch = PlayerManager::Instance()->SetPlane();

		m_HPPrev = Watch->GetHitPointPer();
		m_HPChangeTime = 0.f;
		m_HPRe = Watch->GetHitPointPer();
	}
	void Update() noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		auto* DrawerMngr = Draw::MainDraw::Instance();
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

		auto& Watch = PlayerManager::Instance()->SetPlane();
		if (m_HPPrev != Watch->GetHitPointPer()) {
			m_HPPrev = Watch->GetHitPointPer();
			m_HPRe = m_HPRe2;
			m_HPChangeTime = 2.f;
		}

		m_HPChangeTime = std::max(m_HPChangeTime - DrawerMngr->GetDeltaTime(), 0.f);
		if (m_HPChangeTime <= 1.f) {
			m_HPRe2 = Util::Lerp(Watch->GetHitPointPer(), m_HPRe, m_HPChangeTime);
		}
		else {
			m_HPRe2 = m_HPRe;
		}

		m_Timer += DrawerMngr->GetDeltaTime();
	}
	void Draw() noexcept {

		auto& Watch = PlayerManager::Instance()->SetPlane();
		{
			int XP = 64, YP = 1080 - 92, XS = 400, YS = 32;
			int R = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetHitPointPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetHitPointPer())), 0, 255);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Black, true);
			DrawBox(XP, YP, XP + static_cast<int>(static_cast<float>(XS) * m_HPRe2), YP + YS, ColorPalette::Red, true);
			DrawBox(XP, YP, XP + static_cast<int>(static_cast<float>(XS) * Watch->GetHitPointPer()), YP + YS, GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Gray30, false, 3);
		}

		{
			int XP = 1920 - 64, YP = 1080 / 2, XS = 32, YS = 400;
			int R = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetStallPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetStallPer())), 0, 255);
			DrawBox(XP, YP, XP + XS, YP + static_cast<int>(static_cast<float>(YS) * Watch->GetStallPer()), Watch->IsStall() ? ColorPalette::Red : GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Green, false, 3);
		}
		
		{
			int XP = 1920 - 64, YP = 1080 / 2 - 400, XS = 32, YS = 400;
			int R = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetBoostPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetBoostPer())), 0, 255);
			DrawBox(XP, YP + YS - static_cast<int>(static_cast<float>(YS) * Watch->GetBoostPer()), XP + XS, YP + YS, Watch->IsOverHeat() ? ColorPalette::Red : GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Green, false, 3);
		}

		{
			int YP = 0;
			if (Watch->IsStall() && (static_cast<int>(m_Timer * 10.f) % 10 < 5)) {
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::MIDDLE,
					1920 / 2, 1080 / 2 - 400 + YP,
					ColorPalette::Red, ColorPalette::Red50,
					"STALL");
				YP += 32;
			}

			if (Watch->IsOverHeat() && (static_cast<int>(m_Timer * 10.f) % 10 < 5)) {
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::MIDDLE,
					1920 / 2, 1080 / 2 - 400 + YP,
					ColorPalette::Red, ColorPalette::Red50,
					"OVER HEAT");
				YP += 32;
			}
		}

		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}
	void Dispose() noexcept {
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
