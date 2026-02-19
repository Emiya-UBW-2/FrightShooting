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
		auto& Watch = PlayerManager::Instance()->SetPlane();

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

struct EnemyMove {
	int				m_Frame{};
	Util::VECTOR3D	m_Pos{};
	Util::Matrix3x3	m_Rot{};
};
enum class AmmoMoveType : size_t {
	Fixed,
	Target,
	Homing,
};
struct EnemyAmmo {
	int				m_Frame{};
	Util::Matrix3x3	m_Rot{};
	AmmoMoveType	m_AmmoMoveType{};
};
class EnemyScript {
	int						m_EnemyID{};
	int						m_HP{};
	std::vector<EnemyMove>	m_EnemyMove;
	std::vector<EnemyAmmo>	m_EnemyAmmo;

	float					m_Frame{};
	bool					m_IsActive{ false };
	bool					m_IsDown{ false };
public:
	auto& EnemyObj(void) noexcept {
		return PlayerManager::Instance()->SetEnemy().at(m_EnemyID);
	}
	void SetActive(void) noexcept {
		m_EnemyID = PlayerManager::Instance()->SetEnemy().size();
		PlayerManager::Instance()->SetEnemy().emplace_back();
		PlayerManager::Instance()->SetEnemy().at(m_EnemyID) = std::make_shared<Enemy>();
		ObjectManager::Instance()->InitObject(EnemyObj(), EnemyObj(), "data/model/Plane/");
		EnemyObj()->SetPos(Util::VECTOR3D::vget(5.f, 15.f, 0.f) * Scale3DRate, Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(0)));
		m_IsActive = true;
	}
	bool IsActive(void) const noexcept {
		return m_IsActive;
	}
	void SetDown(void) noexcept {
		m_IsDown = true;
	}
public:
	void Init(std::string Path) noexcept {
		//
		{
			File::InputFileStream FileStream;
			FileStream.Open("data/Enemy/" + Path + "/Data.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "HitPoint") {
						m_HP = std::stoi(Args.at(1));
					}
				}
			}
			FileStream.Close();
		}
		//
		{
			m_EnemyMove.clear();
			File::InputFileStream FileStream;
			FileStream.Open("data/Enemy/" + Path + "/Move.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "SetPoint") {
						m_EnemyMove.emplace_back();
						m_EnemyMove.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyMove.back().m_Pos = Util::VECTOR3D::vget(std::stof(Args.at(2)), std::stof(Args.at(3)), std::stof(Args.at(4))) * Scale3DRate;
						m_EnemyMove.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(std::stof(Args.at(7)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(6)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(5))));
					}
					if (Args.at(0) == "PutFixedAmmo") {
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(3)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(2))));
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Fixed;
					}
					if (Args.at(0) == "PutTargetAmmo") {
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
					}
					if (Args.at(0) == "PutHomingAmmo") {
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(3)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(2))));
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
					}
				}
			}
			FileStream.Close();
		}

		m_Frame = 0.f;
	}
	void Update() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (!m_IsDown) {
			for (int loop = 1; loop < m_EnemyMove.size(); ++loop) {
				if (m_EnemyMove.at(loop - 1).m_Frame <= m_Frame && m_Frame <= m_EnemyMove.at(loop).m_Frame) {
					float Per = (m_Frame - m_EnemyMove.at(loop - 1).m_Frame) / (m_EnemyMove.at(loop).m_Frame - m_EnemyMove.at(loop - 1).m_Frame);
					Util::VECTOR3D Pos = Util::Lerp(m_EnemyMove.at(loop - 1).m_Pos, m_EnemyMove.at(loop).m_Pos, Per);
					Util::Matrix3x3 Rot = Util::Lerp(m_EnemyMove.at(loop - 1).m_Rot, m_EnemyMove.at(loop).m_Rot, Per);

					EnemyObj()->SetPos(Pos, Rot);
					break;
				}
			}

			for (int loop = 0; loop < m_EnemyAmmo.size(); ++loop) {
				if (std::fabsf(m_Frame - static_cast<float>(m_EnemyAmmo.at(loop).m_Frame)) < 1.f) {//todo:等速以外の場合
					switch (m_EnemyAmmo.at(loop).m_AmmoMoveType) {
					case AmmoMoveType::Fixed:
						EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, m_EnemyAmmo.at(loop).m_Rot);
						break;
					case AmmoMoveType::Target:
					{
						auto& Player = PlayerManager::Instance()->SetPlane();
						Util::VECTOR3D Pos = Player->GetMat().pos() + Player->GetMat().zvec() * -(10.f * Scale3DRate);
						Util::Matrix3x3 Rot = Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), (EnemyObj()->GetRailMat().pos() - Pos).normalized());
						EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, Rot);
					}
						break;
					case AmmoMoveType::Homing:
						EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, m_EnemyAmmo.at(loop).m_Rot);
						break;
					default:
						break;
					}
					break;
				}
			}
		}
		else {
			//死んだムーブ
			Util::VECTOR3D Pos = EnemyObj()->GetRailMat().pos() + Util::VECTOR3D::up() * (-10.f * Scale3DRate * DrawerMngr->GetDeltaTime());
			Util::Matrix3x3 Rot = Util::Matrix3x3::Get33DX(EnemyObj()->GetRailMat().rotation()
				* Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(360.f * DrawerMngr->GetDeltaTime())));
			EnemyObj()->SetPos(Pos, Rot);
		}
		m_Frame += 1.f;
	}
};

struct EnemyPop {
	int				m_Frame{};
	EnemyScript		m_EnemyScript;
};
class StageScript {
	std::vector<EnemyPop>	m_EnemyPop;
	float					m_Frame{};
public:
	auto& EnemyPop(void) noexcept {
		return m_EnemyPop;
	}
public:
	void Init(std::string Path) noexcept {
		//
		{
			m_EnemyPop.clear();
			File::InputFileStream FileStream;
			FileStream.Open("data/Stage/" + Path + ".txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "SetEnemy") {
						m_EnemyPop.emplace_back();
						m_EnemyPop.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyPop.back().m_EnemyScript.Init(Args.at(2));
					}
				}
			}
			FileStream.Close();
		}
		m_Frame = 0.f;
	}
	void Update() noexcept {
		for (int loop = 0; loop < m_EnemyPop.size(); ++loop) {
			if (std::fabsf(m_Frame - static_cast<float>(m_EnemyPop.at(loop).m_Frame)) < 1.f) {//todo:等速以外の場合
				m_EnemyPop.at(loop).m_EnemyScript.SetActive();
				break;
			}
			if (m_EnemyPop.at(loop).m_EnemyScript.IsActive()) {
				m_EnemyPop.at(loop).m_EnemyScript.Update();
			}
		}
		m_Frame += 1.f;
	}
};

class MainScene : public Util::SceneBase {
	StageScript						m_StageScript;

	std::unique_ptr<MainUI>			m_MainUI{};
	std::unique_ptr<AimPoint>		m_AimPoint{};
	Sound::SoundUniqueID			m_EnviID{ InvalidID };
	Util::VECTOR3D					CamPosition;
	Util::VECTOR3D					CamTarget;
	Util::VECTOR3D					CamUp;
	float							m_Fade{ 1.f };
	bool							m_Exit{ false };
	char		padding[7]{};

	float							m_DamagePer{ 0.f };
	float							m_DamageWatch{ 0.f };
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
