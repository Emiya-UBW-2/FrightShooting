#pragma once

#pragma warning(disable:4464)

#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"

#include "../MainScene/GameRule.hpp"

enum class EnemyFrame {
	Center,
	Eye,
	Gun1,
	Gun2,
	Gun1LR,
	Gun1UD,
	LWingtip,
	RWingtip,
	Nozzle1,
	Nozzle2,
	Max,
};
static const char* EnemyFrameName[static_cast<int>(EnemyFrame::Max)] = {
	"センター",
	"目",
	"機銃1",
	"機銃2",
	"砲塔旋回",
	"砲塔仰角",
	"左翼端",
	"右翼端",
	"ノズル1",
	"ノズル2",
};

class Enemy :public BaseObject {
	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	Util::Matrix4x4			RailMat;
	char		padding2[4]{};

	int						m_HitPoint{ m_HitPointMax };
	static constexpr int	m_HitPointMax{ 100 };

	LineDraw				m_LineDraw1;
	LineDraw				m_LineDraw2;

	LineDraw				m_LineDraw3;
	LineDraw				m_LineDraw4;
public:
	Enemy(void) noexcept {}
	Enemy(const Enemy&) = delete;
	Enemy(Enemy&&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy& operator=(Enemy&&) = delete;
	virtual ~Enemy(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(EnemyFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return EnemyFrameName[id]; }
public:
	int				GetHitPoint(void) const noexcept { return m_HitPoint; }
	float			GetHitPointPer(void) const noexcept { return static_cast<float>(m_HitPoint) / static_cast<float>(m_HitPointMax); }

	void			SetPlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		SetMatrix(RailMat);
		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineDraw1.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::LWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineDraw2.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::RWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineDraw3.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle1)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineDraw4.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle2)).pos());
		}
	}
	void			UpdatePlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
	}
	void			SetAmmo(bool IsHoming, Util::Matrix3x3 Mat) noexcept {
		EffectPool::Instance()->Shot(Mat.Get44DX() * Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun1)).pos()));
		AmmoPool::Instance()->ShotAmmo(Mat.Get44DX() * Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun1)).pos()),
			(2.5f) * Scale3DRate, GetObjectID());

		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);

		if (IsHoming) {
			//TODO:ホーミング
		}
	}
	auto			GetRailMat(void) const noexcept {
		return RailMat;
	}
public:
	void Load_Sub(void) noexcept override {
		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		SetMatrix(RailMat);
		//アニメアップデート
		{
			for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
				//SetAnim(loop).SetPer(0.f);
			}
			//SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
			SetModel().FlipAnimAll();
		}
		//砲塔旋回
		if (HaveFrame(static_cast<int>(EnemyFrame::Gun1LR)) && HaveFrame(static_cast<int>(EnemyFrame::Gun1UD))) {
			auto& Player = PlayerManager::Instance()->SetPlane();
			Util::VECTOR3D Vec1 = GetRailMat().zvec() * -1.f;
			Util::VECTOR3D Vec2 = (Player->GetMat().pos() - GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun1UD)).pos()).normalized();

			auto Vec1XZ = Vec1; Vec1XZ.y = 0.f;
			auto Vec2XZ = Vec2; Vec2XZ.y = 0.f;

			auto Vec1TY = Vec1; Vec1TY.x = 0.f; Vec1TY.z = Vec1XZ.magnitude();
			auto Vec2TY = Vec2; Vec1TY.x = 0.f; Vec2TY.z = Vec2XZ.magnitude();

			SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun1LR),
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(),Util::VECTOR3D::SignedAngle(Vec1XZ.normalized(), Vec2XZ.normalized(), Util::VECTOR3D::up())) *
				GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun1LR))
			);

			SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun1UD),
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::left(), Util::VECTOR3D::SignedAngle(Vec1TY.normalized(), Vec2TY.normalized(), Util::VECTOR3D::right())) *
				GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun1UD))
			);
		}
		//
		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineDraw1.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::LWingtip)).pos(), 0.25f);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineDraw2.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::RWingtip)).pos(), 0.25f);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineDraw3.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle1)).pos(), 0.05f);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineDraw4.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle2)).pos(), 0.05f);
		}
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) const noexcept override {
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (!GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}
	}
	void DrawFront_Sub(void) const noexcept override {
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}

		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineDraw1.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineDraw2.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineDraw3.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineDraw4.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
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
	int						m_HP{};
	char		padding0[4]{};

	std::vector<EnemyMove>	m_EnemyMove;
	std::vector<EnemyAmmo>	m_EnemyAmmo;

	std::string				m_ObjPath;

	float					m_Frame{};
	float					m_EndFrame{ -1.f };

	bool					m_IsActive{ false };
	bool					m_IsDown{ false };
	char		padding[6]{};

	std::shared_ptr<Enemy>	m_Enemy;
