#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4710)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/Key.hpp"

#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

#include "MainDraw.hpp"
#include "FontDraw.hpp"
#include "ImageDraw.hpp"

namespace Draw {
	enum class PartsType :size_t {
		Box,
		NineSlice,
		Image,
		String,
		Json,
		Max,
	};
	static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
		"Box",
		"NineSlice",
		"Image",
		"String",
		"Json",
	};

	enum class AnimType :size_t {
		None,
		Normal,
		OnSelect,
		OnPress,
		Active,
		DisActive,
		Max,
	};

	static const char* AnimTypeStr[static_cast<int>(AnimType::Max)] = {
		"None",
		"Normal",
		"OnSelect",
		"OnPress",
		"Active",
		"DisActive",
	};

	class Param2D {
	public:
		Util::VECTOR2D	OfsNoRad{ Util::VECTOR2D::vget(0.f, 0.f) };
		Util::VECTOR2D	Ofs{};
		Util::VECTOR2D	Size{};
		Util::VECTOR2D	Scale{ Util::VECTOR2D::vget(1.f, 1.f) };
		Util::VECTOR2D	Center{ Util::VECTOR2D::vget(0.f, 0.f) };
		float			Rad{ Util::deg2rad(0.f) };
		char		padding[4]{};
		Util::ColorRGBA	Color{ Util::ColorRGBA(255, 255, 255, 255) };
	public:
		Param2D(void) noexcept {}
		Param2D(const Param2D& o) noexcept { *this = o; }
		Param2D(Param2D&& o) noexcept { *this = o; }
		Param2D& operator=(const Param2D& o) noexcept {
			this->OfsNoRad = o.OfsNoRad;
			this->Ofs = o.Ofs;
			this->Size = o.Size;
			this->Scale = o.Scale;
			this->Center = o.Center;
			this->Rad = o.Rad;
			this->Color = o.Color;
			return *this;
		}
		Param2D& operator=(Param2D&& o) noexcept {
			this->OfsNoRad = o.OfsNoRad;
			this->Ofs = o.Ofs;
			this->Size = o.Size;
			this->Scale = o.Scale;
			this->Center = o.Center;
			this->Rad = o.Rad;
			this->Color = o.Color;
			return *this;
		}
		virtual ~Param2D(void) noexcept {}
	};

	class DrawUISystem;

	class DrawModule {
		struct AnimParam {
			size_t			m_TargetID{};

			bool			m_OfsNoRadChanged{};
			bool			m_OfsChanged{};
			bool			m_SizeChanged{};
			bool			m_ScaleChanged{};
			bool			m_CenterChanged{};
			bool			m_RadChanged{};
			bool			m_ColorChanged{};
			char		padding[1]{};

			Param2D			m_Target{};

			int				m_StartFrame{};
			int				m_EndFrame{};
		};
		class PartsParam {
			std::string			m_Name{};
			PartsType			m_Type{};

			Param2D				m_Base{};
			Param2D				m_Now{};
			Param2D				m_Before{};

			Util::VECTOR2D		m_Min{};
			Util::VECTOR2D		m_Max{};
			std::string			m_ImagePath{};
			int					m_DrawModuleHandle{};
			int					m_StringID{};
			std::string			m_String{};
			const GraphHandle*	m_ImageHandle{};
			bool				m_IsHitCheck{};
			char		padding[7]{};
		public:
			void			SetString(std::string_view value) noexcept { this->m_String = value; }
			const auto& GetName(void) const noexcept { return this->m_Name; }
			bool			IsHitCheck(void) const noexcept { return this->m_IsHitCheck; }
		public:
			bool			IsHitPoint(int x, int y, Param2D Parent) const noexcept;
			void			Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
			void			Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
		public:
			void			CalcAnim(const AnimParam& Anim, bool IsFirstFrame, float Per) noexcept;
			void			SetDefault(void) noexcept {
				this->m_Before = this->m_Now = this->m_Base;
			}
			void			AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view ChildBranch, std::string_view FilePath, Param2D Param) noexcept;
			void			SetByJson(DrawUISystem* DrawUI, nlohmann::json& data, const std::string& BranchName) noexcept;
		};
		struct AnimData {
			AnimType				m_Type{ AnimType::None };
			std::vector<AnimParam>	m_AnimParam;
			bool					m_IsLoop{};
			char		padding[7]{};
		};
	private:
		std::vector<PartsParam>	m_PartsParam{};
		std::vector<AnimData>	m_AnimData{};
		Param2D					m_BasePositionParam{ Param2D() };
		int						m_AnimDataLastSelect{};
		int						m_Frame{};
		std::string				m_BranchName{};
		bool					m_IsSelect{};
		bool					m_IsActive{ true };
		bool					m_IsAnimeEnd{ false };
		bool					m_UseActive{};
		bool					m_UseButton{};
		bool					m_IsHitCheck{};
		char		padding[2]{};
	public:
		//コンストラクタ
		DrawModule(void) noexcept {
			this->m_PartsParam.reserve(128);
			this->m_AnimData.reserve(128);
		}
		DrawModule(const DrawModule&) = delete;
		DrawModule(DrawModule&& o) noexcept {
			this->m_PartsParam = o.m_PartsParam;
			this->m_AnimData = o.m_AnimData;
			this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
			this->m_Frame = o.m_Frame;
			this->m_IsSelect = o.m_IsSelect;
		}
		DrawModule& operator=(const DrawModule&) = delete;
		DrawModule& operator=(DrawModule&& o) noexcept {
			this->m_PartsParam = o.m_PartsParam;
			this->m_AnimData = o.m_AnimData;
			this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
			this->m_Frame = o.m_Frame;
			this->m_IsSelect = o.m_IsSelect;
			return *this;
		}
		//デストラクタ
		~DrawModule(void) noexcept {
			this->m_PartsParam.clear();
			this->m_AnimData.clear();
		}
	public:
		bool			IsSelectButton(void) const noexcept { return this->m_IsSelect; }
		bool			IsActive(void) const noexcept { return this->m_IsActive; }
		bool			IsAnimeEnd(void) const noexcept { return this->m_IsAnimeEnd; }
		bool			IsHitCheck(void) const noexcept { return this->m_IsHitCheck; }
		std::string		GetBranchName(void) const noexcept { return this->m_BranchName; }
		Param2D			GetBasePositionParam(void) const noexcept { return this->m_BasePositionParam; }
		PartsParam* GetParts(std::string_view ChildName) const noexcept {
			for (auto& parts : this->m_PartsParam) {
				if (parts.GetName() == ChildName) {
					return (PartsParam*)&parts;
				}
			}
			return nullptr;
		}
	public:
		void			SetActive(bool value) noexcept {
			this->m_IsActive = value;
			this->m_IsAnimeEnd = false;
		}
		void			AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view FilePath, Param2D Param = Param2D()) noexcept;
		void			DeleteChild(std::string_view ChildName) noexcept;
	public:
		void			Init(DrawUISystem* DrawUI, std::string_view Path, std::string_view Branch) noexcept;
		void			Update(DrawUISystem* DrawUI, Param2D Param = Param2D()) noexcept;
		void			Draw(DrawUISystem* DrawUI) noexcept {
			for (auto& parts : this->m_PartsParam) {
				parts.Draw(DrawUI, this->m_BasePositionParam);
			}
			//DrawString(Param.OfsNoRad.x, Param.OfsNoRad.y, GetBranchName().c_str(), ColorPalette::Red);
		}
	};



	class DrawUISystem {
		std::vector<DrawModule>	m_DrawModule;
		bool					m_IsFirstFrameDraw{};
		char		padding[7]{};
	public://コンストラクタ、デストラクタ
		DrawUISystem(void) noexcept {
			this->m_DrawModule.reserve(128);
		}
		DrawUISystem(const DrawUISystem&) = delete;
		DrawUISystem(DrawUISystem&&) = delete;
		DrawUISystem& operator=(const DrawUISystem&) = delete;
		DrawUISystem& operator=(DrawUISystem&&) = delete;
		~DrawUISystem(void) noexcept {}
	public:
		int				Add(std::string_view Path, std::string_view Branch) noexcept {
			int ID = static_cast<int>(this->m_DrawModule.size());
			this->m_DrawModule.emplace_back();
			this->m_DrawModule.back().Init(this, Path, Branch);
			return ID;
		}
		DrawModule& Get(int ID) noexcept { return this->m_DrawModule.at(static_cast<size_t>(ID)); }
		int				GetID(std::string_view Value) noexcept {
			for (auto& Module : this->m_DrawModule) {
				if (Value == Module.GetBranchName()) {
					return static_cast<int>(&Module - &this->m_DrawModule.front());
				}
			}
			return InvalidID;
		}
	public:
		void			AddChild(std::string_view Path, std::string_view FilePath, Param2D Param = Param2D()) noexcept {
			std::string Name;
			std::string ChildName;
			if (Path.find("/") != std::string::npos) {
				ChildName = Path.substr(Path.rfind("/") + 1);
				Name = Path.substr(0, Path.rfind("/"));
			}
			else {
				ChildName = Path;
				Name = "";
			}
			Get(GetID(Name.c_str())).AddChild(this, ChildName.c_str(), FilePath, Param);
		}
		void			DeleteChild(std::string_view Path) noexcept {
			std::string Name;
			std::string ChildName;
			if (Path.find("/") != std::string::npos) {
				ChildName = Path.substr(Path.rfind("/") + 1);
				Name = Path.substr(0, Path.rfind("/"));
			}
			else {
				ChildName = Path;
				Name = "";
			}
			//子供のモジュールを削除
			for (int loop = 0, max = static_cast<int>(this->m_DrawModule.size()); loop < max; ++loop) {
				auto& Module = this->m_DrawModule.at(static_cast<size_t>(loop));
				if (Module.GetBranchName().find(Path) != std::string::npos) {
					this->m_DrawModule.erase(this->m_DrawModule.begin() + loop);
					--loop;
					--max;
				}
			}
			for (auto& Module : this->m_DrawModule) {
				if (Module.GetBranchName() == Name) {
					Module.DeleteChild(ChildName.c_str());
					break;
				}
			}
		}
	public:
		void			Init(std::string_view Path) noexcept {
			Add(Path, "");
			this->m_IsFirstFrameDraw = true;
		}
		void			Update(bool IsActiveMouseMove) noexcept {
			if (this->m_DrawModule.size() == 0) { return; }
			this->m_DrawModule.at(0).Update(this);
			if (IsActiveMouseMove) {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				auto* KeyMngr = Util::KeyParam::Instance();

				Util::VECTOR2D MousePos = Util::VECTOR2D::vget(static_cast<float>(DrawerMngr->GetMousePositionX()), static_cast<float>(DrawerMngr->GetMousePositionY()));

				struct ButtonList {
					size_t ID{};
					int ABSDeg{};
					float Length{};
				};

				std::vector<ButtonList> DownList{};
				std::vector<ButtonList> RightList{};
				std::vector<ButtonList> UpList{};
				std::vector<ButtonList> LeftList{};

				bool IsDown = KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::DOWN);
				bool IsRight = KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::RIGHT);
				bool IsUp = KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::UP);
				bool IsLeft = KeyMngr->GetMenuKeyRepeat(Util::EnumMenu::LEFT);

				for (auto& Module : this->m_DrawModule) {
					if (!Module.IsHitCheck()) { continue; }
					if (Module.IsSelectButton()) { continue; }

					size_t index = static_cast<size_t>(&Module - &this->m_DrawModule.front());

					Util::VECTOR2D Pos = Module.GetBasePositionParam().OfsNoRad;
					Util::VECTOR2D Vec = Pos - MousePos;
					float Length = std::hypotf(Vec.x, Vec.y);
					float Deg = Util::rad2deg(std::atan2f(Vec.x, Vec.y));

					if (IsDown && (0.f + -45.f <= Deg && Deg < 0.f + 45.f)) {
						ButtonList List;
						List.ID = index;
						List.Length = Length;
						List.ABSDeg = static_cast<int>(std::fabsf(Deg)) / 3;
						DownList.emplace_back(List);

					}
					if (IsRight && (90.f - 45.f <= Deg && Deg <= 90.f + 45.f)) {
						ButtonList List;
						List.ID = index;
						List.Length = Length;
						List.ABSDeg = static_cast<int>(std::fabsf(Deg - 90.f)) / 3;
						RightList.emplace_back(List);
					}
					if (IsUp && (180.f + -45.f <= Deg || Deg <= -180.f + 45.f)) {
						ButtonList List;
						List.ID = index;
						List.Length = Length;
						if (Deg > 0) {
							List.ABSDeg = static_cast<int>(std::fabsf(180.f - Deg)) / 3;
						}
						else {
							List.ABSDeg = static_cast<int>(std::fabsf(-180.f - Deg)) / 3;
						}
						UpList.emplace_back(List);
					}
					if (IsLeft && (-90.f + -45.f <= Deg && Deg <= -90.f + 45.f)) {
						ButtonList List;
						List.ID = index;
						List.Length = Length;
						List.ABSDeg = static_cast<int>(std::fabsf(-90.f - Deg)) / 3;
						LeftList.emplace_back(List);
					}
				}
				if (IsDown && DownList.size() > 0) {
					std::sort(DownList.begin(), DownList.end(), [](const ButtonList& A, const ButtonList& B) { return A.Length < B.Length; });
					size_t ID = DownList.front().ID;
					int Deg = DownList.front().ABSDeg;
					for (size_t loop = 0; loop < std::min<size_t>(DownList.size(), 3); ++loop) {
						if (Deg > DownList.at(loop).ABSDeg) {
							Deg = DownList.at(loop).ABSDeg;
							ID = DownList.at(loop).ID;
						}
					}

					auto& Module = this->m_DrawModule.at(ID);
					Util::VECTOR2D Pos = Module.GetBasePositionParam().OfsNoRad;
					DrawerMngr->SetMousePosition(static_cast<int>(Pos.x), static_cast<int>(Pos.y));
				}
				if (IsRight && RightList.size() > 0) {
					std::sort(RightList.begin(), RightList.end(), [](const ButtonList& A, const ButtonList& B) { return A.Length < B.Length; });
					size_t ID = RightList.front().ID;
					int Deg = RightList.front().ABSDeg;
					for (size_t loop = 0; loop < std::min<size_t>(RightList.size(), 3); ++loop) {
						if (Deg > RightList.at(loop).ABSDeg) {
							Deg = RightList.at(loop).ABSDeg;
							ID = RightList.at(loop).ID;
						}
					}

					auto& Module = this->m_DrawModule.at(ID);
					Util::VECTOR2D Pos = Module.GetBasePositionParam().OfsNoRad;
					DrawerMngr->SetMousePosition(static_cast<int>(Pos.x), static_cast<int>(Pos.y));
				}
				if (IsUp && UpList.size() > 0) {
					std::sort(UpList.begin(), UpList.end(), [](const ButtonList& A, const ButtonList& B) { return A.Length < B.Length; });
					size_t ID = UpList.front().ID;
					int Deg = UpList.front().ABSDeg;
					for (size_t loop = 0; loop < std::min<size_t>(UpList.size(), 3); ++loop) {
						if (Deg > UpList.at(loop).ABSDeg) {
							Deg = UpList.at(loop).ABSDeg;
							ID = UpList.at(loop).ID;
						}
					}

					auto& Module = this->m_DrawModule.at(ID);
					Util::VECTOR2D Pos = Module.GetBasePositionParam().OfsNoRad;
					DrawerMngr->SetMousePosition(static_cast<int>(Pos.x), static_cast<int>(Pos.y));
				}
				if (IsLeft && LeftList.size() > 0) {
					std::sort(LeftList.begin(), LeftList.end(), [](const ButtonList& A, const ButtonList& B) { return A.Length < B.Length; });
					size_t ID = LeftList.front().ID;
					int Deg = LeftList.front().ABSDeg;
					for (size_t loop = 0; loop < std::min<size_t>(LeftList.size(), 3); ++loop) {
						if (Deg > LeftList.at(loop).ABSDeg) {
							Deg = LeftList.at(loop).ABSDeg;
							ID = LeftList.at(loop).ID;
						}
					}

					auto& Module = this->m_DrawModule.at(ID);
					Util::VECTOR2D Pos = Module.GetBasePositionParam().OfsNoRad;
					DrawerMngr->SetMousePosition(static_cast<int>(Pos.x), static_cast<int>(Pos.y));
				}
			}
		}
		void			Draw(void) noexcept {
			if (this->m_DrawModule.size() == 0) { return; }
			if (this->m_IsFirstFrameDraw) {
				this->m_IsFirstFrameDraw = false;
				return;
			}
			this->m_DrawModule.at(0).Draw(this);
		}
		void			Dispose(void) noexcept {
			this->m_DrawModule.clear();
		}
	};
}
