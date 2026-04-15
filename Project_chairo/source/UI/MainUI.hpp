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
	bool							m_IsAlert{ false };
	char		padding[4]{};
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	float							m_HPPrev{};
	float							m_HPChangeTime{};
	float							m_HPRe{};
	float							m_HPRe2{};
	float							m_Timer{};
	float							m_AltRand{};
	float							m_SpdRand{};
	char		padding2[4]{};
public:
	MainUI(void) noexcept {}
	MainUI(const MainUI&) = delete;
	MainUI(MainUI&&) = delete;
	MainUI& operator=(const MainUI&) = delete;
	MainUI& operator=(MainUI&&) = delete;
	virtual ~MainUI(void) noexcept {}
public:
	bool IsPauseActive(void) const noexcept { return this->m_IsPauseActive; }
	bool IsExit(void) const noexcept { return this->m_IsExit; }
	void SetIsAlert(bool IsAlert) noexcept { this->m_IsAlert = IsAlert; }
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

		this->m_HPPrev = Watch->GetHitPointPer();
		this->m_HPChangeTime = 0.f;
		this->m_HPRe = Watch->GetHitPointPer();
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
		if (this->m_HPPrev != Watch->GetHitPointPer()) {
			this->m_HPPrev = Watch->GetHitPointPer();
			this->m_HPRe = this->m_HPRe2;
			this->m_HPChangeTime = 2.f;
		}

		this->m_HPChangeTime = std::max(this->m_HPChangeTime - DrawerMngr->GetDeltaTime(), 0.f);
		if (this->m_HPChangeTime <= 1.f) {
			this->m_HPRe2 = Util::Lerp(Watch->GetHitPointPer(), this->m_HPRe, this->m_HPChangeTime);
		}
		else {
			this->m_HPRe2 = this->m_HPRe;
		}

		this->m_Timer += DrawerMngr->GetDeltaTime();

		Util::Easing(&m_AltRand, GetRandf(2.f), 0.9f);
		Util::Easing(&m_SpdRand, GetRandf(2.f), 0.9f);
	}
	void Draw() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();

		auto& Watch = PlayerManager::Instance()->SetPlane();
		{
			int XP = 64, YP = DrawerMngr->GetDispHeight() - 92, XS = 400, YS = 32;
			int R = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetHitPointPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetHitPointPer())), 0, 255);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Black, true);
			DrawBox(XP, YP, XP + static_cast<int>(static_cast<float>(XS) * this->m_HPRe2), YP + YS, ColorPalette::Red, true);
			DrawBox(XP, YP, XP + static_cast<int>(static_cast<float>(XS) * Watch->GetHitPointPer()), YP + YS, GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Gray30, false, 3);
		}

		{
			int XP = DrawerMngr->GetDispWidth() - 64, YP = DrawerMngr->GetDispHeight() / 2, XS = 32, YS = 400;
			int R = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetStallPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetStallPer())), 0, 255);
			DrawBox(XP, YP, XP + XS, YP + static_cast<int>(static_cast<float>(YS) * Watch->GetStallPer()), Watch->IsStall() ? ColorPalette::Red : GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Green, false, 3);
		}
		{
			int XP = DrawerMngr->GetDispWidth() - 64, YP = DrawerMngr->GetDispHeight() / 2 - 400, XS = 32, YS = 400;
			int R = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, Watch->GetBoostPer())), 0, 255);
			int G = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, Watch->GetBoostPer())), 0, 255);
			DrawBox(XP, YP + YS - static_cast<int>(static_cast<float>(YS) * Watch->GetBoostPer()), XP + XS, YP + YS, Watch->IsOverHeat() ? ColorPalette::Red : GetColor(R, G, 0), true);
			DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Green, false, 3);
		}

		{
			auto* CameraParts = Camera::Camera3D::Instance();
			int XP = DrawerMngr->GetDispWidth() / 2 + static_cast<int>(CameraParts->GetShake().x * -10.f);
			int YP = DrawerMngr->GetDispHeight() / 2 + static_cast<int>(CameraParts->GetShake().y * -10.f);
			float Rad = Watch->GetRollPer() * -1.f;
			{
				float Alt = Watch->GetMat().pos().y / Scale3DRate + this->m_AltRand;
				int Range = 300;
				for (int loop = -Range / 10; loop < Range / 10; ++loop) {
					int XP1 = -300 - 16;
					int YP1 = loop * (Range / 30) + static_cast<int>(Alt * 10) % (Range / 30);
					int XP2 = -300;
					int YP2 = loop * (Range / 30) + static_cast<int>(Alt * 10) % (Range / 30);
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);
				}
				for (int loop = -Range / 50; loop < Range / 50; ++loop) {
					int XP1 = -300 - 32;
					int YP1 = loop * (Range / 6) + static_cast<int>(Alt * 10) % (Range / 6);
					int XP2 = -300;
					int YP2 = loop * (Range / 6) + static_cast<int>(Alt * 10) % (Range / 6);
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);
				}
				{
					int XP1 = -300 - 48;
					int YP1 = 0;
					int XP2 = -300 - 36;
					int YP2 = 0;
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);

					auto* Font = Draw::FontPool::Instance();
					Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
						Draw::FontXCenter::RIGHT, Draw::FontYCenter::MIDDLE,
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						ColorPalette::Green, ColorPalette::DarkGreen,
						"ALT %03d ", static_cast<int>(Alt));
				}
			}
			{
				float Alt = Watch->GetSpeed() / Watch->GetSpeedMax() * 100.f + this->m_SpdRand;
				int Range = 300;
				for (int loop = -Range / 10; loop < Range / 10; ++loop) {
					int XP1 = 300;
					int YP1 = loop * (Range / 30) + static_cast<int>(Alt * 10) % (Range / 30);
					int XP2 = 300 + 16;
					int YP2 = loop * (Range / 30) + static_cast<int>(Alt * 10) % (Range / 30);
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);
				}
				for (int loop = -Range / 50; loop < Range / 50; ++loop) {
					int XP1 = 300;
					int YP1 = loop * (Range / 6) + static_cast<int>(Alt * 10) % (Range / 6);
					int XP2 = 300 + 32;
					int YP2 = loop * (Range / 6) + static_cast<int>(Alt * 10) % (Range / 6);
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);
				}
				{
					int XP1 = 300 + 48;
					int YP1 = 0;
					int XP2 = 300 + 36;
					int YP2 = 0;
					DrawLine(
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP2) + sin(Rad) * static_cast<float>(YP2)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP2) - sin(Rad) * static_cast<float>(XP2)),
						ColorPalette::Green, 2);

					auto* Font = Draw::FontPool::Instance();
					Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
						Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
						XP + static_cast<int>(cos(Rad) * static_cast<float>(XP1) + sin(Rad) * static_cast<float>(YP1)), YP + static_cast<int>(cos(Rad) * static_cast<float>(YP1) - sin(Rad) * static_cast<float>(XP1)),
						ColorPalette::Green, ColorPalette::DarkGreen,
						" %03d SPD", static_cast<int>(Alt));
				}
			}
		}

		{
			int YP = 0;
			if (Watch->IsStall() && (static_cast<int>(this->m_Timer * 10.f) % 10 < 5)) {
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::MIDDLE,
					DrawerMngr->GetDispWidth() / 2, DrawerMngr->GetDispHeight() / 2 - 400 + YP,
					ColorPalette::Red, ColorPalette::Red50,
					"STALL");
				YP += 32;
			}

			if (Watch->IsOverHeat() && (static_cast<int>(this->m_Timer * 10.f) % 10 < 5)) {
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::MIDDLE,
					DrawerMngr->GetDispWidth() / 2, DrawerMngr->GetDispHeight() / 2 - 400 + YP,
					ColorPalette::Red, ColorPalette::Red50,
					"OVER HEAT");
				YP += 32;
			}

			if (this->m_IsAlert) {
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::MIDDLE,
					DrawerMngr->GetDispWidth() / 2, DrawerMngr->GetDispHeight() / 2 - 400 + YP,
					ColorPalette::Red, ColorPalette::Red50,
					"MISSILE");
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