public:
	auto& EnemyObj(void) const noexcept {
		return m_Enemy;
	}
	void SetActive(void) noexcept {
		m_Enemy = std::make_shared<Enemy>();
		ObjectManager::Instance()->InitObject(EnemyObj(), EnemyObj(), m_ObjPath);
		EnemyObj()->SetPlanePosition(m_EnemyMove.at(0).m_Pos, m_EnemyMove.at(0).m_Rot);
		m_IsActive = true;
	}
	bool IsActive(void) const noexcept {
		return m_IsActive;
	}
	bool IsAlive(void) const noexcept {
		return !m_IsDown;
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
					if (Args.at(0) == "SetModel") {
						m_ObjPath = Args.at(1);
						ObjectManager::Instance()->LoadModel(m_ObjPath);
					}
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
					if (Args.at(0) == "PutTargetAmmoLoop") {
						//撃ち始めるまでの時間,撃つ間隔,何回撃つか,クールダウン時間
						auto startTime = std::stoi(Args.at(1));//Frame
						auto shotFrame = std::stoi(Args.at(2));//Frame
						auto shotCount = std::stoi(Args.at(3));
						auto shotCoolDown = std::stoi(Args.at(4));//Frame
						for (int loop = 0; loop < 1000; ++loop) {
							m_EnemyAmmo.emplace_back();
							m_EnemyAmmo.back().m_Frame = startTime + (loop % shotCount) * shotFrame + (loop / shotCount) * shotCoolDown;
							m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
						}
					}
					if (Args.at(0) == "PutHomingAmmo") {//todo:ホーミング弾は未実装
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(3)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(2))));
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
					}
					if (Args.at(0) == "Delete") {
						m_EndFrame = std::stof(Args.at(1));//Frame
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
			for (size_t loop = 1; loop < m_EnemyMove.size(); ++loop) {
				if (static_cast<float>(m_EnemyMove.at(loop - 1).m_Frame) <= m_Frame && m_Frame <= static_cast<float>(m_EnemyMove.at(loop).m_Frame)) {
					float Per = (m_Frame - static_cast<float>(m_EnemyMove.at(loop - 1).m_Frame)) / static_cast<float>(m_EnemyMove.at(loop).m_Frame - m_EnemyMove.at(loop - 1).m_Frame);
					Util::VECTOR3D Pos = Util::Lerp(m_EnemyMove.at(loop - 1).m_Pos, m_EnemyMove.at(loop).m_Pos, Per);
					Util::Matrix3x3 Rot = Util::Lerp(m_EnemyMove.at(loop - 1).m_Rot, m_EnemyMove.at(loop).m_Rot, Per);

					EnemyObj()->UpdatePlanePosition(Pos, Rot);
					break;
				}
			}

			for (size_t loop = 0; loop < m_EnemyAmmo.size(); ++loop) {
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

			if (m_Frame >= m_EndFrame) {
				m_IsDown = true;
			}
		}
		else {
			//死んだムーブ
			Util::VECTOR3D Pos = EnemyObj()->GetRailMat().pos() + Util::VECTOR3D::up() * (-10.f * Scale3DRate * DrawerMngr->GetDeltaTime());
			Util::Matrix3x3 Rot = Util::Matrix3x3::Get33DX(EnemyObj()->GetRailMat().rotation()
				* Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(360.f * DrawerMngr->GetDeltaTime())));
			EnemyObj()->UpdatePlanePosition(Pos, Rot);
		}
		m_Frame += 1.f;
	}
	void Dispose() noexcept {
		m_Enemy.reset();
	}
};

struct EnemyPop {
	int				m_Frame{};
	char		padding[4]{};
	EnemyScript		m_EnemyScript;
};
class StageScript {
	std::vector<EnemyPop>	m_EnemyPop;
	//char		padding[4]{};
	float					m_Frame{};
	float					m_ZPosGoal{};
	std::string				m_SetStartEvent {};
	std::string				m_SetEndEvent{};
	std::string				m_NextStage{};
public:
	auto& EnemyPop(void) noexcept { return m_EnemyPop; }
	const auto& GetZPosGoal(void) const noexcept { return m_ZPosGoal; }

	const auto& GetStartEvent(void) const noexcept { return m_SetStartEvent; }
	const auto& GetEndEvent(void) const noexcept { return m_SetEndEvent; }
	const auto& GetNextStage(void) const noexcept { return m_NextStage; }
public:
	void Load(std::string Path) noexcept {
		//
		{
			m_SetStartEvent = "";
			m_SetEndEvent = "";
			m_NextStage = "";
			m_EnemyPop.clear();

			File::InputFileStream FileStream;
			FileStream.Open("data/Stage/" + Path + ".txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "SetStageModel") {
						GameRule::Instance()->SetStageModel(Args.at(1));
					}
					else if (Args.at(0) == "SetGameType") {
						for (int loop = 0; loop < static_cast<int>(GameType::Max); ++loop) {
							if (Args.at(1) == GameTypeName[loop]) {
								GameRule::Instance()->SetGameType(static_cast<GameType>(loop));
								break;
							}
						}
					}
					else if (Args.at(0) == "GoNextStageNormal") {
						m_ZPosGoal = std::stof(Args.at(1)) * Scale3DRate;
						m_NextStage = Args.at(2);
					}
					else if (Args.at(0) == "GoNextStageAllRange") {
						m_NextStage = Args.at(1);
					}
					else if (Args.at(0) == "StartEvent") {
						m_SetStartEvent = Args.at(1);
					}
					else if (Args.at(0) == "EndEvent") {
						m_SetEndEvent = Args.at(1);
					}
					else if (Args.at(0) == "SetEnemy") {
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
		for (size_t loop = 0; loop < m_EnemyPop.size(); ++loop) {
			if (std::fabsf(m_Frame - static_cast<float>(m_EnemyPop.at(loop).m_Frame)) < 1.f) {//todo:等速以外の場合
				m_EnemyPop.at(loop).m_EnemyScript.SetActive();
				continue;
			}
			if (m_EnemyPop.at(loop).m_EnemyScript.IsActive()) {
				m_EnemyPop.at(loop).m_EnemyScript.Update();
			}
		}
		m_Frame += 1.f;
	}
	void Dispose() noexcept {
		for (size_t loop = 0; loop < m_EnemyPop.size(); ++loop) {
			m_EnemyPop.at(loop).m_EnemyScript.Dispose();
		}
	}
};

