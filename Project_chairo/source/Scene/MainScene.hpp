#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../MainScene/Script.hpp"
#include "../MainScene/GameRule.hpp"
#include "../UI/MainUI.hpp"

class AimPoint {
	const Draw::GraphHandle* m_Cursor{};
	Util::VECTOR2D					m_AimPoint2D_Near;
	Util::VECTOR2D					m_AimPoint2D_Far;
public:
	AimPoint(void) noexcept {}
	AimPoint(const AimPoint&) = delete;
	AimPoint(AimPoint&&) = delete;
	AimPoint& operator=(const AimPoint&) = delete;
	AimPoint& operator=(AimPoint&&) = delete;
	virtual ~AimPoint(void) noexcept {}
public:
	void Load() noexcept {
		m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	}
	void CalcPoint() noexcept {
		auto& Watch = PlayerManager::Instance()->SetPlane();

		auto Mat = Watch->GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1));
		{
			auto Pos2D = ConvWorldPosToScreenPos((Mat.pos() + Mat.zvec2() * (25.f * Scale3DRate)).get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				this->m_AimPoint2D_Near.x = Pos2D.x;
				this->m_AimPoint2D_Near.y = Pos2D.y;
			}
		}
		{
			auto Pos2D = ConvWorldPosToScreenPos((Mat.pos() + Mat.zvec2() * (50.f * Scale3DRate)).get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				this->m_AimPoint2D_Far.x = Pos2D.x;
				this->m_AimPoint2D_Far.y = Pos2D.y;
			}
		}
	}
	void Draw() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetDrawBright(0, 128, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Far.x), static_cast<int>(this->m_AimPoint2D_Far.y),
			0.5f / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())), 0.f, true);
		SetDrawBright(0, 255, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Near.x), static_cast<int>(this->m_AimPoint2D_Near.y),
			1.f / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())), 0.f, true);
		SetDrawBright(255, 255, 255);
	}
};

class MainScene : public Util::SceneBase {
	const Draw::GraphHandle* m_Cursor{};
	const Draw::GraphHandle* m_Damage{};

	StageScript						m_StageScript;

	std::unique_ptr<MainUI>			m_MainUI{};
	std::unique_ptr<AimPoint>		m_AimPoint{};
	Sound::SoundUniqueID			m_EnviID{ InvalidID };

	Sound::SoundUniqueID			m_BGMID{ InvalidID };

	Util::Matrix3x3					EyeMatR;
	Util::VECTOR3D					CamPosition;
	Util::VECTOR3D					CamTarget;
	Util::VECTOR3D					CamUp;
	Util::VECTOR3D					CamUpR;
	float							m_Fade{ 1.f };
	float							m_FadeStage{ 1.f };
	float							m_ManeuverFovPer{ 1.f };
	float							m_ManeuverActive{ 0.f };
	Util::VECTOR3D					m_ManeuverPos2D{};
	char		padding2[4]{};
	bool							m_Exit{ false };
	bool							m_NextStage{ false };
	bool							m_NextEvent{ false };
	char		padding[1]{};

	float							m_DamagePer{ 0.f };
	float							m_DamageWatch{ 0.f };
	std::string						m_NowStage{};

	Sound::SoundUniqueID HitHumanID{ InvalidID };
	Sound::SoundUniqueID alert{ InvalidID };
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
