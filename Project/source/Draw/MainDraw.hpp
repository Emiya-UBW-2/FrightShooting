#pragma once
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#pragma warning( pop )

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "ImageDraw.hpp"
#include "../File/FileStream.hpp"
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/Option.hpp"

namespace Draw {
	enum class EnumWindowMode : size_t {
		Window,
		BorderLess,
		FullScreen,
		Max
	};
	static const char* WindowModeStr[static_cast<int>(EnumWindowMode::Max)] = {
		"Window",
		"BorderLess",
		"FullScreen",
	};

	//DXLib描画クラス
	class MainDraw : public Util::SingletonBase<MainDraw> {
	private:
		friend class Util::SingletonBase<MainDraw>;
	private:
		LONGLONG	m_TickTime{ 0 };
		LONGLONG	m_StartTime{ 0 };
		int			m_ScreenWidth{ 960 };			//スクリーンサイズX
		int			m_ScreenHeight{ 720 };			//スクリーンサイズY
		int			m_WindowDrawWidth{ 960 };		//ウィンドウサイズX
		int			m_WindowDrawHeight{ 720 };		//ウィンドウサイズY
		int			m_WindowWidth{ 960 };			//ディスプレイ表示X
		int			m_WindowHeight{ 720 };			//ディスプレイ表示Y
		ScreenHandle	m_BufferScreen{};
		int			m_MouseX{ InvalidID };
		int			m_MouseY{ InvalidID };
		int			m_MouseAddX{ 0 };
		int			m_MouseAddY{ 0 };
		int			m_RenderDispWidth{ 1920 };			//UI描画などの基準X
		int			m_RenderDispHeight{ 1080 };			//UI描画などの基準Y
		int			m_DispWidth{ 1920 };			//UI描画などの基準X
		int			m_DispHeight{ 1080 };			//UI描画などの基準Y
		int			m_CalculateTick{ 60 };			//更新レート
		int			m_UpdateTickCount{ 1 };
		int			m_FPSLimit{ 60 };				//更新レート
		bool		m_WaitVSync{ true };
		char		padding[3]{};
		float		m_CalcTimer{};
		float		m_DrawTimer{};
	private:
		static const int		BaseDPI = 96;
		static int GetDPI(void) noexcept {
			int DPI = BaseDPI;
			DxLib::GetMonitorDpi(NULL, &DPI);
			if (DPI == 0) {
				DPI = BaseDPI;
			}
			return DPI;
		}
	private://コンストラクタ、デストラクタ
		MainDraw(void) noexcept;
		MainDraw(const MainDraw&) = delete;
		MainDraw(MainDraw&&) = delete;
		MainDraw& operator=(const MainDraw&) = delete;
		MainDraw& operator=(MainDraw&&) = delete;
		~MainDraw(void) noexcept;
	public://Getter
		auto	GetFrameRate(void) const noexcept {
			if (this->m_UpdateTickCount >= 2) {
				return 30.f;
			}
			return static_cast<float>(this->m_CalculateTick);
		}
		auto	GetDeltaTime(void) const noexcept { return 1.f / GetFrameRate(); }
		auto	GetGravAccel(void) const noexcept { return 1.f / 2.f * (9.8f * Scale3DRate) / (GetFrameRate() * GetFrameRate()); }


		auto	GetRenderDispWidth(void) const noexcept { return this->m_RenderDispWidth; }//描画範囲のX座標
		auto	GetRenderDispHeight(void) const noexcept { return this->m_RenderDispHeight; }//描画範囲のY座標

		auto	GetDispWidth(void) const noexcept { return this->m_DispWidth; }//描画範囲のX座標
		auto	GetDispHeight(void) const noexcept { return this->m_DispHeight; }//描画範囲のY座標

		auto	GetWindowDrawWidth(void) const noexcept { return this->m_WindowDrawWidth; }//描画範囲のX座標
		auto	GetWindowDrawHeight(void) const noexcept { return this->m_WindowDrawHeight; }//描画範囲のY座標

		auto	GetMousePositionX(void) const noexcept { return this->m_MouseX; }//マウスのX座標
		auto	GetMousePositionY(void) const noexcept { return this->m_MouseY; }//マウスのY座標

		auto	GetMouseAddX(void) const noexcept { return this->m_MouseAddX; }//マウスのX座標
		auto	GetMouseAddY(void) const noexcept { return this->m_MouseAddY; }//マウスのY座標

		auto	GetUpdateTickCount(void) const noexcept { return this->m_UpdateTickCount; }//この周回では何回アップデートできるかをチェックする
	public://Setter
		void		SetWindowMode(EnumWindowMode value) const noexcept {
			switch (value) {
			case EnumWindowMode::Window:
				DxLib::SetWindowStyleMode(0);
				DxLib::SetWindowPosition(-8, 0);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, (this->m_ScreenHeight - 79) * GetDPI() / BaseDPI);
				DxLib::ChangeWindowMode(TRUE);
				break;
			case EnumWindowMode::BorderLess:
				DxLib::SetWindowStyleMode(4);
				DxLib::SetWindowPosition(-8, -8);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, this->m_ScreenHeight * GetDPI() / BaseDPI);
				DxLib::ChangeWindowMode(TRUE);
				break;
			case EnumWindowMode::FullScreen:
				DxLib::SetWindowStyleMode(4);
				DxLib::SetWindowPosition(-8, -8);
				DxLib::SetWindowSize(this->m_ScreenWidth * GetDPI() / BaseDPI, this->m_ScreenHeight * GetDPI() / BaseDPI);
				DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE);
				DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
				DxLib::ChangeWindowMode(FALSE);
				break;
			case EnumWindowMode::Max:
				break;
			default:
				break;
			}
		}
		void		SetWaitVSync(bool value) noexcept {
			this->m_WaitVSync = value;
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				DxLib::SetWaitVSyncFlag(FALSE);//DirectX9では後からの変更が効かない
			}
			else {
				DxLib::SetWaitVSyncFlag(value);//DirectX11ではあとからの変更が効く
			}
		}

		void		FlipSetting(void) noexcept;

		void		SetMousePosition(int MouseX, int MouseY) noexcept {
			this->m_MouseX = (MouseX * this->m_WindowWidth / GetDispWidth()) + (this->m_WindowDrawWidth - this->m_WindowWidth) / 2;
			this->m_MouseY = (MouseY * this->m_WindowHeight / GetDispHeight()) + (this->m_WindowDrawHeight - this->m_WindowHeight) / 2;

			DxLib::SetMousePoint(this->m_MouseX, this->m_MouseY);
		}
	public:
		//ループ前に実施
		void		LoopStart(void) noexcept {
			this->m_TickTime = DxLib::GetNowHiPerformanceCount();
			this->m_StartTime = DxLib::GetNowHiPerformanceCount();
		}
	public:
		//ループ中に実施
		bool		CanContinueLoop(void) const noexcept {
			return DxLib::ProcessMessage() == 0;
		}
		void		Update(void) noexcept;
		void		StartDraw(void) noexcept;
		void		EndDraw(void) noexcept;
	};
}
