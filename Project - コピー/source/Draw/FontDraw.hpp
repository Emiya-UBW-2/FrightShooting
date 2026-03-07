#pragma once
#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#pragma warning( pop )
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"

namespace Draw {
	enum class FontXCenter :size_t {
		LEFT,
		MIDDLE,
		RIGHT,
	};
	enum class FontYCenter :size_t {
		TOP,
		MIDDLE,
		BOTTOM,
	};
	enum class FontType :size_t {
		MS_Gothic,			// MSゴシック
		DIZ_UD_Gothic,		// DIZ UD ゴシック
	};
	// フォントハンドル
	class FontHandle : public Util::DXHandle {
	public:
		FontHandle(void) noexcept {}
		FontHandle(const FontHandle&) = delete;
		FontHandle(FontHandle&&) = delete;
		FontHandle& operator=(const FontHandle&) = delete;
		FontHandle& operator=(FontHandle&&) = delete;
		virtual ~FontHandle(void) noexcept {}
	protected:
		void Dispose_Sub(void) noexcept override {
			DxLib::DeleteFontToHandle(Util::DXHandle::get());
		}
	public:
		// 長さ取得
		int GetDrawWidth(std::basic_string_view<TCHAR> String) const noexcept { return DxLib::GetDrawNStringWidthToHandle(String.data(), String.size(), Util::DXHandle::get(), FALSE); }
		template <typename... Args>
		int GetDrawWidthFormat(const std::string& String, Args&&... args) const noexcept { return DxLib::GetDrawFormatStringWidthToHandle(Util::DXHandle::get(), String.c_str(), args...); }
		int GetDrawExtendWidth(float siz, std::basic_string_view<TCHAR> String) const noexcept { return DxLib::GetDrawExtendNStringWidthToHandle(static_cast<double>(siz), String.data(), String.size(), Util::DXHandle::get(), FALSE); }
		template <typename... Args>
		int GetDrawExtendWidthFormat(float siz, const std::string& String, Args&&... args) const noexcept { return DxLib::GetDrawExtendFormatStringWidthToHandle(static_cast<double>(siz), Util::DXHandle::get(), String.c_str(), args...); }
		// 文字描画
		template <typename... Args>
		void DrawString(FontXCenter FontX, FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= GetFontSizeToHandle(Util::DXHandle::get()) / 2;
				break;
			case FontYCenter::BOTTOM:
				y -= GetFontSizeToHandle(Util::DXHandle::get());
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				break;
			case FontXCenter::MIDDLE:
				x -= GetDrawWidthFormat(String, args...) / 2;
				break;
			case FontXCenter::RIGHT:
				x -= GetDrawWidthFormat(String, args...);
				break;
			default:
				break;
			}
			DxLib::DrawFormatString2ToHandle(x, y, Color, EdgeColor, Util::DXHandle::get(), String.c_str(), args...);
		}
		template <typename... Args>
		void DrawExtendString(FontXCenter FontX, FontYCenter FontY, int x, int y, float xsiz, float ysiz, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			switch (FontY) {
			case FontYCenter::TOP:
				break;
			case FontYCenter::MIDDLE:
				y -= static_cast<int>(static_cast<float>(GetFontSizeToHandle(Util::DXHandle::get()) / 2) * ysiz);
				break;
			case FontYCenter::BOTTOM:
				y -= static_cast<int>(static_cast<float>(GetFontSizeToHandle(Util::DXHandle::get())) * ysiz);
				break;
			default:
				break;
			}
			switch (FontX) {
			case FontXCenter::LEFT:
				break;
			case FontXCenter::MIDDLE:
				x -= GetDrawExtendWidthFormat(xsiz, String.c_str(), args...) / 2;
				break;
			case FontXCenter::RIGHT:
				x -= GetDrawExtendWidthFormat(xsiz, String.c_str(), args...);
				break;
			default:
				break;
			}
			DxLib::DrawExtendFormatString2ToHandle(x, y, static_cast<double>(xsiz), static_cast<double>(ysiz), Color, EdgeColor, Util::DXHandle::get(), String.c_str(), args...);
		}
		// 文字描画
		auto DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
			float draw_area_x_left = static_cast<float>(x1);
			float draw_area_x_right = static_cast<float>(x2);
			float draw_area_y_top = static_cast<float>(y1);
			float draw_area_y_bottom = static_cast<float>(y2);

