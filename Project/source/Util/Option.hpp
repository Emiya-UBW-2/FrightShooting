#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "Enum.hpp"
#include "Util.hpp"
#include "../File/FileStream.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#include <array>
#pragma warning( pop )

namespace Util {
	enum class EnumOptionSelectType : size_t {
		Bool,
		Select,
		Int,
		Max,
	};
	static const char* OptionSelectTypeStr[static_cast<int>(EnumOptionSelectType::Max)] = {
		"Bool",
		"Select",
		"Int",
	};

	enum class OptionType :int {
		WindowMode,
		VSync,
		FPSLimit,
		Fov,
		GraphicsPreset,
		DrawScale,
		ObjectLevel,
		DoF,
		Bloom,
		Shadow,
		SSAO,
		ScreenEffect,
		Reflection,
		AntiAliasing,
		GodRay,
		Distortion,
		MotionBlur,

		MasterVolume,
		BGMVolume,
		SEVolume,

		XSensing,
		YSensing,

		HeadBobbing,
		Language,

		Max,
	};
	static const char* OptionStr[static_cast<int>(OptionType::Max)] = {
		"WindowMode",
		"VSync",
		"FPSLimit",
		"Fov",
		"GraphicsPreset",
		"DrawScale",
		"ObjectLevel",
		"DoF",
		"Bloom",
		"Shadow",
		"SSAO",
		"ScreenEffect",
		"Reflection",
		"AntiAliasing",
		"GodRay",
		"Distortion",
		"MotionBlur",

		"MasterVolume",
		"BGMVolume",
		"SEVolume",

		"XSensing",
		"YSensing",

		"HeadBobbing",
		"Language",
	};

	class OptionParam : public SingletonBase<OptionParam> {
	private:
		friend class SingletonBase<OptionParam>;
	private:
		class Param {
			EnumOptionSelectType		m_SelectType{};
			int							m_Value{};
			int							m_ValueMin{};
			int							m_ValueMax{};
			char		padding[4]{};
			std::string					m_Type{};
			std::vector<std::string>	m_ValueList{};
		public:
			std::string GetType(void) const noexcept { return this->m_Type; }

			int GetSelectMax(void) const noexcept { return this->m_ValueMax; }
			int GetSelectMin(void) const noexcept { return this->m_ValueMin; }
			int GetSelect(void) const noexcept { return this->m_Value; }

			std::string GetValueNow(void) const noexcept {
				switch (this->m_SelectType) {
				case EnumOptionSelectType::Bool:
				case EnumOptionSelectType::Select:
					return this->m_ValueList.at(static_cast<size_t>(GetSelect()));
					break;
				case EnumOptionSelectType::Int:
					return std::to_string(GetSelect());
					break;
				case EnumOptionSelectType::Max:
				default:
					break;
				}
				return "";
			}

