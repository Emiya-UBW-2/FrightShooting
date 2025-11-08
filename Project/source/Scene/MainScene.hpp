#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"
#include "../MainScene/PlayerManager.hpp"
#include "../MainScene/Gun.hpp"

class EquipUI {
	const std::shared_ptr<Gun>*	m_Gun{};
public:
	//コンストラクタ
	EquipUI(void) noexcept {}
	EquipUI(const EquipUI&) = delete;
	EquipUI(EquipUI&& o) noexcept {
		this->m_Gun = o.m_Gun;
		o.m_Gun = nullptr;
	}
	EquipUI& operator=(const EquipUI&) = delete;
	EquipUI& operator=(EquipUI&& o) noexcept {
		this->m_Gun = o.m_Gun;
		o.m_Gun = nullptr;
		return *this;
	}
	//デストラクタ
	~EquipUI(void) noexcept {}
public:
	void Init(const std::shared_ptr<Gun>* pGraph) noexcept {
		this->m_Gun = pGraph;
	}
	void Draw(int xpos, int ypos) noexcept {
		DxLib::DrawBox(xpos, ypos, xpos + 256, ypos + 128, GetColor(0, 0, 0), true);
		DxLib::SetDrawBright(0, 255, 0);
		(*this->m_Gun)->DrawPic(xpos, ypos, xpos + 256, ypos + 128, true);
		DxLib::SetDrawBright(255, 255, 255);
		DxLib::DrawBox(xpos, ypos, xpos + 256, ypos + 128, GetColor(0, 255, 0), false, 3);

		int xsize = std::min(((xpos + 256 - 5) - (xpos + 5)) / (*this->m_Gun)->GetAmmoTotal(), 10);
		for (int loop = 0; loop < (*this->m_Gun)->GetAmmoTotal(); ++loop) {
			DxLib::DrawBox(
				(xpos + 256 - 5) - (xsize * (loop + 1)) + 1, ypos + 128 - 5 - 24,
				(xpos + 256 - 5) - (xsize * (loop + 0)) - 1, ypos + 128 - 5, GetColor(128, 128, 128), true);
		}
		for (int loop = 0; loop < (*this->m_Gun)->GetAmmoNum(); ++loop) {
			DxLib::DrawBox(
				(xpos + 256 - 5) - (xsize * (loop + 1)) + 1, ypos + 128 - 5 - 24,
				(xpos + 256 - 5) - (xsize * (loop + 0)) - 1, ypos + 128 - 5, (loop == 0) ? GetColor(255, 0, 0) : GetColor(0, 255, 0), true);
		}
	}
};

class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	InfoType						m_EntrancePoint{ InfoType::Entrance1 };
	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsChangeEquip{ false };
	char		padding[4]{};
	std::shared_ptr<Gun>			m_HandGun{};
	std::shared_ptr<Gun>			m_MainGun{};
	std::shared_ptr<Suppressor>		m_HandGunAttach{};
	std::shared_ptr<Scope>			m_MainGunAttach{};
	float							m_CharaStyleChange{};
	float							m_CharaStyleChangeR{};
	CharaStyle						m_CharaStyle{};
	char		padding2[4]{};
	const Draw::GraphHandle*		m_StandGraph{};
	const Draw::GraphHandle*		m_SquatGraph{};
	const Draw::GraphHandle*		m_ProneGraph{};
	const Draw::GraphHandle*		m_Watch{};
	const Draw::GraphHandle*		m_Cursor{};
	const Draw::GraphHandle*		m_Lock{};
	Util::VECTOR3D					m_CamOffset{};
	Util::VECTOR3D					m_CamVec{};
	std::string						m_MapName{ "Map1" };
	float							m_FPSPer{ 0.f };
	float							m_CamCheckLen{};
	float							m_CamCheckTimer{};
	float							m_Fade{ 1.f };
	float							m_ShotFov{ 0.f };
	char		padding3[4]{};
	Sound::SoundUniqueID			m_cursorID{ InvalidID };
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	std::vector<EquipUI>			m_EquipUI;
	float							m_EquipUITimer{};
	float							m_EquipUIActivePer{};
	int								m_EquipID{ 0 };
	float							m_EquipPer{};
	float							m_AutoAimActive{};
	float							m_AimRotate{};
	bool							m_UseLens{ false };
	char		padding4[3]{};
	Util::VECTOR2D					m_LensPos{};
	Util::VECTOR2D					m_LensSize{};
	char		padding5[4]{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override;
	void SetShadowDrawRigid_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) noexcept override;
	void Draw_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override;
	void ShadowDrawFar_Sub(void) noexcept override;
	void ShadowDraw_Sub(void) noexcept override;
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
