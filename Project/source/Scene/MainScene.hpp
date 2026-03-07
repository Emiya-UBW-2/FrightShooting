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

class Main3DUI {
	const Draw::GraphHandle*		m_Cursor{};
	const Draw::GraphHandle*		m_Lock{};
	bool							m_AimPointDraw{ false };
	char		padding[3]{};
	Util::VECTOR2D					m_AimPoint2D;
	std::array<float, 5>				m_AtackPer{};
	size_t							m_AttackNow{};
public:
	void Load() noexcept {
		m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
		m_Lock = Draw::GraphPool::Instance()->Get("data/Image/Lock.png")->Get();
	}
	void Update() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		for (auto& a : m_AtackPer) {
			a = std::max(a - DrawerMngr->GetDeltaTime(), 0.f);
		}
		for (auto& c : PlayerManager::Instance()->SetPlane()) {
			if (c->GetDamageID() == 0) {
				m_AtackPer.at(m_AttackNow) = 1.f;
				++m_AttackNow %= m_AtackPer.size();
			}
		}
	}
	void Draw() noexcept {
		this->m_AimPointDraw = false;

		auto* DrawerMngr = Draw::MainDraw::Instance();
		auto& Watch = PlayerManager::Instance()->SetPlane().at(0);

		auto Pos2D = ConvWorldPosToScreenPos((Watch->GetMat().pos() + Watch->GetMat().zvec2() * (100.f * Scale3DRate)).get());
		if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
			this->m_AimPointDraw = true;
			this->m_AimPoint2D.x = Pos2D.x;
			this->m_AimPoint2D.y = Pos2D.y;
		}

		if (this->m_AimPointDraw) {
			SetDrawBright(0, 255, 0);
			m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D.x), static_cast<int>(this->m_AimPoint2D.y),
				1.f / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())),
				0.f, true);
			for (auto& a : m_AtackPer) {
				if (a > 0.f) {
					SetDrawBright(255, 0, 0);
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * (1.f - std::fabsf(a - 0.7f) / (1.f - 0.7f))), 0, 255));
					m_Lock->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D.x), static_cast<int>(this->m_AimPoint2D.y),
						(1.f + std::powf(a, 2.f)) / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())),
						0.f, true);
					DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
			SetDrawBright(255, 255, 255);
		}
	}
};

class MainUI {
	const Draw::GraphHandle*		m_Alt{};
	const Draw::GraphHandle*		m_Speed{};
	const Draw::GraphHandle*		m_Meter{};
	const Draw::GraphHandle*		m_Damage{};
	const Draw::GraphHandle*		m_NRad{};
	const Draw::GraphHandle*		m_EnemyRad{};

