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


class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsChangeEquip{ false };
	char		padding[4]{};
	float							m_CharaStyleChange{};
	float							m_CharaStyleChangeR{};
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