			bool IsActive(void) const noexcept {
				if (this->m_SelectType == EnumOptionSelectType::Bool) {
					return GetValueNow() == "True";
				}
				return false;
			}
		public:
			void SetSelect(int Value) noexcept {
				this->m_Value = std::clamp(Value, GetSelectMin(), GetSelectMax());
			}
			void SetSelect(const std::string& Value) noexcept {
				switch (this->m_SelectType) {
				case EnumOptionSelectType::Bool:
				case EnumOptionSelectType::Select:
					for (auto& v : this->m_ValueList) {
						if (v == Value) {
							SetSelect(static_cast<int>(&v - &this->m_ValueList.front()));
							break;
						}
					}
					break;
				case EnumOptionSelectType::Int:
					SetSelect(std::stoi(Value));
					break;
				case EnumOptionSelectType::Max:
				default:
					break;
				}
			}
		public:
			void SetByJson(nlohmann::json& data) noexcept {
				//Type
				this->m_Type = data["Type"];
				//SelectType
				std::string SelType = data["SelectType"];
				for (int loop = 0; loop < static_cast<int>(EnumOptionSelectType::Max); ++loop) {
					if (SelType == OptionSelectTypeStr[loop]) {
						this->m_SelectType = static_cast<EnumOptionSelectType>(loop);
						break;
					}
				}
				//Value
				switch (this->m_SelectType) {
				case EnumOptionSelectType::Bool:
					this->m_ValueList.emplace_back("False");
					this->m_ValueList.emplace_back("True");
					this->m_ValueMin = 0;
					this->m_ValueMax = static_cast<int>(this->m_ValueList.size()) - 1;
					break;
				case EnumOptionSelectType::Select:
					if (data.contains("Value")) {
						for (auto& v : data["Value"]) {
							this->m_ValueList.emplace_back(v);
						}
					}
					this->m_ValueMin = 0;
					this->m_ValueMax = static_cast<int>(this->m_ValueList.size()) - 1;
					break;
				case EnumOptionSelectType::Int:
					if (data.contains("Value")) {
						int loop = 0;
						for (auto& v : data["Value"]) {
							if (loop == 0) {
								this->m_ValueMin = v;
							}
							else if (loop == 1) {
								this->m_ValueMax = v;
							}
							++loop;
						}
					}
					break;
				case EnumOptionSelectType::Max:
					break;
				default:
					break;
				}
				//DefaultValue
				switch (this->m_SelectType) {
				case EnumOptionSelectType::Bool:
				case EnumOptionSelectType::Select:
				{
					std::string DefaultValue = data["DefaultValue"];
					for (auto& v : this->m_ValueList) {
						if (v == DefaultValue) {
							SetSelect(static_cast<int>(&v - &this->m_ValueList.front()));
							break;
						}
					}
				}
				break;
				case EnumOptionSelectType::Int:
				{
					int tmp = data["DefaultValue"];
					SetSelect(tmp);
				}
				break;
				case EnumOptionSelectType::Max:
					break;
				default:
					break;
				}
			}
			void Dispose(void) noexcept {
				this->m_ValueList.clear();
			}
		};

		std::vector<Param>									m_ParamList;
		std::array<int, static_cast<int>(OptionType::Max)>	m_OptionID;
		//char		padding[4]{};
	private:
		//コンストラクタ
		OptionParam(void) noexcept {
			std::ifstream file("data/ProjectSetting/Option.json");
			nlohmann::json jsonData = nlohmann::json::parse(file);
			for (auto& data : jsonData["data"]) {
				this->m_ParamList.emplace_back();
				this->m_ParamList.back().SetByJson(data);
			}
			Load();
			for (int loop = 0; loop < static_cast<int>(OptionType::Max); ++loop) {
				this->m_OptionID.at(static_cast<size_t>(loop)) = GetParamID(OptionStr[static_cast<size_t>(loop)]);
			}
		}
		OptionParam(const OptionParam&) = delete;
		OptionParam(OptionParam&&) = delete;
		OptionParam& operator=(const OptionParam&) = delete;
		OptionParam& operator=(OptionParam&&) = delete;
		//デストラクタ
		~OptionParam(void) noexcept {
			Save();
			for (auto& param : this->m_ParamList) {
				param.Dispose();
			}
			this->m_ParamList.clear();
		}
	public:
		int GetParamID(std::string_view Type) const noexcept {
			for (auto& param : this->m_ParamList) {
				if (param.GetType() == Type) {
					return static_cast<int>(&param - &this->m_ParamList.front());
				}
			}
			return InvalidID;
		}
		const Param* GetParam(int ID) const noexcept {
			if (ID == InvalidID) { return nullptr; }
			return &this->m_ParamList.at(static_cast<size_t>(ID));
		}
		void SetParam(int ID, int Param) noexcept {
			if (ID == InvalidID) { return; }
			this->m_ParamList.at(static_cast<size_t>(ID)).SetSelect(Param);
		}
		int GetOptionType(OptionType ID) const noexcept { return this->m_OptionID.at(static_cast<size_t>(ID)); }
	public:
		void Load(void) noexcept {
			File::InputFileStream Istream("Save/Option.dat");
			while (!Istream.ComeEof()) {
				std::string Line = File::InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
				std::string Left = File::InputFileStream::getleft(Line, "=");
				std::string Right = File::InputFileStream::getright(Line, "=");
				for (auto& param : this->m_ParamList) {
					if (param.GetType() == Left) {
						param.SetSelect(Right);
						break;
					}
				}
			}
		}
		void Save(void) noexcept {
			File::OutputFileStream Ostream("Save/Option.dat");
			for (auto& param : this->m_ParamList) {
				std::string Line = param.GetType() + "=" + param.GetValueNow();
				Ostream.AddLine(Line);
			}
		}
	};
}