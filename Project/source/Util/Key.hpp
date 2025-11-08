#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "Util.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )
#include "../File/FileStream.hpp"

namespace Util {
	enum class InputType :size_t {
		KeyBoard,
		XInput,
		DInput,
		Max,
	};

	enum class EnumInput :size_t {
		Begin,
		Mouse_Begin = Begin,
		Mouse_Left = Mouse_Begin,
		Mouse_Right,
		Mouse_Middle,
		Mouse_4,
		Mouse_5,
		Mouse_6,
		Mouse_7,
		Mouse_8,
		Mouse_Max,

		Key_Begin = Mouse_Max,
		Key_BACK = Key_Begin,
		Key_TAB,
		Key_RETURN,
		Key_LSHIFT,
		Key_RSHIFT,
		Key_LCONTROL,
		Key_RCONTROL,
		Key_ESCAPE,
		Key_SPACE,
		Key_PGUP,
		Key_PGDN,
		Key_END,
		Key_HOME,
		Key_LEFT,
		Key_UP,
		Key_RIGHT,
		Key_DOWN,
		Key_INSERT,
		Key_DELETE,
		Key_MINUS,
		Key_YEN,
		Key_PREVTRACK,
		Key_PERIOD,
		Key_SLASH,
		Key_LALT,
		Key_RALT,
		Key_SCROLL,
		Key_SEMICOLON,
		Key_COLON,
		Key_LBRACKET,
		Key_RBRACKET,
		Key_AT,
		Key_BACKSLASH,
		Key_COMMA,
		Key_KANJI,
		Key_CONVERT,
		Key_NOCONVERT,
		Key_KANA,
		Key_APPS,
		Key_CAPSLOCK,
		Key_SYSRQ,
		Key_PAUSE,
		Key_LWIN,
		Key_RWIN,
		Key_NUMLOCK,
		Key_NUMPAD0,
		Key_NUMPAD1,
		Key_NUMPAD2,
		Key_NUMPAD3,
		Key_NUMPAD4,
		Key_NUMPAD5,
		Key_NUMPAD6,
		Key_NUMPAD7,
		Key_NUMPAD8,
		Key_NUMPAD9,
		Key_MULTIPLY,
		Key_ADD,
		Key_SUBTRACT,
		Key_DECIMAL,
		Key_DIVIDE,
		Key_NUMPADENTER,
		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,
		Key_A,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,
		Key_0,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,
		Key_Max,

		XInput_Begin = Key_Max,
		XInput_SQUARE = XInput_Begin,
		XInput_NG,
		XInput_OK,
		XInput_TRIANGLE,
		XInput_L1,
		XInput_R1,
		XInput_L2,
		XInput_R2,
		XInput_OPTION,
		XInput_L3,
		XInput_R3,
		XInput_LEFT,
		XInput_UP,
		XInput_RIGHT,
		XInput_DOWN,
		XInput_ESCAPE,

		XInput_LSTICKLEFT,
		XInput_LSTICKUP,
		XInput_LSTICKRIGHT,
		XInput_LSTICKDOWN,
		XInput_RSTICKLEFT,
		XInput_RSTICKUP,
		XInput_RSTICKRIGHT,
		XInput_RSTICKDOWN,
		XInput_Max,

		DInput_Begin = XInput_Max,
		DInput_SQUARE = DInput_Begin,
		DInput_NG,
		DInput_OK,
		DInput_TRIANGLE,
		DInput_L1,
		DInput_R1,
		DInput_L2,
		DInput_R2,
		DInput_OPTION,
		DInput_L3,
		DInput_R3,
		DInput_LEFT,
		DInput_UP,
		DInput_RIGHT,
		DInput_DOWN,
		DInput_ESCAPE,

		DInput_LSTICKLEFT,
		DInput_LSTICKUP,
		DInput_LSTICKRIGHT,
		DInput_LSTICKDOWN,
		DInput_RSTICKLEFT,
		DInput_RSTICKUP,
		DInput_RSTICKRIGHT,
		DInput_RSTICKDOWN,
		DInput_Max,

		Max = DInput_Max,
	};

	static int MouseInput[static_cast<size_t>(EnumInput::Mouse_Max) - static_cast<size_t>(EnumInput::Mouse_Begin)] = {
		MOUSE_INPUT_1,
		MOUSE_INPUT_2,
		MOUSE_INPUT_3,
		MOUSE_INPUT_4,
		MOUSE_INPUT_5,
		MOUSE_INPUT_6,
		MOUSE_INPUT_7,
		MOUSE_INPUT_8,
	};

