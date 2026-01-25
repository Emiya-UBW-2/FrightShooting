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

class MainUI {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsExit{ false };
	char		padding[5]{};
	Sound::SoundUniqueID			m_OKID{ InvalidID };
public:
	MainUI(void) noexcept {}
	MainUI(const MainUI&) = delete;
	MainUI(MainUI&&) = delete;
	MainUI& operator=(const MainUI&) = delete;
	MainUI& operator=(MainUI&&) = delete;
	virtual ~MainUI(void) noexcept {}
public:
	bool IsPauseActive(void) const noexcept { return m_IsPauseActive; }
	bool IsExit(void) const noexcept { return m_IsExit; }
public:
	void Init() noexcept {
		this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

		this->m_IsSceneEnd = false;
		this->m_IsPauseActive = false;
		this->m_IsExit = false;

		this->m_OptionWindow.Init();
		this->m_PauseUI.Init();

		this->m_PauseUI.SetEvent(0, [this]() {
			this->m_IsSceneEnd = true;
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});
		this->m_PauseUI.SetEvent(1, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_PauseUI.SetEvent(2, [this]() {
			this->m_IsPauseActive = false;
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->SetGuideFlip();
			});
	}
	void Update() noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
			this->m_IsPauseActive ^= 1;
			KeyGuideParts->SetGuideFlip();
		}
		this->m_PauseUI.SetActive(this->m_IsPauseActive && !this->m_OptionWindow.IsActive());
		if (!this->m_IsPauseActive) {
			this->m_OptionWindow.SetActive(false);
		}
		this->m_PauseUI.Update();
		if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
			this->m_IsExit = true;
		}
		this->m_OptionWindow.Update();
	}
	void Draw() noexcept {
		this->m_PauseUI.Draw();
		this->m_OptionWindow.Draw();
	}
	void Dispose() noexcept {
		this->m_PauseUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
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
		auto& Watch = ((std::shared_ptr<PlaneCommon>&)PlayerManager::Instance()->SetPlane().at(0));

		auto* DrawerMngr = Draw::MainDraw::Instance();
		{
			auto Pos2D = ConvWorldPosToScreenPos((Watch->GetMat().pos() + Watch->GetMat().zvec2() * (25.f * Scale3DRate)).get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				this->m_AimPoint2D_Near.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_AimPoint2D_Near.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
			}
		}
		{
			auto Pos2D = ConvWorldPosToScreenPos((Watch->GetMat().pos() + Watch->GetMat().zvec2() * (50.f * Scale3DRate)).get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				this->m_AimPoint2D_Far.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_AimPoint2D_Far.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
			}
		}
	}
	void Draw() noexcept {
		SetDrawBright(0, 128, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Far.x), static_cast<int>(this->m_AimPoint2D_Far.y), 0.5f, 0.f, true);
		SetDrawBright(0, 255, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Near.x), static_cast<int>(this->m_AimPoint2D_Near.y), 1.f, 0.f, true);
		SetDrawBright(255, 255, 255);
	}
};

class MainScene : public Util::SceneBase {
	std::unique_ptr<MainUI>			m_MainUI{};
	std::unique_ptr<AimPoint>		m_AimPoint{};
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	Util::VECTOR3D					CamPosition;
	Util::VECTOR3D					CamTarget;
	Util::VECTOR3D					CamUp;
	float							m_Fade{ 1.f };
	bool							m_Exit{ false };
	char		padding[7]{};
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
