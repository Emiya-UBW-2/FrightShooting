#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5259)
#include "Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/KeyGuide.hpp"

const Util::KeyParam* Util::SingletonBase<Util::KeyParam>::m_Singleton = nullptr;

namespace Util {
	void KeyParam::Assign(InputType type) noexcept {
		if (this->m_InputType != type) {
			this->m_InputType = type;
			this->m_DeviceChangeSwitch = true;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
		}
	}
	void KeyParam::Update(void) noexcept {
		this->m_DeviceChangeSwitch = false;
		//最後に入力したデバイスに更新
		if ((DxLib::CheckHitKeyAll(DX_CHECKINPUT_MOUSE) != 0) || (DxLib::CheckHitKeyAll(DX_CHECKINPUT_KEY) != 0)) {
			Assign(InputType::KeyBoard);
		}
		else if (DxLib::CheckHitKeyAll(DX_CHECKINPUT_PAD) != 0) {
			switch (DxLib::GetJoypadType(DX_INPUT_PAD1)) {
			case DX_PADTYPE_XBOX_360:			// Xbox360コントローラー
			case DX_PADTYPE_XBOX_ONE:			// XboxOneコントローラー
				Assign(InputType::XInput);
				break;
			case DX_PADTYPE_DUAL_SHOCK_4:		// PS4コントローラー
			case DX_PADTYPE_DUAL_SENSE:			// PS5コントローラー
			case DX_PADTYPE_SWITCH_JOY_CON_L:	// Switch Joycon(左)
			case DX_PADTYPE_SWITCH_JOY_CON_R:	// Switch Joycon(右)
			case DX_PADTYPE_SWITCH_PRO_CTRL:	// Switch Proコントローラー
			case DX_PADTYPE_OTHER:				// その他のコントローラー
				Assign(InputType::DInput);
				break;
			default:
				break;
			}
		}
		//XInput
		if (GetLastInputDevice() == InputType::XInput) {
			GetJoypadXInputState(DX_INPUT_PAD1, &this->m_inputXInput);
		}
		//DInput
		if (GetLastInputDevice() == InputType::DInput) {
			GetJoypadDirectInputState(DX_INPUT_PAD1, &this->m_inputDInput);
		}
		//
		for (size_t loop = static_cast<size_t>(EnumInput::Begin); loop < static_cast<size_t>(EnumInput::Max); ++loop) {
			this->m_Input.at(static_cast<size_t>(loop)).Update(GetKeyPress(static_cast<EnumInput>(loop), false));
		}

		auto* DrawerMngr = Draw::MainDraw::Instance();
		int MX = DrawerMngr->GetMousePositionX();
		int MY = DrawerMngr->GetMousePositionY();
		DxLib::GetMousePoint(&MX, &MY);
		bool IsHit = false;
		//
		if (GetLastInputDevice() == InputType::XInput) {
			int RS_X = this->m_inputXInput.ThumbRX;
			int RS_Y = -this->m_inputXInput.ThumbRY;
			//XInputが-1000~1000になるように変換
			RS_X = 1000 * RS_X / 32768;
			RS_Y = 1000 * RS_Y / 32768;
			if (!(RS_X == 0 && RS_Y == 0)) {
				//
				IsHit = true;
				MX += (RS_X * DrawerMngr->GetDispWidth() / 20) / 1000;
				MY += (RS_Y * DrawerMngr->GetDispHeight() / 20) / 1000;
			}
		}
		//
		if (GetLastInputDevice() == InputType::DInput) {
			int RS_X = this->m_inputDInput.Z;
			int RS_Y = this->m_inputDInput.Rz;
			if (!(RS_X == 0 && RS_Y == 0)) {
				//
				IsHit = true;
				MX += (RS_X * DrawerMngr->GetDispWidth() / 20) / 1000;
				MY += (RS_Y * DrawerMngr->GetDispHeight() / 20) / 1000;
			}
		}
		if (IsHit) {
			DxLib::SetMousePoint(MX, MY);
		}
	}
}