			if (0 == String.length()) { return 0.f; }
			if (draw_area_x_right < draw_area_x_left || draw_area_y_bottom < draw_area_y_top) { return 0.f; }

			// 一文字ずつの描画幅情報を取得する
			const auto info = get_draw_string_char_info(String, Util::DXHandle::get());

			// ManageData->LineSpaceValidFlag ? ManageData->LineSpace : ManageData->BaseInfo.FontHeight
			const auto line_space = DxLib::GetFontLineSpaceToHandle(Util::DXHandle::get());
			const float area_width = draw_area_x_right - draw_area_x_left;
			const auto total_draw_width = info.back().DrawX + info.back().SizeX - info.front().DrawX;
			if (total_draw_width <= area_width) {
				// 一行ですむ場合
				const float Padding = (area_width - total_draw_width) / 2.0f;
				DxLib::DrawStringFToHandle(draw_area_x_left + Padding, draw_area_y_top, String.c_str(), Color, Util::DXHandle::get(), EdgeColor, false);
				return static_cast<float>(line_space);
			}

			// 複数行になる場合

			const float area_height = draw_area_y_bottom - draw_area_y_top;

			// 描画開始
			size_t current_string_byte_pos = 0;
			size_t line_front_string_byte_pos = 0;
			float current_y_relative = 0.0f;
			auto line_front_it = info.begin();
			for (auto it = info.begin(); it < info.end(); current_string_byte_pos += it->Bytes, ++it) {
				const auto line_width_contain_current_it_point_char = it->DrawX + it->SizeX - line_front_it->DrawX;
				if (area_width < line_width_contain_current_it_point_char) {
					using namespace std::string_literals;
					// 次の行に行く前に描画、itが指す文字は含まない
					const size_t str_len_byte = current_string_byte_pos - line_front_string_byte_pos;
					// it->DrawXは前の文字の右端に等しい
					// const float line_width = it->DrawX - line_front_it->DrawX;
					const float Padding = 0.f;// (area_width - line_width) / 2.0f;
					const auto line_string = String.substr(line_front_string_byte_pos / sizeof(TCHAR), (str_len_byte / sizeof(TCHAR)));
					DxLib::DrawStringFToHandle(draw_area_x_left + Padding, draw_area_y_top + current_y_relative, line_string.c_str(), Color, Util::DXHandle::get(), EdgeColor, false);
					// itが指す文字が先頭になる
					line_front_string_byte_pos = current_string_byte_pos;
					current_y_relative += static_cast<float>(line_space);
					line_front_it = it;
					if (area_height < current_y_relative) return current_y_relative;// 描画可能領域(y)を超えたら終了
				}
			}
			// 最終行の描画
			// const auto last_line_width = info.back().DrawX + info.back().SizeX - line_front_it->DrawX;
			const float Padding = 0.f;// (area_width - last_line_width) / 2.0f;
			const auto line_string = String.substr(line_front_string_byte_pos / sizeof(TCHAR));
			DxLib::DrawStringFToHandle(draw_area_x_left + Padding, draw_area_y_top + current_y_relative, line_string.c_str(), Color, Util::DXHandle::get(), EdgeColor, false);
			return current_y_relative + static_cast<float>(line_space);
		}
	public:
		// DXフォント用ハンドル作成
		void Load(std::basic_string_view<TCHAR> FontDataPath, int EdgeSize) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::LoadFontDataToHandleWithStrLen(FontDataPath.data(), FontDataPath.length(), EdgeSize));
		}
	private:
		static std::vector<DxLib::DRAWCHARINFO> get_draw_string_char_info(const std::basic_string<TCHAR>& string, int font_handle) {
			std::vector<DxLib::DRAWCHARINFO> info;
			info.resize(string.size());
			auto char_info_num = DxLib::GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), static_cast<int>(string.length() * sizeof(TCHAR)), font_handle, false);
			if (char_info_num < 0) throw std::runtime_error("fail in function DxLib::GetDrawStringCharInfoToHandle");
			if (info.size() < static_cast<size_t>(char_info_num)) {
				info.resize(static_cast<size_t>(char_info_num) + 1);
				// 再取得
				char_info_num = DxLib::GetDrawStringCharInfoToHandle(info.data(), info.size(), string.c_str(), static_cast<int>(string.length() * sizeof(TCHAR)), font_handle, false);
				if (char_info_num < 0 || static_cast<int>(info.size()) < char_info_num) throw std::runtime_error("fail to detect draw info.");
			}
			info.resize(static_cast<size_t>(char_info_num));
			return info;
		}
	};
	//
	class Fonthave {
		// カスタム項目
		FontType		m_Type{ 0 };
		int				m_EdgeSize{ InvalidID };// エッジサイズ
		int				m_CustomSize{ 0 };// フォントハンドル固有のサイズ
		// 
		int				m_scaleType{ DX_DRAWMODE_BILINEAR };
		int				m_CommonSize{ 0 };// フォントハンドル固有のサイズ
		FontHandle		m_Handle{};
	public:
		Fonthave(FontType type, int fontSize, int edgeSize) noexcept;
		Fonthave(const Fonthave&) = delete;
		Fonthave(Fonthave&&) = delete;
		Fonthave& operator=(const Fonthave&) = delete;
		Fonthave& operator=(Fonthave&&) = delete;

		~Fonthave(void) noexcept {
			this->m_Handle.Dispose();
		}
	public:
		bool			Equal(FontType type, int fontSize, int edgeSize) const noexcept {
			return ((this->m_Type == type) && (this->m_CustomSize == fontSize) && (this->m_EdgeSize == edgeSize));
		}
	public:
		template <typename... Args>
		void			DrawString(FontXCenter FontX, FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& String, Args&&... args) const noexcept {
			if (this->m_CustomSize == this->m_CommonSize) {
				this->m_Handle.DrawString(FontX, FontY, x, y, Color, EdgeColor, String, args...);
			}
			else {
				float Scale = static_cast<float>(this->m_CustomSize) / static_cast<float>(this->m_CommonSize);
				auto prev = DxLib::GetDrawMode();
				DxLib::SetDrawMode(this->m_scaleType);
				this->m_Handle.DrawExtendString(FontX, FontY, x, y, Scale, Scale, Color, EdgeColor, String, args...);
				DxLib::SetDrawMode(prev);
			}
		}

		auto			DrawStringAutoFit(int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& String) const noexcept {
			return this->m_Handle.DrawStringAutoFit(x1, y1, x2, y2, Color, EdgeColor, String);
		}

		template <typename... Args>
		auto			GetStringWidth(const std::string& String, Args&&... args) const noexcept {
			if (this->m_CustomSize == this->m_CommonSize) {
				return this->m_Handle.GetDrawWidthFormat(String, args...);
			}
			else {
				float Scale = static_cast<float>(this->m_CustomSize) / static_cast<float>(this->m_CommonSize);
				return this->m_Handle.GetDrawExtendWidthFormat(Scale, String, args...);
			}
		}
	};
	// フォントプール
	class FontPool : public Util::SingletonBase<FontPool> {
	private:
		friend class Util::SingletonBase<FontPool>;
	public:
	private:
		std::vector<std::unique_ptr<Fonthave>>	m_Pools{};
	private:
		FontPool(void) noexcept {}
		FontPool(const FontPool&) = delete;
		FontPool(FontPool&&) = delete;
		FontPool& operator=(const FontPool&) = delete;
		FontPool& operator=(FontPool&&) = delete;
	public:
		std::unique_ptr<Fonthave>& Get(FontType type, int fontSize, int edgeSize) noexcept {
			auto Find = std::find_if(this->m_Pools.begin(), this->m_Pools.end(), [&](const std::unique_ptr<Fonthave>& tgt) {return tgt->Equal(type, fontSize, edgeSize); });
			if (Find != this->m_Pools.end()) {
				return *Find;
			}
			this->m_Pools.emplace_back(std::make_unique<Fonthave>(type, fontSize, edgeSize));
			return this->m_Pools.back();
		}
	};
}
