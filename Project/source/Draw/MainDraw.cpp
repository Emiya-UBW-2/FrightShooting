#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning(disable:5259)
#include "MainDraw.hpp"

#include "FontDraw.hpp"
#include "ImageDraw.hpp"
#include "Camera.hpp"
#include "PostPass.hpp"

#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Localize.hpp"

const Draw::MainDraw* Util::SingletonBase<Draw::MainDraw>::m_Singleton = nullptr;

namespace Draw {
	MainDraw::MainDraw(void) noexcept {
		this->m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		this->m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		DxLib::SetOutApplicationLogValidFlag(FALSE);
		DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
		DxLib::ChangeWindowMode(TRUE);
		DxLib::SetUseDirect3DVersion(DX_DIRECT3D_11);
		DxLib::SetGraphMode(this->m_ScreenWidth, this->m_ScreenHeight, 32);
		DxLib::SetUseDirectInputFlag(TRUE);
		DxLib::SetDirectInputMouseMode(TRUE);
		DxLib::SetWindowSizeChangeEnableFlag(TRUE, FALSE);
		DxLib::SetUsePixelLighting(TRUE);
		DxLib::SetWaitVSyncFlag(false);
		DxLib::Set3DSoundOneMetre(1.0f);									// 

		//プロジェクト固有設定
		{
			std::ifstream file("data/ProjectSetting/ProjectSetting.json");
			nlohmann::json jsonData = nlohmann::json::parse(file);
			{
				std::string Name = jsonData["GameTitle"];
				DxLib::SetMainWindowText(Name.c_str());
			}
			{
				int loop = 0;
				for (const int& Size : jsonData["BaseDispSize"]) {
					if (loop == 0) {
						this->m_DispWidth = Size;
					}
					else {
						this->m_DispHeight = Size;
					}
					++loop;
				}
			}
			{
				this->m_CalculateTick = jsonData["CalculateTick"];
			}
		}
		//
		DxLib::DxLib_Init();
		DxLib::SetSysCommandOffFlag(TRUE);
		DxLib::SetAlwaysRunFlag(TRUE);
		this->m_BufferScreen.Make(this->m_DispWidth, this->m_DispHeight, false);
		//
		FlipSetting();
	}
	MainDraw::~MainDraw(void) noexcept {
		this->m_BufferScreen.Dispose();
		DxLib::DxLib_End();
	}

	void MainDraw::FlipSetting(void) noexcept {
		auto* pOption = Util::OptionParam::Instance();
		{
			const auto& ModeStr = pOption->GetParam(pOption->GetOptionType(Util::OptionType::WindowMode))->GetValueNow();
			for (int loop = 0; loop < static_cast<int>(EnumWindowMode::Max); ++loop) {
				if (ModeStr == WindowModeStr[loop]) {
					SetWindowMode(static_cast<EnumWindowMode>(loop));
					break;
				}
			}
		}
		SetWaitVSync(pOption->GetParam(pOption->GetOptionType(Util::OptionType::VSync))->IsActive());
		this->m_FPSLimit = std::stoi(pOption->GetParam(pOption->GetOptionType(Util::OptionType::FPSLimit))->GetValueNow());

		this->m_RenderDispWidth = this->m_DispWidth * pOption->GetParam(pOption->GetOptionType(Util::OptionType::DrawScale))->GetSelect() / 100;
		this->m_RenderDispHeight = this->m_DispHeight * pOption->GetParam(pOption->GetOptionType(Util::OptionType::DrawScale))->GetSelect() / 100;
	}

