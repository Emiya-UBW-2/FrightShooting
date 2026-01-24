#pragma once

#pragma warning(disable:5045)

#include "Draw/DrawUISystem.hpp"

#include "Util/Localize.hpp"
#include "Util/Sound.hpp"

#pragma warning( push, 3 )
#include <vector>
#include <string>
#include "File/json.hpp"
#pragma warning( pop )

class OptionWindow {
	struct ParamButton {
		std::string		m_Str{};
		int				m_ID{};
		int				m_MinID{};
		int				m_MaxID{};
		char		padding[4]{};
	};
private:
	constexpr static int	m_ParamMax{ 18 };

	Draw::DrawUISystem*	m_DrawUI{ nullptr };
	int				m_TabButton[4] = { InvalidID,InvalidID,InvalidID,InvalidID };
	int				m_UIBase{ InvalidID };
	int				m_CloseButton{ InvalidID };
	int				m_NowSelectTab{ 0 };
	int				m_NowTabMax{ 0 };
	ParamButton		m_Param[m_ParamMax]{};
	int				m_LanguagePrev{};
	char		padding[4]{};
	Sound::SoundUniqueID cancelID{ InvalidID };
	Sound::SoundUniqueID cursorID{ InvalidID };
	Sound::SoundUniqueID NGID{ InvalidID };
	Sound::SoundUniqueID OKID{ InvalidID };
	int					isSelectTabSoundPrev{ InvalidID };
	int					isSelectSoundPrev{ InvalidID };
public:
	OptionWindow(void) noexcept {}
	OptionWindow(const OptionWindow&) = delete;
	OptionWindow(OptionWindow&&) = delete;
	OptionWindow& operator=(const OptionWindow&) = delete;
	OptionWindow& operator=(OptionWindow&&) = delete;
	virtual ~OptionWindow(void) noexcept {}
public:
	bool		IsActive(void) const noexcept { return this->m_DrawUI->Get(this->m_UIBase).IsActive(); }
	void		SetActive(bool value) noexcept;
private:
	void		UpdateColumnStr(void) noexcept;
	void		SetTab(void) noexcept;
	void		EndTab(void) noexcept;
public:
	void		Init(void) noexcept;
	void		Update(void) noexcept;
	void		Draw(void) noexcept {
		this->m_DrawUI->Draw();
	}
	void		Dispose(void) noexcept {
		delete this->m_DrawUI;
	}
};
