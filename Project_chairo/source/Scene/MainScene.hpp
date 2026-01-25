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
	bool							m_IsResetMouse{ false };
	char		padding[3]{};
	float							m_CharaStyleChange{};
	float							m_CharaStyleChangeR{};
	const Draw::GraphHandle*		m_Cursor{};
	const Draw::GraphHandle*		m_Lock{};
	const Draw::GraphHandle*		m_Speed{};
	const Draw::GraphHandle*		m_Meter{};
	const Draw::GraphHandle*		m_Damage{};
	Util::VECTOR3D					m_CamOffset{};
	Util::VECTOR3D					m_CamVec{};
	float							m_CamCheckLen{};
	float							m_CamCheckTimer{};
	float							m_Fade{ 1.f };
	float							m_DamagePer{ 0.f };
	float							m_DamageWatch{ 0.f };
	float							m_SpeedPer{ 0.f };
	size_t							m_AttackNow{};
	Sound::SoundUniqueID			m_cursorID{ InvalidID };
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	bool							m_AimPointDraw{ false };
	char		padding4[3]{};
	Util::VECTOR2D					m_LensPos{};
	Util::VECTOR2D					m_LensSize{};
	Util::VECTOR2D					m_AimPoint2D_Near;
	Util::VECTOR2D					m_AimPoint2D_Far;
	char		padding5[4]{};

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;
	Util::VECTOR3D CamUp;
	char		padding3[4]{};
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
	void DrawFront_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override;
	void ShadowDrawFar_Sub(void) noexcept override;
	void ShadowDraw_Sub(void) noexcept override;
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
