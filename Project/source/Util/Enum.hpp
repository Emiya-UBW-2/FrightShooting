#pragma once
#include <cstdint>

static constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
static constexpr int		InvalidID{ -1 };								/*共通の無効値*/
static constexpr float		FrameRate{ 60.f };								/*フレームレート*/
static constexpr float		DeltaTime{ 1.f / FrameRate };					/*1F単位の経過時間*/
static constexpr float		GravAccel{ 1.f / 2.f * (9.8f * Scale3DRate) / (FrameRate * FrameRate) };	/*1F単位の重力加速度*/

enum class EnumScene :size_t {
	Title,
	Main,
};

enum class LanguageType :int {
	Jpn,
	Eng,
};
static const char* LanguageStr[] = {
"Jpn",
"Eng",
};

namespace ColorPalette {

	// 固定でA8R8G8B8を出す
	constexpr uint32_t GetColorFix(uint8_t Red, uint8_t Green, uint8_t Blue) {
		return 0xff000000 | (((uint32_t)Red) << 16) | (((uint32_t)Green) << 8) | ((uint32_t)Blue);
	}
	// カラー指定
	static constexpr unsigned int Red{ GetColorFix(255, 0, 0) };
	static constexpr unsigned int Red25{ GetColorFix(192, 0, 0) };
	static constexpr unsigned int Red50{ GetColorFix(128, 0, 0) };
	static constexpr unsigned int Green{ GetColorFix(0, 255, 0) };// GetColorFix(43, 255, 91)
	static constexpr unsigned int DarkGreen{ GetColorFix(0, 64, 0) };// GetColorFix(21, 128, 45)
	static constexpr unsigned int UIGreen{ GetColorFix(0, 192, 0) };
	static constexpr unsigned int UIDarkGreen{ GetColorFix(0, 32, 0) };
	static constexpr unsigned int Blue{ GetColorFix(0, 0, 255) };
	static constexpr unsigned int Yellow{ GetColorFix(255, 255, 0) };

	static constexpr unsigned int White{ GetColorFix(255, 255, 255) };
	static constexpr unsigned int Gray05{ GetColorFix(255 * 95 / 100, 255 * 95 / 100, 255 * 95 / 100) };
	static constexpr unsigned int Gray10{ GetColorFix(255 * 90 / 100, 255 * 90 / 100, 255 * 90 / 100) };
	static constexpr unsigned int Gray15{ GetColorFix(255 * 85 / 100, 255 * 85 / 100, 255 * 85 / 100) };
	static constexpr unsigned int Gray20{ GetColorFix(255 * 80 / 100, 255 * 80 / 100, 255 * 80 / 100) };
	static constexpr unsigned int Gray25{ GetColorFix(255 * 75 / 100, 255 * 75 / 100, 255 * 75 / 100) };
	static constexpr unsigned int Gray30{ GetColorFix(255 * 70 / 100, 255 * 70 / 100, 255 * 70 / 100) };
	static constexpr unsigned int Gray35{ GetColorFix(255 * 65 / 100, 255 * 65 / 100, 255 * 65 / 100) };
	static constexpr unsigned int Gray40{ GetColorFix(255 * 60 / 100, 255 * 60 / 100, 255 * 60 / 100) };
	static constexpr unsigned int Gray45{ GetColorFix(255 * 55 / 100, 255 * 55 / 100, 255 * 55 / 100) };
	static constexpr unsigned int Gray50{ GetColorFix(255 * 50 / 100, 255 * 50 / 100, 255 * 50 / 100) };
	static constexpr unsigned int Gray55{ GetColorFix(255 * 45 / 100, 255 * 45 / 100, 255 * 45 / 100) };
	static constexpr unsigned int Gray60{ GetColorFix(255 * 40 / 100, 255 * 40 / 100, 255 * 40 / 100) };
	static constexpr unsigned int Gray65{ GetColorFix(255 * 35 / 100, 255 * 35 / 100, 255 * 35 / 100) };
	static constexpr unsigned int Gray70{ GetColorFix(255 * 30 / 100, 255 * 30 / 100, 255 * 30 / 100) };
	static constexpr unsigned int Gray75{ GetColorFix(255 * 25 / 100, 255 * 25 / 100, 255 * 25 / 100) };
	static constexpr unsigned int Gray80{ GetColorFix(255 * 20 / 100, 255 * 20 / 100, 255 * 20 / 100) };
	static constexpr unsigned int Gray85{ GetColorFix(255 * 15 / 100, 255 * 15 / 100, 255 * 15 / 100) };
	static constexpr unsigned int Gray90{ GetColorFix(255 * 10 / 100, 255 * 10 / 100, 255 * 10 / 100) };
	static constexpr unsigned int Gray95{ GetColorFix(255 * 5 / 100, 255 * 5 / 100, 255 * 5 / 100) };
	static constexpr unsigned int Black{ GetColorFix(0, 0, 0) };
}