	float							m_DamageWatch{ 0.f };
	float							m_SpeedPer{ 0.f };
	float							m_AltPer{ 0.f };
	char		padding[4]{};
public:
	void DamageUIActive() noexcept {
		m_DamageWatch = 2.f;
	}
public:
	void Load() noexcept {
		m_Alt = Draw::GraphPool::Instance()->Get("data/Image/alt.png")->Get();
		m_Speed = Draw::GraphPool::Instance()->Get("data/Image/speed.png")->Get();
		m_Meter = Draw::GraphPool::Instance()->Get("data/Image/meter.png")->Get();
		m_Damage = Draw::GraphPool::Instance()->Get("data/Image/damage.png")->Get();
		m_NRad = Draw::GraphPool::Instance()->Get("data/Image/Nrad.png")->Get();
		m_EnemyRad = Draw::GraphPool::Instance()->Get("data/Image/enemyrad.png")->Get();
	}
	void Update() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		auto& Watch = (PlayerManager::Instance()->SetPlane().at(0));
		m_DamageWatch = std::max(m_DamageWatch - DrawerMngr->GetDeltaTime(), 1.f - Watch->GetHitPointPer());
	}
	void Draw() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		auto* Localize = Util::LocalizePool::Instance();
		auto* CameraParts = Camera::Camera3D::Instance();

		auto& Watch = PlayerManager::Instance()->SetPlane().at(0);

		if (std::clamp(static_cast<int>(255.f * m_DamageWatch * 0.5f), 0, 255) > 10) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_DamageWatch * 0.5f), 0, 255));
			m_Damage->DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		{
			float speed = Watch->GetSpeed() / (1.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f);

			Util::Easing(&this->m_SpeedPer, 90.f + speed * 3.f + GetRandf(3.f), 0.9f);

			int X = DrawerMngr->GetDispWidth() / 2 + 765 + static_cast<int>(CameraParts->GetShake().x * 10.f), Y = DrawerMngr->GetDispHeight() - 128 - 64 + static_cast<int>(CameraParts->GetShake().y * 10.f);
			m_Speed->DrawRotaGraph(X, Y, 1.0f, 0.f, true);
			m_Meter->DrawRotaGraph(X, Y, 1.0f, Util::deg2rad(this->m_SpeedPer), true);
		}
		{
			float alt = Watch->GetMat().pos().y / Scale3DRate;

			Util::Easing(&this->m_AltPer, -alt / 500.f * 90.f + GetRandf(3.f), 0.9f);

			int X = DrawerMngr->GetDispWidth() / 2 - 765 + static_cast<int>(CameraParts->GetShake().z * 10.f), Y = DrawerMngr->GetDispHeight() - 128 - 64 + static_cast<int>(CameraParts->GetShake().y * 10.f);
			m_Alt->DrawRotaGraph(X, Y, 1.0f, 0.f, true);
			m_Meter->DrawRotaGraph(X, Y, 1.0f, Util::deg2rad(this->m_AltPer), true);
		}
		{
			int xpos = DrawerMngr->GetDispWidth() / 2;
			int ypos = DrawerMngr->GetDispHeight() * 3 / 4;
			if (false) {
				KeyGuideParts->DrawButton(xpos - 24 / 2, ypos - 24 / 2, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload));
				Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
					Draw::FontXCenter::MIDDLE, Draw::FontYCenter::TOP,
					xpos, ypos + 18,
					ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(316)));
				ypos += 52;
			}
		}
		{
			int X = DrawerMngr->GetDispWidth() / 2, Y = DrawerMngr->GetDispHeight() / 2;
			auto Vec1 = CameraParts->GetCamera().GetCamVec() - CameraParts->GetCamera().GetCamPos(); Vec1.y = 0.f; Vec1 = Vec1.normalized();
			float Rad = std::atan2f(Vec1.x, -Vec1.z);

			for (int loop = 0; loop < 8; ++loop) {
				int X2 = X + std::sin(Util::deg2rad(loop * 360 / 8)) * (200.f + 6.f), Y2 = Y + std::cos(Util::deg2rad(loop * 360 / 8)) * (200.f + 6.f);
				int X3 = X + std::sin(Util::deg2rad(loop * 360 / 8)) * (200.f + 12.f), Y3 = Y + std::cos(Util::deg2rad(loop * 360 / 8)) * (200.f + 12.f);
				DrawLine(X2, Y2, X3, Y3, ColorPalette::Black, 3);
			}

			for (int loop = 0; loop < 36; ++loop) {
				int X2 = X + std::sin(-Rad + Util::deg2rad(loop * 360 / 36)) * (200.f - 6.f), Y2 = Y + std::cos(-Rad + Util::deg2rad(loop * 360 / 36)) * (200.f - 6.f);
				int X3 = X + std::sin(-Rad + Util::deg2rad(loop * 360 / 36)) * (200.f - 12.f), Y3 = Y + std::cos(-Rad + Util::deg2rad(loop * 360 / 36)) * (200.f - 12.f);
				DrawLine(X2, Y2, X3, Y3, ColorPalette::Black, 3);
			}
			m_NRad->DrawRotaGraph(X, Y, 1.f, Rad, true);

			for (auto& c : PlayerManager::Instance()->SetTarget()) {
				auto Vec2 = c->GetMat().pos() - Watch->GetMat().pos(); Vec2.y = 0.f; Vec2 = Vec2.normalized();
				m_EnemyRad->DrawRotaGraph(X, Y, 1.f, Rad - std::atan2f(Vec2.x, -Vec2.z), true);
			}
		}
		{
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, 32, 3)->DrawString(
				Draw::FontXCenter::LEFT, Draw::FontYCenter::TOP,
				18, 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8("TIME  : %d:%05.2f"),
				static_cast<int>(PlayerManager::Instance()->GetTime() / 60.f),
				PlayerManager::Instance()->GetTime() - static_cast<int>(PlayerManager::Instance()->GetTime() / 60.f) * 60.f);

			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, 32, 3)->DrawString(
				Draw::FontXCenter::LEFT, Draw::FontYCenter::TOP,
				18, 18 + 32 + 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8("SCORE : %d"), PlayerManager::Instance()->GetScore());
		}
	}
};

class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_IsRetire{ false };
	bool							m_Exit{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsChangeEquip{ false };
	bool							m_IsResetMouse{ false };
	char		padding[3]{};
	float							m_CharaStyleChange{};
	float							m_CharaStyleChangeR{};
	Util::VECTOR3D					m_CamOffset{};
	Util::VECTOR3D					m_CamVec{};
	float							m_FPSPer{ 0.f };
	Util::Matrix4x4					m_EyeRotFree;
	float							m_CamCheckLen{};
	float							m_CamCheckTimer{};
	float							m_Fade{ 1.f };
	float							m_ShotFov{ 0.f };
	float							m_DamagePer{ 0.f };
	Sound::SoundUniqueID			m_cursorID{ InvalidID };
	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };

	Sound::SoundUniqueID			m_BGMID{ InvalidID };
	Util::VECTOR2D					m_LensPos{};
	Util::VECTOR2D					m_LensSize{};
	char		padding5[4]{};

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;
	Util::VECTOR3D CamUp;

	MainUI							m_MainUI;
	Main3DUI						m_Main3DUI;
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
