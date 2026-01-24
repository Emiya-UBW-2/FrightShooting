#include "OptionWindow.hpp"
#include "Util/Enum.hpp"
#include "Util/Sound.hpp"
#include "Draw/PostPass.hpp"

void OptionWindow::SetActive(bool value) noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	bool IsChange = (value != IsActive());
	this->m_DrawUI->Get(this->m_UIBase).SetActive(value);
	if (IsChange) {
		if (IsActive()) {
			this->m_LanguagePrev = pOption->GetParam(pOption->GetOptionType(Util::OptionType::Language))->GetSelect();
			SetTab();
		}
		else {
			EndTab();
			for (int loop = 0; loop < static_cast<int>(Util::InputType::Max); ++loop) {
				KeyMngr->Save(static_cast<Util::InputType>(loop));
			}
			pOption->Save();
		}
		this->m_NowSelectTab = 0;
	}
}

void OptionWindow::UpdateColumnStr(void) noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	switch (this->m_NowSelectTab) {
	case 0:
	{
		for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
			auto& param = this->m_Param[loop];
			auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::MasterVolume)));
			param.m_Str = pOption->GetParam(Type)->GetValueNow();
			if (param.m_MinID != InvalidID) {
				this->m_DrawUI->Get(param.m_MinID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMin());
			}
			if (param.m_MaxID != InvalidID) {
				this->m_DrawUI->Get(param.m_MaxID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMax());
			}
		}
	}
	break;
	case 1:
	{
		for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
			auto& param = this->m_Param[loop];
			auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::WindowMode)));
			param.m_Str = pOption->GetParam(Type)->GetValueNow();
			if (param.m_MinID != InvalidID) {
				this->m_DrawUI->Get(param.m_MinID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMin());
			}
			if (param.m_MaxID != InvalidID) {
				this->m_DrawUI->Get(param.m_MaxID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMax());
			}
		}
	}
	break;
	case 2:
	{
		for (int loop = 0; loop < 2; ++loop) {
			auto& param = this->m_Param[loop];
			auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::XSensing)));
			param.m_Str = pOption->GetParam(Type)->GetValueNow();
			if (param.m_MinID != InvalidID) {
				this->m_DrawUI->Get(param.m_MinID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMin());
			}
			if (param.m_MaxID != InvalidID) {
				this->m_DrawUI->Get(param.m_MaxID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMax());
			}
		}
		for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
			auto& param = this->m_Param[loop];
			param.m_Str = Util::KeyParam::GetKeyStr(KeyMngr->GetKeyAssign(static_cast<Util::EnumBattle>(loop - 2), 0));
			this->m_DrawUI->Get(param.m_MaxID).SetActive(true);
		}
	}
	break;
	case 3:
	{
		for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
			auto& param = this->m_Param[loop];
			auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::HeadBobbing)));
			param.m_Str = pOption->GetParam(Type)->GetValueNow();
			if (param.m_MinID != InvalidID) {
				this->m_DrawUI->Get(param.m_MinID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMin());
			}
			if (param.m_MaxID != InvalidID) {
				this->m_DrawUI->Get(param.m_MaxID).SetActive(pOption->GetParam(Type)->GetSelect() != pOption->GetParam(Type)->GetSelectMax());
			}
		}
	}
	break;
	default:
		break;
	}
	for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
		auto& param = this->m_Param[loop];
		this->m_DrawUI->Get(param.m_ID).GetParts("String0")->SetString(param.m_Str);
	}
}
void OptionWindow::SetTab(void) noexcept {
	std::string ChildBase = "OptionUI/Child";
	ChildBase += std::to_string(this->m_NowSelectTab + 1);
	Draw::Param2D Param;
	Param.OfsNoRad = Util::VECTOR2D::vget(960, 205);
	{
		std::string Path = "data/UI/Option/Tab";
		Path += std::to_string(this->m_NowSelectTab + 1);
		Path += ".json";
		this->m_DrawUI->AddChild(ChildBase.c_str(), Path.c_str(), Param);
	}
	this->m_NowTabMax = this->m_ParamMax;
	for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
		auto& param = this->m_Param[loop];
		std::string Child = ChildBase;
		Child += "/Column";
		Child += std::to_string(loop + 1);
		param.m_ID = this->m_DrawUI->GetID(Child.c_str());
		if (param.m_ID == InvalidID) {
			this->m_NowTabMax = loop;
			break;
		}
		param.m_MinID = this->m_DrawUI->GetID((Child + "/Box1").c_str());
		param.m_MaxID = this->m_DrawUI->GetID((Child + "/Box0").c_str());
	}
	UpdateColumnStr();
	this->m_DrawUI->Get(this->m_TabButton[this->m_NowSelectTab]).SetActive(true);
}
void OptionWindow::EndTab(void) noexcept {
	std::string Path = "OptionUI/Child";
	Path += std::to_string(this->m_NowSelectTab + 1);
	this->m_DrawUI->DeleteChild(Path.c_str());
	this->m_DrawUI->Get(this->m_TabButton[this->m_NowSelectTab]).SetActive(false);
	this->m_NowTabMax = 0;
}
void OptionWindow::Init(void) noexcept {
	cancelID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cancel.wav", false);
	cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
	NGID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ng.wav", false);
	OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

	this->m_DrawUI = new Draw::DrawUISystem();

	this->m_DrawUI->Init("data/UI/Option/OptionBase.json");

	this->m_UIBase = this->m_DrawUI->GetID("");
	for (int loop = 0; loop < 4; ++loop) {
		std::string Path = "OptionUI/Tab";
		Path += std::to_string(loop + 1);
		this->m_TabButton[loop] = this->m_DrawUI->GetID(Path.c_str());
		this->m_DrawUI->Get(this->m_TabButton[loop]).SetActive(false);
	}
	this->m_NowSelectTab = 0;
	this->m_CloseButton = this->m_DrawUI->GetID("OptionUI/CloseButton");
	SetActive(false);
}
void OptionWindow::Update(void) noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	if (this->m_DrawUI->Get(this->m_UIBase).IsActive()) {
		{
			int IsSelect = InvalidID;
			for (int loop = 0; loop < 4; ++loop) {
				if (this->m_DrawUI->Get(this->m_TabButton[loop]).IsSelectButton()) {
					IsSelect = loop;
					break;
				}
			}
			if (this->m_DrawUI->Get(this->m_CloseButton).IsSelectButton()) {
				IsSelect = 4;
			}
			if (IsSelect != InvalidID && (IsSelect != isSelectTabSoundPrev)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			isSelectTabSoundPrev = IsSelect;
		}
		for (int loop = 0; loop < 4; ++loop) {
			if (this->m_DrawUI->Get(this->m_TabButton[loop]).IsSelectButton() && KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Diside)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			if (this->m_DrawUI->Get(this->m_TabButton[loop]).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				if (this->m_NowSelectTab != loop) {
					EndTab();
					this->m_NowSelectTab = loop;
					SetTab();
				}
			}
		}

		if (this->m_DrawUI->Get(this->m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Diside)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cancelID)->Play(DX_PLAYTYPE_BACK, TRUE);
		}
		if (this->m_DrawUI->Get(this->m_CloseButton).IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
			SetActive(false);
		}

		{
			int IsSelect = InvalidID;
			for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
				auto& param = this->m_Param[loop];
				if (this->m_DrawUI->Get(param.m_ID).IsSelectButton()) {
					IsSelect = loop;
					break;
				}
			}
			if (IsSelect != InvalidID && (IsSelect != isSelectSoundPrev)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			isSelectSoundPrev = IsSelect;
		}

		if (KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::Diside)) {
			for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
				auto& param = this->m_Param[loop];
				if (param.m_MinID != InvalidID) {
					if (this->m_DrawUI->Get(param.m_MinID).IsActive() && this->m_DrawUI->Get(param.m_MinID).IsSelectButton()) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				}
				if (param.m_MaxID != InvalidID) {
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}

			switch (this->m_NowSelectTab) {
			case 0:
			{
				for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
					auto& param = this->m_Param[loop];
					auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::MasterVolume)));
					if (this->m_DrawUI->Get(param.m_MinID).IsActive() && this->m_DrawUI->Get(param.m_MinID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() - 1);
						Sound::SoundPool::Instance()->FlipVolume();
					}
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() + 1);
						Sound::SoundPool::Instance()->FlipVolume();
					}
				}
			}
			break;
			case 1:
			{
				auto* DrawerMngr = Draw::MainDraw::Instance();
				auto* PostPassParts = Draw::PostPassEffect::Instance();
				for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
					auto& param = this->m_Param[loop];
					Util::OptionType OptType = static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::WindowMode));
					auto Type = pOption->GetOptionType(OptType);
					bool IsChange = false;
					if (this->m_DrawUI->Get(param.m_MinID).IsActive() && this->m_DrawUI->Get(param.m_MinID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() - 1);
						IsChange = true;
					}
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() + 1);
						IsChange = true;
					}
					if (IsChange) {
						if (OptType == Util::OptionType::GraphicsPreset) {
							switch (pOption->GetParam(Type)->GetSelect()) {
							case 0:
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::DoF), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Bloom), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Shadow), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::SSAO), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect), 0);
								//pOption->SetParam(pOption->GetOptionType(Util::OptionType::Reflection), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::AntiAliasing), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::GodRay), 0);
								break;
							case 1:
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::DoF), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Bloom), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Shadow), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::SSAO), 0);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect), 1);
								//pOption->SetParam(pOption->GetOptionType(Util::OptionType::Reflection), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::AntiAliasing), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::GodRay), 1);
								break;
							case 2:
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel), 2);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::DoF), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Bloom), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Shadow), 2);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::SSAO), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect), 1);
								//pOption->SetParam(pOption->GetOptionType(Util::OptionType::Reflection), 2);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::AntiAliasing), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::GodRay), 1);
								break;
							case 3:
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel), 2);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::DoF), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Bloom), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::Shadow), 3);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::SSAO), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect), 1);
								//pOption->SetParam(pOption->GetOptionType(Util::OptionType::Reflection), 3);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::AntiAliasing), 1);
								pOption->SetParam(pOption->GetOptionType(Util::OptionType::GodRay), 1);
								break;
							default:
								break;
							}
						}
						DrawerMngr->FlipSetting();
						PostPassParts->Reset();
					}
				}
			}
			break;
			case 2:
			{
				for (int loop = 0; loop < 2; ++loop) {
					auto& param = this->m_Param[loop];
					auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::XSensing)));
					if (this->m_DrawUI->Get(param.m_MinID).IsActive() && this->m_DrawUI->Get(param.m_MinID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() - 1);
					}
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() + 1);
					}
				}
				//
				for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
					auto& param = this->m_Param[loop];
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						Util::EnumBattle Battle = static_cast<Util::EnumBattle>(loop - 2);
						KeyMngr->AssignBattleID(Battle, 0, KeyMngr->GetDefaultKeyAssign(Battle, 0));
					}
				}
			}
			break;
			case 3:
			{
				auto* Localize = Util::LocalizePool::Instance();
				for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
					auto& param = this->m_Param[loop];
					auto Type = pOption->GetOptionType(static_cast<Util::OptionType>(loop + static_cast<int>(Util::OptionType::HeadBobbing)));
					if (this->m_DrawUI->Get(param.m_MinID).IsActive() && this->m_DrawUI->Get(param.m_MinID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() - 1);
						Localize->Load();
					}
					if (this->m_DrawUI->Get(param.m_MaxID).IsActive() && this->m_DrawUI->Get(param.m_MaxID).IsSelectButton()) {
						pOption->SetParam(Type, pOption->GetParam(Type)->GetSelect() + 1);
						Localize->Load();
					}
				}
			}
			break;
			default:
				break;
			}
			UpdateColumnStr();
		}
		if (this->m_NowSelectTab == 2) {
			bool IsChange = false;
			for (int loop = 2; loop < this->m_NowTabMax; ++loop) {
				auto& param = this->m_Param[loop];
				Util::EnumBattle Battle = static_cast<Util::EnumBattle>(loop - 2);
				if (this->m_DrawUI->Get(param.m_ID).IsSelectButton()) {
					for (int loop2 = static_cast<int>(Util::EnumInput::Begin); loop2 < static_cast<int>(Util::EnumInput::Max); ++loop2) {
						Util::EnumInput Input = static_cast<Util::EnumInput>(loop2);
						if (KeyMngr->GetKeyPress(Input, true)) {
							if (Input != KeyMngr->GetKeyAssign(Battle, 0)) {
								Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
							}
							KeyMngr->AssignBattleID(Battle, 0, Input);
							IsChange = true;
							break;
						}
					}
				}
				if (IsChange) { break; }
			}
			if (IsChange || KeyMngr->IsDeviceChange()) {
				UpdateColumnStr();
			}
		}

		for (int loop = 0; loop < this->m_NowTabMax; ++loop) {
			auto& param = this->m_Param[loop];
			this->m_DrawUI->Get(param.m_ID).SetActive(this->m_DrawUI->Get(param.m_ID).IsSelectButton());
		}
	}
	this->m_DrawUI->Update(IsActive());
}