	static int KeyInput[static_cast<size_t>(EnumInput::Key_Max) - static_cast<size_t>(EnumInput::Key_Begin)] = {
		KEY_INPUT_BACK,
		KEY_INPUT_TAB,
		KEY_INPUT_RETURN,

		KEY_INPUT_LSHIFT,
		KEY_INPUT_RSHIFT,
		KEY_INPUT_LCONTROL,
		KEY_INPUT_RCONTROL,
		KEY_INPUT_ESCAPE,
		KEY_INPUT_SPACE,
		KEY_INPUT_PGUP,
		KEY_INPUT_PGDN,
		KEY_INPUT_END,
		KEY_INPUT_HOME,
		KEY_INPUT_LEFT,
		KEY_INPUT_UP,
		KEY_INPUT_RIGHT,
		KEY_INPUT_DOWN,
		KEY_INPUT_INSERT,
		KEY_INPUT_DELETE,

		KEY_INPUT_MINUS,
		KEY_INPUT_YEN,
		KEY_INPUT_PREVTRACK,
		KEY_INPUT_PERIOD,
		KEY_INPUT_SLASH,
		KEY_INPUT_LALT,
		KEY_INPUT_RALT,
		KEY_INPUT_SCROLL,
		KEY_INPUT_SEMICOLON,
		KEY_INPUT_COLON,
		KEY_INPUT_LBRACKET,
		KEY_INPUT_RBRACKET,
		KEY_INPUT_AT,
		KEY_INPUT_BACKSLASH,
		KEY_INPUT_COMMA,
		KEY_INPUT_KANJI,
		KEY_INPUT_CONVERT,
		KEY_INPUT_NOCONVERT,
		KEY_INPUT_KANA,
		KEY_INPUT_APPS,
		KEY_INPUT_CAPSLOCK,
		KEY_INPUT_SYSRQ,
		KEY_INPUT_PAUSE,
		KEY_INPUT_LWIN,
		KEY_INPUT_RWIN,

		KEY_INPUT_NUMLOCK,
		KEY_INPUT_NUMPAD0,
		KEY_INPUT_NUMPAD1,
		KEY_INPUT_NUMPAD2,
		KEY_INPUT_NUMPAD3,
		KEY_INPUT_NUMPAD4,
		KEY_INPUT_NUMPAD5,
		KEY_INPUT_NUMPAD6,
		KEY_INPUT_NUMPAD7,
		KEY_INPUT_NUMPAD8,
		KEY_INPUT_NUMPAD9,
		KEY_INPUT_MULTIPLY,
		KEY_INPUT_ADD,
		KEY_INPUT_SUBTRACT,
		KEY_INPUT_DECIMAL,
		KEY_INPUT_DIVIDE,
		KEY_INPUT_NUMPADENTER,

		KEY_INPUT_F1,
		KEY_INPUT_F2,
		KEY_INPUT_F3,
		KEY_INPUT_F4,
		KEY_INPUT_F5,
		KEY_INPUT_F6,
		KEY_INPUT_F7,
		KEY_INPUT_F8,
		KEY_INPUT_F9,
		KEY_INPUT_F10,
		KEY_INPUT_F11,
		KEY_INPUT_F12,

		KEY_INPUT_A,
		KEY_INPUT_B,
		KEY_INPUT_C,
		KEY_INPUT_D,
		KEY_INPUT_E,
		KEY_INPUT_F,
		KEY_INPUT_G,
		KEY_INPUT_H,
		KEY_INPUT_I,
		KEY_INPUT_J,
		KEY_INPUT_K,
		KEY_INPUT_L,
		KEY_INPUT_M,
		KEY_INPUT_N,
		KEY_INPUT_O,
		KEY_INPUT_P,
		KEY_INPUT_Q,
		KEY_INPUT_R,
		KEY_INPUT_S,
		KEY_INPUT_T,
		KEY_INPUT_U,
		KEY_INPUT_V,
		KEY_INPUT_W,
		KEY_INPUT_X,
		KEY_INPUT_Y,
		KEY_INPUT_Z,

		KEY_INPUT_0 ,
		KEY_INPUT_1,
		KEY_INPUT_2,
		KEY_INPUT_3,
		KEY_INPUT_4,
		KEY_INPUT_5,
		KEY_INPUT_6,
		KEY_INPUT_7,
		KEY_INPUT_8,
		KEY_INPUT_9,

	};

	static int XInputInput[static_cast<size_t>(EnumInput::XInput_Max) - static_cast<size_t>(EnumInput::XInput_Begin)] = {
		XINPUT_BUTTON_X,			//□
		XINPUT_BUTTON_A,		 	//×
		XINPUT_BUTTON_B,		 	//〇
		XINPUT_BUTTON_Y,     		//△
		XINPUT_BUTTON_LEFT_SHOULDER,
		XINPUT_BUTTON_RIGHT_SHOULDER,
		0xF100, 	//L2
		0xF200, 	//R2
		XINPUT_BUTTON_START,
		XINPUT_BUTTON_LEFT_THUMB,
		XINPUT_BUTTON_RIGHT_THUMB,
		XINPUT_BUTTON_DPAD_LEFT,
		XINPUT_BUTTON_DPAD_UP,
		XINPUT_BUTTON_DPAD_RIGHT,
		XINPUT_BUTTON_DPAD_DOWN,
		XINPUT_BUTTON_BACK,
		0xF001, 	//LSTICK
		0xF002, 	//LSTICK
		0xF004, 	//LSTICK
		0xF008, 	//LSTICK
		0xF011, 	//RSTICK
		0xF012, 	//RSTICK
		0xF014, 	//RSTICK
		0xF018, 	//RSTICK
	};

	static int DInputInput[static_cast<size_t>(EnumInput::DInput_Max) - static_cast<size_t>(EnumInput::DInput_Begin)] = {
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		9,
		10,
		11,
		0xF001,
		0xF002,
		0xF004,
		0xF008,
		8,
		0xF011,
		0xF012,
		0xF014,
		0xF018,
		0xF021,
		0xF022,
		0xF024,
		0xF028,
	};