	void MainDraw::Update(void) noexcept {
		LONGLONG WaitTime = DxLib::GetNowHiPerformanceCount() - this->m_TickTime;
		LONGLONG EndFrame = 1000LL * 1000LL / static_cast<LONGLONG>(GetFrameRate());
		if (WaitTime > EndFrame) {
			this->m_UpdateTickCount = static_cast<int>(WaitTime / EndFrame);
			//this->m_UpdateTickCount = std::min(this->m_UpdateTickCount, 30);//3回(20FPS)未満は処理落ちさせる
			this->m_TickTime += this->m_UpdateTickCount * EndFrame;
		}
		else {
			this->m_UpdateTickCount = 0;
		}
		//
		{
			int prevX = this->m_WindowDrawWidth;
			int prevY = this->m_WindowDrawHeight;
			DxLib::GetWindowSize(&this->m_WindowDrawWidth, &this->m_WindowDrawHeight);
			if ((prevX |= this->m_WindowDrawWidth) || (prevY |= this->m_WindowDrawHeight)) {
				int WidthT = this->m_WindowDrawWidth;
				int HeightT = this->m_WindowDrawHeight;
				if (HeightT >= (WidthT * GetDispHeight() / GetDispWidth())) {// 4:3
					HeightT = (WidthT * GetDispHeight() / GetDispWidth());
				}
				else {// 16:9より横長
					WidthT = (HeightT * GetDispWidth() / GetDispHeight());
				}
				this->m_WindowWidth = WidthT * GetDPI() / BaseDPI;
				this->m_WindowHeight = HeightT * GetDPI() / BaseDPI;
			}
		}
		if (this->m_WindowWidth != 0 && this->m_WindowHeight != 0) {
			//マウス座標取得
			DxLib::GetMousePoint(&this->m_MouseX, &this->m_MouseY);
			this->m_MouseX = (-(this->m_WindowDrawWidth - this->m_WindowWidth) / 2 + this->m_MouseX) * GetDispWidth() / this->m_WindowWidth;
			this->m_MouseY = (-(this->m_WindowDrawHeight - this->m_WindowHeight) / 2 + this->m_MouseY) * GetDispHeight() / this->m_WindowHeight;
		}
	}
	void MainDraw::StartDraw(void) noexcept {
		if (this->m_UpdateTickCount > 0) {
			this->m_CalcTimer = static_cast<float>(DxLib::GetNowHiPerformanceCount() - this->m_StartTime) / 1000.f / static_cast<float>(this->m_UpdateTickCount);
		}
		this->m_BufferScreen.SetDraw_Screen();
	}
	void MainDraw::EndDraw(void) noexcept {
		auto* Font = FontPool::Instance();
#if _DEBUG		//デバッグ表示
		//DxLib::printfDx("FPS:[%4.1f]\n", DxLib::GetFPS());
#endif
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		DxLib::ClearDrawScreen();
		{
			auto prev = DxLib::GetDrawMode();
			DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
			this->m_BufferScreen.DrawExtendGraph(
				this->m_WindowDrawWidth / 2 - this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2, this->m_WindowDrawHeight / 2 + this->m_WindowHeight / 2,
				false);
			DxLib::SetDrawMode(prev);

			Font->Get(FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
				FontXCenter::RIGHT, FontYCenter::TOP,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2 - 32, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2 + 32 + 24 * 0,
				ColorPalette::UIGreen, ColorPalette::UIDarkGreen,
				"%05.2f FPS", DxLib::GetFPS());

			Font->Get(FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
				FontXCenter::RIGHT, FontYCenter::TOP,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2 - 32, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2 + 32 + 24 * 1,
				ColorPalette::UIGreen, ColorPalette::UIDarkGreen,
				"Calc %05.2f ms", this->m_CalcTimer);

			Font->Get(FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
				FontXCenter::RIGHT, FontYCenter::TOP,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2 - 32, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2 + 32 + 24 * 2,
				ColorPalette::UIGreen, ColorPalette::UIDarkGreen,
				"Draw %05.2f ms", this->m_DrawTimer);

			Font->Get(FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
				FontXCenter::RIGHT, FontYCenter::TOP,
				this->m_WindowDrawWidth / 2 + this->m_WindowWidth / 2 - 32, this->m_WindowDrawHeight / 2 - this->m_WindowHeight / 2 + 32 + 24 * 3,
				ColorPalette::UIGreen, ColorPalette::UIDarkGreen,
				"DrawCall %3d", DxLib::GetDrawCallCount());
		}
		DxLib::ScreenFlip();
		if (!this->m_WaitVSync) {
			if (this->m_UpdateTickCount > 0) {
				this->m_DrawTimer = static_cast<float>(DxLib::GetNowHiPerformanceCount() - this->m_StartTime) / 1000.f - this->m_CalcTimer;
			}
			// 4msだけスリープ
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * (1000 / this->m_FPSLimit - 4))) {
				if (DxLib::ProcessMessage() != 0) { return; }
				DxLib::SleepThread(1);	// 1msecスリープする
			}
			while ((DxLib::GetNowHiPerformanceCount() - this->m_StartTime) < static_cast<LONGLONG>(1000 * 1000 / this->m_FPSLimit)) {
			}
		}
		else {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				DxLib::WaitVSync(1);
			}
		}
#if _DEBUG
		DxLib::clsDx();
#endif
		this->m_StartTime = DxLib::GetNowHiPerformanceCount();
	}
}
