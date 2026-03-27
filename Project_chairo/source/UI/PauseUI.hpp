#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:5045)

#include "../Draw/DrawUISystem.hpp"
#include "../Util/Localize.hpp"
#include "../Util/Sound.hpp"

#pragma warning( push, 3 )
#include <functional>
#pragma warning( pop )

class PauseUI {
private:
	Draw::DrawUISystem*						m_DrawUI{ nullptr };
	int										m_UIBase{ InvalidID };
	bool									m_IsActive{};
	char		padding[3]{};
	std::array<std::pair<int, std::function<void()>>, 3>	m_ButtonDo{};

	Sound::SoundUniqueID cursorID{ InvalidID };
	Sound::SoundUniqueID OKID{ InvalidID };
	int					isSelectSoundPrev{ InvalidID };
	char		padding2[4]{};
public:
	PauseUI(void) noexcept {}
	PauseUI(const PauseUI&) = delete;
	PauseUI(PauseUI&&) = delete;
	PauseUI& operator=(const PauseUI&) = delete;
	PauseUI& operator=(PauseUI&&) = delete;
	virtual ~PauseUI(void) noexcept {}
public:
	bool		IsEnd(void) const noexcept { return !this->m_DrawUI->Get(this->m_UIBase).IsActive() && this->m_DrawUI->Get(this->m_UIBase).IsAnimeEnd(); }
	void		SetEnd(void) noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(false); }
	bool		IsActive(void) const noexcept { return this->m_IsActive; }
	void		SetActive(bool value) noexcept { this->m_IsActive = value; }
	void		SetEvent(int ID, const std::function<void()>& value) noexcept { this->m_ButtonDo[static_cast<size_t>(ID)].second = value; }
public:
	void		Init(void) noexcept {
		cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
		OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

		this->m_DrawUI = new Draw::DrawUISystem();
		this->m_DrawUI->Init("data/UI/Pause/PauseUI.json");
		this->m_UIBase = this->m_DrawUI->GetID("");

		for (auto& b : this->m_ButtonDo) {
			size_t index = static_cast<size_t>(&b - &this->m_ButtonDo.front());
			b.first = this->m_DrawUI->GetID(("Tab" + std::to_string(index + 1)).c_str());
		}
		this->m_DrawUI->Get(this->m_UIBase).SetActive(true);
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (IsActive()) {
			int IsSelect = InvalidID;
			for (auto& b : this->m_ButtonDo) {
				size_t index = static_cast<size_t>(&b - &this->m_ButtonDo.front());
				if (this->m_DrawUI->Get(b.first).IsSelectButton()) {
					IsSelect = static_cast<int>(index);
					break;
				}
			}
			if (IsSelect != InvalidID && (IsSelect != isSelectSoundPrev)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			isSelectSoundPrev = IsSelect;

			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				for (auto& b : this->m_ButtonDo) {
					if (this->m_DrawUI->Get(b.first).IsSelectButton()) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
						if (b.second) {
							b.second();
						}
						break;
					}
				}
			}
		}
		this->m_DrawUI->Get(this->m_UIBase).SetActive(IsActive());
		this->m_DrawUI->Update(IsActive());
	}
	void		Draw(void) noexcept {
		this->m_DrawUI->Draw();
	}
	void		Dispose(void) noexcept {
		delete this->m_DrawUI;
	}
};