	static const char* MouseInputStr[static_cast<size_t>(EnumInput::Mouse_Max) - static_cast<size_t>(EnumInput::Mouse_Begin)] = {
		"LMOUSE",
		"RMOUSE",
		"MMOUSE",
		"4",
		"5",
		"6",
		"7",
		"8",
	};

	static const char* KeyInputStr[static_cast<size_t>(EnumInput::Key_Max) - static_cast<size_t>(EnumInput::Key_Begin)] = {
		"BACK",
		"TAB",
		"RETURN",

		"LSHIFT",
		"RSHIFT",
		"LCTRL",
		"RCTRL",
		"ESCAPE",
		"SPACE",
		"PGUP",
		"PGDN",
		"END",
		"HOME",
		"LEFT",
		"UP",
		"RIGHT",
		"DOWN",
		"INSERT",
		"DELETE",

		"-",
		"\\",
		"^",
		".",
		"/",
		"LALT",
		"RALT",
		"SCROLL",
		";",
		":",
		"[",
		"]",
		"@",
		"\\",
		",",
		"KANJI",
		"CONV",
		"NOCONV",
		"KANA",
		"APPS",
		"CAPS",
		"SYSRQ",
		"PAUSE",
		"L WIN",
		"R WIN",

		"NMLOCK",
		"NUM0",
		"NUM1",
		"NUM2",
		"NUM3",
		"NUM4",
		"NUM5",
		"NUM6",
		"NUM7",
		"NUM8",
		"NUM9",
		"NUM*",
		"NUM+",
		"NUM-",
		"NUM.",
		"NUM/",
		"ENTER",

		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",

		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",

		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",

	};

	static const char* XInputInputStr[static_cast<size_t>(EnumInput::XInput_Max) - static_cast<size_t>(EnumInput::XInput_Begin)] = {
		"XX",
		"XA",
		"XB",
		"XY",
		"XL1",
		"XR1",
		"XL2",
		"XR2",
		"XSTART",
		"XL3",
		"XR3",
		"XLEFT",
		"XUP",
		"XRIGHT",
		"XDOWN",
		"XBACK",
		"XLSLEFT",
		"XLSUP",
		"XLSRIGHT",
		"XLSDOWN",
		"XRSLEFT",
		"XRSUP",
		"XRSRIGHT",
		"XRSDOWN",
	};

	static const char* DInputInputStr[static_cast<size_t>(EnumInput::DInput_Max) - static_cast<size_t>(EnumInput::DInput_Begin)] = {
		"D□",
		"D×",
		"D〇",
		"D△",
		"DL1",
		"DR1",
		"DL2",
		"DR2",
		"DSTART",
		"DL3",
		"DR3",
		"DLEFT",
		"DUP",
		"DRIGHT",
		"DDOWN",
		"DBACK",
		"DLSLEFT",
		"DLSUP",
		"DLSRIGHT",
		"DLSDOWN",
		"DRSLEFT",
		"DRSUP",
		"DRSRIGHT",
		"DRSDOWN",
	};

	enum class EnumMenu :size_t {
		Esc,
		Diside,
		Cancel,
		LEFT,
		UP,
		RIGHT,
		DOWN,
		Tab,
		Max,
	};

	enum class EnumBattle :size_t {
		W,
		S,
		A,
		D,
		Q,
		E,
		Run,
		Walk,
		Squat,
		Prone,
		Jump,
		Attack,
		Aim,
		ChangeWeapon,
		Reload,
		Max,
	};
	static const char* BattleStr[static_cast<size_t>(EnumBattle::Max)] = {
		"W",
		"S",
		"A",
		"D",
		"Q",
		"E",
		"Run",
		"Walk",
		"Squat",
		"Prone",
		"Jump",
		"Attack",
		"Aim",
		"ChangeWeapon",
		"Reload",
	};

	class KeyParam : public SingletonBase<KeyParam> {
	private:
		friend class SingletonBase<KeyParam>;
	private:
		class Key {
			int			m_PressTimer{ 0 };
			bool		m_Press{ false };
			bool		m_PrevPress{ false };
			bool		m_IsRepeat{ false };
			char		padding[1]{};
		public:
			void Update(bool PressButton) noexcept {
				this->m_PrevPress = this->m_Press;
				this->m_Press = PressButton;
				if (Press()) {
					++this->m_PressTimer;
					this->m_IsRepeat = Trigger();
					if (this->m_PressTimer > 3) {
						this->m_PressTimer -= 3;
						this->m_IsRepeat = true;
					}
				}
				else {
					this->m_PressTimer = -30;
					this->m_IsRepeat = false;
				}
			}
		public:
			bool Press(void) const noexcept { return this->m_Press; }
			bool Trigger(void) const noexcept { return this->m_Press && !this->m_PrevPress; }
			bool Repeat(void) const noexcept { return this->m_IsRepeat; }
			bool Release(void) const noexcept { return !this->m_Press; }
			bool ReleaseTrigger(void) const noexcept { return !this->m_Press && this->m_PrevPress; }
		};
		struct KeyPair {
			EnumInput	m_ID{ EnumInput::Max };
			EnumInput	m_DefaultID{ EnumInput::Max };
			Key*		m_Ptr{ nullptr };
		};
	private:
		std::vector<Key>																		m_Input{};
		std::array<std::array<std::vector<KeyPair>, 2>, static_cast<size_t>(InputType::Max)>	m_MenuKey{};
		std::array<std::array<std::vector<KeyPair>, 2>, static_cast<size_t>(InputType::Max)>	m_BattleKey{};
		InputType																				m_InputType{ InputType::XInput };
		DxLib::DINPUT_JOYSTATE																	m_inputDInput{};
		DxLib::XINPUT_STATE																		m_inputXInput{};
		bool																					m_DeviceChangeSwitch{ true };
		char		padding[5]{};
	private:
		//コンストラクタ
		KeyParam(void) noexcept {
			this->m_Input.resize(static_cast<size_t>(EnumInput::Max));
			for (auto& mk : this->m_MenuKey) {
				for (auto& k : mk) {
					k.resize(static_cast<size_t>(EnumBattle::Max));
					for (auto& kp : k) {
						kp.m_ID = EnumInput::Max;
						kp.m_DefaultID = EnumInput::Max;
						kp.m_Ptr = nullptr;
					}
				}
			}
			for (auto& bk : this->m_BattleKey) {
				for (auto& k : bk) {
					k.resize(static_cast<size_t>(EnumBattle::Max));
					for (auto& kp : k) {
						kp.m_ID = EnumInput::Max;
						kp.m_DefaultID = EnumInput::Max;
						kp.m_Ptr = nullptr;
					}
				}
			}
			for (int loop = 0; loop < static_cast<int>(InputType::Max); ++loop) {
				this->m_InputType = static_cast<InputType>(loop);
				Load(static_cast<InputType>(loop));

				switch (static_cast<InputType>(loop)) {
				case InputType::KeyBoard:
					AssignID(EnumMenu::Esc, 0, EnumInput::Key_ESCAPE);
					AssignID(EnumMenu::Tab, 0, EnumInput::Key_TAB);
					//
					AssignID(EnumMenu::Diside, 0, EnumInput::Mouse_Left);
					AssignID(EnumMenu::Diside, 1, EnumInput::Key_F);
					AssignID(EnumMenu::Cancel, 0, EnumInput::Mouse_Right);
					AssignID(EnumMenu::Cancel, 1, EnumInput::Key_R);
					AssignID(EnumMenu::LEFT, 0, EnumInput::Key_A);
					AssignID(EnumMenu::UP, 0, EnumInput::Key_W);
					AssignID(EnumMenu::RIGHT, 0, EnumInput::Key_D);
					AssignID(EnumMenu::DOWN, 0, EnumInput::Key_S);
					break;
				case InputType::XInput:
					AssignID(EnumMenu::Esc, 0, EnumInput::XInput_ESCAPE);
					AssignID(EnumMenu::Esc, 0, EnumInput::Key_ESCAPE);
					AssignID(EnumMenu::Tab, 0, EnumInput::XInput_OPTION);
					//
					AssignID(EnumMenu::Diside, 0, EnumInput::XInput_NG);
					AssignID(EnumMenu::Cancel, 0, EnumInput::XInput_OK);
					AssignID(EnumMenu::LEFT, 0, EnumInput::XInput_LEFT);
					AssignID(EnumMenu::LEFT, 1, EnumInput::XInput_LSTICKLEFT);
					AssignID(EnumMenu::UP, 0, EnumInput::XInput_UP);
					AssignID(EnumMenu::UP, 1, EnumInput::XInput_LSTICKUP);
					AssignID(EnumMenu::RIGHT, 0, EnumInput::XInput_RIGHT);
					AssignID(EnumMenu::RIGHT, 1, EnumInput::XInput_LSTICKRIGHT);
					AssignID(EnumMenu::DOWN, 0, EnumInput::XInput_DOWN);
					AssignID(EnumMenu::DOWN, 1, EnumInput::XInput_LSTICKDOWN);
					break;
				case InputType::DInput:
					AssignID(EnumMenu::Esc, 0, EnumInput::DInput_ESCAPE);
					AssignID(EnumMenu::Esc, 0, EnumInput::Key_ESCAPE);
					AssignID(EnumMenu::Tab, 0, EnumInput::DInput_OPTION);
					//
					AssignID(EnumMenu::Diside, 0, EnumInput::DInput_NG);
					AssignID(EnumMenu::Cancel, 0, EnumInput::DInput_OK);
					AssignID(EnumMenu::LEFT, 0, EnumInput::DInput_LEFT);
					AssignID(EnumMenu::LEFT, 1, EnumInput::DInput_LSTICKLEFT);
					AssignID(EnumMenu::UP, 0, EnumInput::DInput_UP);
					AssignID(EnumMenu::UP, 1, EnumInput::DInput_LSTICKUP);
					AssignID(EnumMenu::RIGHT, 0, EnumInput::DInput_RIGHT);
					AssignID(EnumMenu::RIGHT, 1, EnumInput::DInput_LSTICKRIGHT);
					AssignID(EnumMenu::DOWN, 0, EnumInput::DInput_DOWN);
					AssignID(EnumMenu::DOWN, 1, EnumInput::DInput_LSTICKDOWN);
					break;
				case InputType::Max:
				default:
					break;
				}
			}

			this->m_InputType = InputType::KeyBoard;
		}
		KeyParam(const KeyParam&) = delete;
		KeyParam(KeyParam&&) = delete;
		KeyParam& operator=(const KeyParam&) = delete;
		KeyParam& operator=(KeyParam&&) = delete;
		//デストラクタ
		~KeyParam(void) noexcept {
			for (int loop = 0; loop < static_cast<int>(InputType::Max); ++loop) {
				Save(static_cast<InputType>(loop));
			}
			this->m_Input.clear();
			for (auto& mk : this->m_MenuKey) {
				for (auto& k : mk) {
					k.clear();
				}
			}
			for (auto& bk : this->m_BattleKey) {
				for (auto& k : bk) {
					k.clear();
				}
			}
		}
	public:
		void Load(InputType type) noexcept {
			std::string Path;
			std::string DefaultPath;
			//
			switch (type) {
			case InputType::KeyBoard:
				Path = "Save/Input_Keyboard.dat";
				break;
			case InputType::XInput:
				Path = "Save/Input_XInput.dat";
				break;
			case InputType::DInput:
				Path = "Save/Input_DInput.dat";
				break;
			case InputType::Max:
			default:
				break;
			}
			//デフォルトデータ
			switch (type) {
			case InputType::KeyBoard:
				DefaultPath = "data/ProjectSetting/Input_Keyboard.dat";
				break;
			case InputType::XInput:
				DefaultPath = "data/ProjectSetting/Input_XInput.dat";
				break;
			case InputType::DInput:
				DefaultPath = "data/ProjectSetting/Input_DInput.dat";
				break;
			case InputType::Max:
			default:
				break;
			}
			if (!File::IsFileExist(Path.c_str())) {
				Path = DefaultPath;
			}
			//
			{
				File::InputFileStream Istream(DefaultPath.c_str());
				while (!Istream.ComeEof()) {
					std::string Line = File::InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
					std::string Left = File::InputFileStream::getleft(Line, "=");
					std::string Left0 = File::InputFileStream::getleft(Left, ",");
					std::string Left1 = File::InputFileStream::getright(Left, ",");
					std::string Right = File::InputFileStream::getright(Line, "=");
					for (int loop = 0; loop < static_cast<int>(EnumBattle::Max); ++loop) {
						if (Left0 == BattleStr[static_cast<size_t>(loop)]) {
							int ID = std::stoi(Left1);
							EnumInput Enum = EnumInput::Max;
							int Min = static_cast<int>(EnumInput::Begin);
							int Max = static_cast<int>(EnumInput::Max);
							switch (type) {
							case InputType::KeyBoard:
								Min = static_cast<int>(EnumInput::Mouse_Begin);
								Max = static_cast<int>(EnumInput::Key_Max);
								break;
							case InputType::XInput:
								Min = static_cast<int>(EnumInput::XInput_Begin);
								Max = static_cast<int>(EnumInput::XInput_Max);
								break;
							case InputType::DInput:
								Min = static_cast<int>(EnumInput::DInput_Begin);
								Max = static_cast<int>(EnumInput::DInput_Max);
								break;
							case InputType::Max:
							default:
								break;
							}
							for (int loop2 = Min; loop2 < Max; ++loop2) {
								if (Right == GetKeyStr(static_cast<EnumInput>(loop2))) {
									Enum = static_cast<EnumInput>(loop2);
									break;
								}
							}
							//

							DefaultAssignID(static_cast<EnumBattle>(loop), ID, Enum);
							break;
						}
					}
				}
			}
			{
				File::InputFileStream Istream(Path.c_str());
				while (!Istream.ComeEof()) {
					std::string Line = File::InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
					std::string Left = File::InputFileStream::getleft(Line, "=");
					std::string Left0 = File::InputFileStream::getleft(Left, ",");
					std::string Left1 = File::InputFileStream::getright(Left, ",");
					std::string Right = File::InputFileStream::getright(Line, "=");
					for (int loop = 0; loop < static_cast<int>(EnumBattle::Max); ++loop) {
						if (Left0 == BattleStr[static_cast<size_t>(loop)]) {
							int ID = std::stoi(Left1);
							EnumInput Enum = EnumInput::Max;
							int Min = static_cast<int>(EnumInput::Begin);
							int Max = static_cast<int>(EnumInput::Max);
							switch (type) {
							case InputType::KeyBoard:
								Min = static_cast<int>(EnumInput::Mouse_Begin);
								Max = static_cast<int>(EnumInput::Key_Max);
								break;
							case InputType::XInput:
								Min = static_cast<int>(EnumInput::XInput_Begin);
								Max = static_cast<int>(EnumInput::XInput_Max);
								break;
							case InputType::DInput:
								Min = static_cast<int>(EnumInput::DInput_Begin);
								Max = static_cast<int>(EnumInput::DInput_Max);
								break;
							case InputType::Max:
							default:
								break;
							}
							for (int loop2 = Min; loop2 < Max; ++loop2) {
								if (Right == GetKeyStr(static_cast<EnumInput>(loop2))) {
									Enum = static_cast<EnumInput>(loop2);
									break;
								}
							}
							//

							AssignID(static_cast<EnumBattle>(loop), ID, Enum);
							break;
						}
					}
				}
			}
		}
		void Save(InputType type) const noexcept {
			std::string Path;
			switch (type) {
			case InputType::KeyBoard:
				Path = "Save/Input_Keyboard.dat";
				break;
			case InputType::XInput:
				Path = "Save/Input_XInput.dat";
				break;
			case InputType::DInput:
				Path = "Save/Input_DInput.dat";
				break;
			case InputType::Max:
			default:
				break;
			}

			File::OutputFileStream Ostream(Path.c_str());
			for (int loop = 0; loop < static_cast<int>(EnumBattle::Max); ++loop) {
				std::string Left;
				std::string Left0 = BattleStr[static_cast<size_t>(loop)];
				Ostream.AddLine(Left0 + "," + std::to_string(0) + "=" + GetKeyStr(GetKeyAssign(type, static_cast<EnumBattle>(loop), 0)));
				Ostream.AddLine(Left0 + "," + std::to_string(1) + "=" + GetKeyStr(GetKeyAssign(type, static_cast<EnumBattle>(loop), 1)));
			}
		}
	private:
		void Assign(InputType type) noexcept;
	public:
		void DefaultAssignID(EnumMenu Select, int ID, EnumInput Input) noexcept {
			auto& menu = this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select));
			menu.m_DefaultID = Input;
		}
		void DefaultAssignID(EnumBattle Select, int ID, EnumInput Input) noexcept {
			auto& battle = this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select));
			battle.m_DefaultID = Input;
		}
		void AssignID(EnumMenu Select, int ID, EnumInput Input) noexcept {
			auto& menu = this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select));
			menu.m_ID = Input;
			if (Input == EnumInput::Max) {
				menu.m_Ptr = nullptr;
			}
			else {
				menu.m_Ptr = &this->m_Input.at(static_cast<size_t>(menu.m_ID));
			}
		}
		void AssignID(EnumBattle Select, int ID, EnumInput Input) noexcept {
			auto& battle = this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select));
			battle.m_ID = Input;
			if (Input == EnumInput::Max) {
				battle.m_Ptr = nullptr;
			}
			else {
				battle.m_Ptr = &this->m_Input.at(static_cast<size_t>(battle.m_ID));
			}

		}
	public:
		void AssignBattleID(EnumBattle Battle, int ID, EnumInput Input) noexcept {
			//それ以外のアサインで被っている奴を外す
			for (int loop3 = 0; loop3 < static_cast<int>(EnumBattle::Max); ++loop3) {
				EnumBattle OtherBattle = static_cast<EnumBattle>(loop3);
				if (Battle == OtherBattle) { continue; }
				if (Input == GetKeyAssign(OtherBattle, ID)) {
					AssignID(OtherBattle, ID, EnumInput::Max);
					break;
				}
			}
			AssignID(Battle, ID, Input);
		}
	public:
		static const char* GetKeyStr(EnumInput ID) noexcept {
			if (static_cast<size_t>(EnumInput::Mouse_Max) > static_cast<size_t>(ID)) {
				return MouseInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Mouse_Begin)];
			}
			else if (static_cast<size_t>(EnumInput::Key_Max) > static_cast<size_t>(ID)) {
				return KeyInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Key_Begin)];
			}
			else if (static_cast<size_t>(EnumInput::XInput_Max) > static_cast<size_t>(ID)) {
				return XInputInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::XInput_Begin)];
			}
			else if (static_cast<size_t>(EnumInput::DInput_Max) > static_cast<size_t>(ID)) {
				return DInputInputStr[static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::DInput_Begin)];
			}
			return "NONE";
		}
		bool GetKeyPress(EnumInput ID, bool checkDevice) const noexcept {
			if (!DxLib::GetWindowActiveFlag()) {
				return false;
			}
			if (static_cast<size_t>(EnumInput::Mouse_Max) > static_cast<size_t>(ID)) {
				if (checkDevice) {
					if (GetLastInputDevice() != InputType::KeyBoard) {
						return false;
					}
				}
				size_t index = static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Mouse_Begin);
				return (DxLib::GetMouseInput() & MouseInput[index]) != 0;
			}
			else if (static_cast<size_t>(EnumInput::Key_Max) > static_cast<size_t>(ID)) {
				if (checkDevice) {
					if (GetLastInputDevice() != InputType::KeyBoard) {
						return false;
					}
				}
				size_t index = static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::Key_Begin);
				return (DxLib::CheckHitKey(KeyInput[index]) != 0);
			}
			else if (static_cast<size_t>(EnumInput::XInput_Max) > static_cast<size_t>(ID)) {
				if (checkDevice) {
					if (GetLastInputDevice() != InputType::XInput) {
						return false;
					}
				}
				size_t index = static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::XInput_Begin);
				auto XID = XInputInput[index];
				bool IsPress = false;
				if (XID >= 0xF000) {
					bool LS_Active = false;
					bool RS_Active = false;
					float LS_Degree = 0.f;
					float RS_Degree = 0.f;
					{
						int LS_X = this->m_inputXInput.ThumbLX;
						int LS_Y = -this->m_inputXInput.ThumbLY;
						//XInputが-1000~1000になるように変換
						LS_X = 1000 * LS_X / 32768;
						LS_Y = 1000 * LS_Y / 32768;
						//変換により誤差が発生しうるのでデッドゾーンを指定
						if (abs(LS_X) < 10) { LS_X = 0; }
						if (abs(LS_Y) < 10) { LS_Y = 0; }
						if (!(LS_X == 0 && LS_Y == 0)) {
							LS_Active = true;
							LS_Degree = rad2deg(std::atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
						}
					}
					if (false) {
						int RS_X = this->m_inputXInput.ThumbRX;
						int RS_Y = -this->m_inputXInput.ThumbRY;
						//XInputが-1000~1000になるように変換
						RS_X = 1000 * RS_X / 32768;
						RS_Y = 1000 * RS_Y / 32768;
						//変換により誤差が発生しうるのでデッドゾーンを指定
						if (abs(RS_X) < 10) { RS_X = 0; }
						if (abs(RS_Y) < 10) { RS_Y = 0; }
						if (!(RS_X == 0 && RS_Y == 0)) {
							RS_Active = true;
							RS_Degree = rad2deg(std::atan2f(static_cast<float>(RS_X), static_cast<float>(-RS_Y)));
						}
					}
					switch (XID) {
					case 0xF100:
						IsPress = this->m_inputXInput.LeftTrigger > 0;
						break;
					case 0xF200:
						IsPress = this->m_inputXInput.RightTrigger > 0;
						break;
					case 0xF001:
						if (LS_Active) {
							IsPress = (-140.f <= LS_Degree && LS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF002:
						if (LS_Active) {
							IsPress = (-50.f <= LS_Degree && LS_Degree <= 50.f);//UP
						}
						break;
					case 0xF004:
						if (LS_Active) {
							IsPress = (40.f <= LS_Degree && LS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF008:
						if (LS_Active) {
							IsPress = (130.f <= LS_Degree || LS_Degree <= -130.f);//DOWN
						}
						break;
					case 0xF011:
						if (RS_Active) {
							IsPress = (-140.f <= RS_Degree && RS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF012:
						if (RS_Active) {
							IsPress = (-50.f <= RS_Degree && RS_Degree <= 50.f);//UP
						}
						break;
					case 0xF014:
						if (RS_Active) {
							IsPress = (40.f <= RS_Degree && RS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF018:
						if (RS_Active) {
							IsPress = (130.f <= RS_Degree || RS_Degree <= -130.f);//DOWN
						}
						break;
					default:
						break;
					}
				}
				else if (0 <= XID && XID < 16) {
					IsPress = (this->m_inputXInput.Buttons[XID] != 0);
				}
				return IsPress;
			}
			else if (static_cast<size_t>(EnumInput::DInput_Max) > static_cast<size_t>(ID)) {
				if (checkDevice) {
					if (GetLastInputDevice() != InputType::DInput) {
						return false;
					}
				}
				size_t index = static_cast<size_t>(ID) - static_cast<size_t>(EnumInput::DInput_Begin);
				auto DID = DInputInput[index];
				bool IsPress = false;
				if (DID >= 0xF000) {
					bool LPOV_Active = false;
					bool LS_Active = false;
					bool RS_Active = false;
					float LPOV_Degree = 0.f;
					float LS_Degree = 0.f;
					float RS_Degree = 0.f;
					{
						int LS_X = this->m_inputDInput.X;
						int LS_Y = this->m_inputDInput.Y;
						//XInputが-1000~1000になるように変換
						LS_X = 1000 * LS_X / 32768;
						LS_Y = 1000 * LS_Y / 32768;
						//変換により誤差が発生しうるのでデッドゾーンを指定
						if (abs(LS_X) < 10) { LS_X = 0; }
						if (abs(LS_Y) < 10) { LS_Y = 0; }
						if (!(LS_X == 0 && LS_Y == 0)) {
							LS_Active = true;
							LS_Degree = rad2deg(std::atan2f(static_cast<float>(LS_X), static_cast<float>(-LS_Y)));
						}
					}
					if (false) {
						int RS_X = this->m_inputDInput.Z;
						int RS_Y = this->m_inputDInput.Rz;
						//XInputが-1000~1000になるように変換
						RS_X = 1000 * RS_X / 32768;
						RS_Y = 1000 * RS_Y / 32768;
						//変換により誤差が発生しうるのでデッドゾーンを指定
						if (abs(RS_X) < 10) { RS_X = 0; }
						if (abs(RS_Y) < 10) { RS_Y = 0; }
						if (!(RS_X == 0 && RS_Y == 0)) {
							RS_Active = true;
							RS_Degree = rad2deg(std::atan2f(static_cast<float>(RS_X), static_cast<float>(-RS_Y)));
						}
					}
					{
						if (this->m_inputDInput.POV[0] != 0xffffffff) {
							LPOV_Active = true;
							LPOV_Degree = static_cast<float>(this->m_inputDInput.POV[0]) / 100.f;
						}
					}
					switch (DID) {
					case 0xF001:
						if (LPOV_Active) {
							IsPress = (220.f <= LPOV_Degree && LPOV_Degree <= 320.f);
						}
						break;
					case 0xF002:
						if (LPOV_Active) {
							IsPress = (310.f <= LPOV_Degree || LPOV_Degree <= 50.f);
						}
						break;
					case 0xF004:
						if (LPOV_Active) {
							IsPress = (40.f <= LPOV_Degree && LPOV_Degree <= 140.f);
						}
						break;
					case 0xF008:
						if (LPOV_Active) {
							IsPress = (130.f <= LPOV_Degree && LPOV_Degree <= 230.f);
						}
						break;
					case 0xF011:
						if (LS_Active) {
							IsPress = (-140.f <= LS_Degree && LS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF012:
						if (LS_Active) {
							IsPress = (-50.f <= LS_Degree && LS_Degree <= 50.f);//UP
						}
						break;
					case 0xF014:
						if (LS_Active) {
							IsPress = (40.f <= LS_Degree && LS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF018:
						if (LS_Active) {
							IsPress = (130.f <= LS_Degree || LS_Degree <= -130.f);//DOWN
						}
						break;
					case 0xF021:
						if (RS_Active) {
							IsPress = (-140.f <= RS_Degree && RS_Degree <= -40.f);//LEFT
						}
						break;
					case 0xF022:
						if (RS_Active) {
							IsPress = (-50.f <= RS_Degree && RS_Degree <= 50.f);//UP
						}
						break;
					case 0xF024:
						if (RS_Active) {
							IsPress = (40.f <= RS_Degree && RS_Degree <= 140.f);//RIGHT
						}
						break;
					case 0xF028:
						if (RS_Active) {
							IsPress = (130.f <= RS_Degree || RS_Degree <= -130.f);//DOWN
						}
						break;
					default:
						break;
					}
				}
				else if (0 <= DID && DID < 16) {
					IsPress = (this->m_inputDInput.Buttons[DID] != 0);
				}
				return IsPress;
			}
			return false;
		}
		const EnumInput& GetKeyAssign(InputType type, EnumMenu Select, int ID) const noexcept { return this->m_MenuKey.at(static_cast<size_t>(type)).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select)).m_ID; }
		const EnumInput& GetKeyAssign(InputType type, EnumBattle Select, int ID) const noexcept { return this->m_BattleKey.at(static_cast<size_t>(type)).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select)).m_ID; }

		const EnumInput& GetKeyAssign(EnumMenu Select, int ID) const noexcept { return GetKeyAssign(GetLastInputDevice(), Select, ID); }
		const EnumInput& GetKeyAssign(EnumBattle Select, int ID) const noexcept { return GetKeyAssign(GetLastInputDevice(), Select, ID); }
		const EnumInput& GetDefaultKeyAssign(EnumMenu Select, int ID) const noexcept { return this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select)).m_DefaultID; }
		const EnumInput& GetDefaultKeyAssign(EnumBattle Select, int ID) const noexcept { return this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice())).at(static_cast<size_t>(ID)).at(static_cast<size_t>(Select)).m_DefaultID; }
	public:
		//入力取得
		bool GetMenuKeyTrigger(EnumMenu Select) const noexcept {
			for (const auto& mk : this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = mk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Trigger()) {
					return  true;
				}
			}
			return false;
		}
		bool GetMenuKeyPress(EnumMenu Select) const noexcept {
			for (const auto& mk : this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = mk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Press()) {
					return  true;
				}
			}
			return false;
		}
		bool GetMenuKeyRepeat(EnumMenu Select) const noexcept {
			for (const auto& mk : this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = mk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Repeat()) {
					return  true;
				}
			}
			return false;
		}
		bool GetMenuKeyRelease(EnumMenu Select) const noexcept {
			for (const auto& mk : this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = mk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Release()) {
					return  true;
				}
			}
			return false;
		}
		bool GetMenuKeyReleaseTrigger(EnumMenu Select) const noexcept {
			for (const auto& mk : this->m_MenuKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = mk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->ReleaseTrigger()) {
					return  true;
				}
			}
			return false;
		}
		bool GetBattleKeyTrigger(EnumBattle Select) const noexcept {
			for (const auto& bk : this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = bk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Trigger()) {
					return  true;
				}
			}
			return false;
		}
		bool GetBattleKeyPress(EnumBattle Select) const noexcept {
			for (const auto& bk : this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = bk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Press()) {
					return  true;
				}
			}
			return false;
		}
		bool GetBattleKeyRepeat(EnumBattle Select) const noexcept {
			for (const auto& bk : this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = bk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Repeat()) {
					return  true;
				}
			}
			return false;
		}
		bool GetBattleKeyRelease(EnumBattle Select) const noexcept {
			for (const auto& bk : this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = bk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->Release()) {
					return  true;
				}
			}
			return false;
		}
		bool GetBattleKeyReleaseTrigger(EnumBattle Select) const noexcept {
			for (const auto& bk : this->m_BattleKey.at(static_cast<size_t>(GetLastInputDevice()))) {
				const auto& m = bk.at(static_cast<size_t>(Select));
				if (m.m_Ptr && m.m_Ptr->ReleaseTrigger()) {
					return  true;
				}
			}
			return false;
		}
		//最後に入力したデバイス
		InputType GetLastInputDevice(void) const noexcept { return this->m_InputType; }
		bool IsDeviceChange(void) const noexcept { return this->m_DeviceChangeSwitch; }
	public:
		//毎フレーム更新する内容
		void Update(void) noexcept;
	};
}
