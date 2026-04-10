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
	Gun2LR,
	Gun2UD,
	LWingtip,
	RWingtip,
	Nozzle1,
	Nozzle2,
	DamagePoint1,
	DamagePoint2,
	DamagePoint3,
	Max,
};
static const char* EnemyFrameName[static_cast<int>(EnemyFrame::Max)] = {
	"センター",
	"目",
	"機銃1",
	"機銃2",
	"砲塔旋回1",
	"砲塔仰角1",
	"砲塔旋回2",
	"砲塔仰角2",
	"左翼端",
	"右翼端",
	"ノズル1",
	"ノズル2",
	"DamagePoint1",
	"DamagePoint2",
	"DamagePoint3",
};

struct DamagePointParam {
	Util::VECTOR3D			Pos2D{};
	bool					IsDraw{};
	char		padding[3]{};
	int						frame{};
	int						m_HP{ };
	int						m_HPMax{ 1 };
	float					m_Radius{ 2.f * Scale3DRate };

	int				GetHitPoint(void) const noexcept { return m_HP; }
	float			GetHitPointPer(void) const noexcept {
		if (m_HPMax == 0) { return 0.f; }
		return static_cast<float>(m_HP) / static_cast<float>(m_HPMax);
	}
	void			SetupMaxHitPoint(int value) noexcept {
		m_HPMax = value;
		m_HP = m_HPMax;
	}
	void			SetDamage(int value) noexcept {
		m_HP = std::clamp(m_HP - value, 0, m_HPMax);
	}

};
class Enemy :public BaseObject {
	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	Util::VECTOR3D				m_Gun1Vec;
	Util::VECTOR3D				m_Gun2Vec;

	std::array<DamagePointParam, 3>	m_DamagePoint;
	//char		padding2[4]{};

	Util::Matrix4x4			m_Mat;
	Util::Matrix4x4			RailMat;

	LineEffect				m_LineEffect1;
	LineEffect				m_LineEffect2;

	LineEffect				m_LineEffect3;
	LineEffect				m_LineEffect4;

	Draw::MV1				m_ColModel{};
public:
	Enemy(void) noexcept {}
	Enemy(const Enemy&) = delete;
	Enemy(Enemy&&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy& operator=(Enemy&&) = delete;
	virtual ~Enemy(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(EnemyFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return EnemyFrameName[id]; }
public:
	const auto&		GetDamagePoint(void) const noexcept { return m_DamagePoint; }
	auto&			SetDamagePoint(void) noexcept { return m_DamagePoint; }

	auto&			SetColModel(void) noexcept { return m_ColModel; }

	void			SetPlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		m_Mat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		SetMatrix(m_Mat);
		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineEffect1.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::LWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineEffect2.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::RWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineEffect3.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle1)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineEffect4.Set(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle2)).pos());
		}
	}
	void			UpdatePlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		m_Mat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
	}
	void			SetAmmo(int Shooter, bool IsHoming, Util::Matrix3x3 Mat, float Scale) noexcept {
		Util::VECTOR3D Pos;
		switch (Shooter) {
		case 0:
			Pos = GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun1)).pos();
			break;
		case 1:
			Pos = GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun2)).pos();
			break;
		default:
			break;
		}

		if (Scale < 5.f) {
			EffectPool::Instance()->Shot(Mat.Get44DX() * Util::Matrix4x4::Mtrans(Pos), Scale);
		}
		else {
			EffectPool::Instance()->Cannon(Mat.Get44DX() * Util::Matrix4x4::Mtrans(Pos));
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 200.f * Scale3DRate);
		}
		if (IsHoming) {
			AmmoPool::Instance()->ShotBomb(Mat.Get44DX() * Util::Matrix4x4::Mtrans(Pos),
				(50.f) * Scale3DRate, GetObjectID());
		}
		else {
			AmmoPool::Instance()->ShotAmmo(Mat.Get44DX() * Util::Matrix4x4::Mtrans(Pos),
				(2.5f) * Scale3DRate, GetObjectID(), Scale);
		}
	}
	auto			GetRailMat(void) const noexcept { return RailMat; }
	void			SetRailMat(Util::Matrix4x4 Mat) noexcept {
		RailMat = Mat;
	}
public:
	void Load_Sub(void) noexcept override {
		std::string Path = GetFilePath() + "col.mv1";
		if (File::IsFileExist(Path.c_str())) {
			Draw::MV1::Load(Path, &m_ColModel, DX_LOADMODEL_PHYSICS_DISABLE);
			m_ColModel.SetupCollInfo();
		}
		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun.wav", true);
	}
	void Init_Sub(void) noexcept override {
		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineEffect1.Init(0.25f, 0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineEffect2.Init(0.25f, 0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineEffect3.Init(0.05f, 0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineEffect4.Init(0.05f, 0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
	}
	void Update_Sub(void) noexcept override {
		SetMatrix(m_Mat);
		if (m_ColModel.IsActive()) {
			m_ColModel.SetMatrix(MyMat);
			m_ColModel.RefreshCollInfo();
		}
		//アニメアップデート
		{
			for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
				//SetAnim(loop).SetPer(0.f);
			}
			//SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
			SetModel().FlipAnimAll();
		}
		//砲塔旋回
		if (m_DamagePoint.at(0).GetHitPoint() > 0) {//todo:ちょっと強引
			if (HaveFrame(static_cast<int>(EnemyFrame::Gun1LR)) && HaveFrame(static_cast<int>(EnemyFrame::Gun1UD))) {
				auto& Player = PlayerManager::Instance()->SetPlane();
				Util::VECTOR3D Vec1 = m_Mat.zvec() * -1.f;
				Util::VECTOR3D Vec2 = (Player->GetMat().pos() - GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun1UD)).pos()).normalized();

				Util::Easing(&m_Gun1Vec, Vec2, 0.975f);

				auto Vec1XZ = Vec1; Vec1XZ.y = 0.f;
				auto Vec2XZ = m_Gun1Vec.normalized(); Vec2XZ.y = 0.f;

				auto Vec1TY = Vec1; Vec1TY.x = 0.f; Vec1TY.z = Vec1XZ.magnitude();
				auto Vec2TY = m_Gun1Vec.normalized(); Vec2TY.x = 0.f; Vec2TY.z = Vec2XZ.magnitude();

				SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun1LR),
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Util::VECTOR3D::SignedAngle(Vec1XZ.normalized(), Vec2XZ.normalized(), Util::VECTOR3D::up())) *
					GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun1LR))
				);

				SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun1UD),
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::left(), Util::VECTOR3D::SignedAngle(Vec1TY.normalized(), Vec2TY.normalized(), Util::VECTOR3D::right())) *
					GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun1UD))
				);
			}
		}
		if (m_DamagePoint.at(1).GetHitPoint() > 0) {//todo:ちょっと強引
			if (HaveFrame(static_cast<int>(EnemyFrame::Gun2LR)) && HaveFrame(static_cast<int>(EnemyFrame::Gun2UD))) {
				auto& Player = PlayerManager::Instance()->SetPlane();
				Util::VECTOR3D Vec1 = m_Mat.zvec() * -1.f;
				Util::VECTOR3D Vec2 = (Player->GetMat().pos() - GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Gun2UD)).pos()).normalized();

				Util::Easing(&m_Gun2Vec, Vec2, 0.975f);

				auto Vec1XZ = Vec1; Vec1XZ.y = 0.f;
				auto Vec2XZ = m_Gun2Vec.normalized(); Vec2XZ.y = 0.f;

				auto Vec1TY = Vec1; Vec1TY.x = 0.f; Vec1TY.z = Vec1XZ.magnitude();
				auto Vec2TY = m_Gun2Vec.normalized(); Vec2TY.x = 0.f; Vec2TY.z = Vec2XZ.magnitude();

				SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun2LR),
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Util::VECTOR3D::SignedAngle(Vec1XZ.normalized(), Vec2XZ.normalized(), Util::VECTOR3D::up())) *
					GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun2LR))
				);

				SetFrameLocalMatrix(static_cast<int>(EnemyFrame::Gun2UD),
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::left(), Util::VECTOR3D::SignedAngle(Vec1TY.normalized(), Vec2TY.normalized(), Util::VECTOR3D::right())) *
					GetFrameBaseLocalMat(static_cast<int>(EnemyFrame::Gun2UD))
				);
			}
		}
		//
		if (HaveFrame(static_cast<int>(EnemyFrame::LWingtip))) {
			m_LineEffect1.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::LWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineEffect2.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::RWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineEffect3.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle1)).pos());
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineEffect4.Update(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::Nozzle2)).pos());
		}

		for (auto& a : this->m_DamagePoint) {
			a.IsDraw = false;
			a.frame = InvalidID;
		}
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
		{
			auto Pos2D = ConvWorldPosToScreenPos(GetMat().pos().get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				auto& dp = this->m_DamagePoint.at(0);
				dp.Pos2D.x = Pos2D.x;
				dp.Pos2D.y = Pos2D.y;
				dp.Pos2D.z = (GetMat().pos()-GetCameraPosition()).magnitude();
				dp.IsDraw = true;
				dp.frame = InvalidID;
			}
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::DamagePoint1))) {
			auto Pos2D = ConvWorldPosToScreenPos(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint1)).pos().get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				auto& dp = this->m_DamagePoint.at(0);
				dp.Pos2D.x = Pos2D.x;
				dp.Pos2D.y = Pos2D.y;
				dp.Pos2D.z = (GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint1)).pos() - GetCameraPosition()).magnitude();
				dp.IsDraw = true;
				dp.frame = static_cast<int>(EnemyFrame::DamagePoint1);
			}
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::DamagePoint2))) {
			auto Pos2D = ConvWorldPosToScreenPos(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint2)).pos().get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				auto& dp = this->m_DamagePoint.at(1);
				dp.Pos2D.x = Pos2D.x;
				dp.Pos2D.y = Pos2D.y;
				dp.Pos2D.z = (GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint2)).pos() - GetCameraPosition()).magnitude();
				dp.IsDraw = true;
				dp.frame = static_cast<int>(EnemyFrame::DamagePoint2);
			}
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::DamagePoint3))) {
			auto Pos2D = ConvWorldPosToScreenPos(GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint3)).pos().get());
			if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
				auto& dp = this->m_DamagePoint.at(2);
				dp.Pos2D.x = Pos2D.x;
				dp.Pos2D.y = Pos2D.y;
				dp.Pos2D.z = (GetFrameLocalWorldMatrix(static_cast<int>(EnemyFrame::DamagePoint3)).pos() - GetCameraPosition()).magnitude();
				dp.IsDraw = true;
				dp.frame = static_cast<int>(EnemyFrame::DamagePoint3);
			}
		}
	}
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
			m_LineEffect1.Draw();
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::RWingtip))) {
			m_LineEffect2.Draw();
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle1))) {
			m_LineEffect3.Draw();
		}
		if (HaveFrame(static_cast<int>(EnemyFrame::Nozzle2))) {
			m_LineEffect4.Draw();
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		m_ColModel.Dispose();
		SetModel().Dispose();
	}
};

enum class EnemyType : size_t {
	Normal,
	AI,
	BOSS,
	Max,
};
static const char* EnemyTypeName[static_cast<int>(EnemyType::Max)] = {
	"Normal",
	"AI",
	"BOSS",
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
	int				m_Shooter{};
	int				m_Frame{};
	float			m_Scale{ 1.f };
	bool			m_IsPlayed{ false };
	char		padding[3]{};
	AmmoMoveType	m_AmmoMoveType{};
	Util::Matrix3x3	m_Rot{};
	char		padding2[4]{};
};
class EnemyScript {
	bool					m_IsActive{ false };
	char		padding[3]{};
	int						m_HP[3]{};
	float					m_Radius[3]{};
	char		padding4[4]{};
	EnemyType				m_EnemyType{ EnemyType::Normal };

	std::vector<EnemyMove>	m_EnemyMove;
	std::vector<EnemyAmmo>	m_EnemyAmmo;

	std::string				m_ObjPath;

	float					m_Frame{};
	float					m_EndFrame{ -1.f };

	std::shared_ptr<Enemy>	m_Enemy;

	float				m_OverTime{};
	float				m_RollPer{};
	Util::Matrix3x3			m_Roll;
	Util::VECTOR3D			m_MovePoint;
	Util::VECTOR3D			m_MovePointAdd;
	Util::VECTOR3D			m_MoveVec;
	Util::VECTOR3D			m_RandomMovePoint;
	char		padding2[4]{};

	float				m_BoostPer{};
	float				m_StallPer{};
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };

	bool				m_OverHeat{ false };
	bool				m_Stall{ false };
	char		padding3[6]{};

	float					m_RotRail{ 0.f };
	float					m_OutsidePer{ };
	//char		padding3[4]{};

	Util::Matrix4x4			m_OutsideMatBefore;
	Util::Matrix4x4			m_OutsideMatAfter;

	float				m_ShootTimer{};
	char		padding5[4]{};
public:
	auto& EnemyObj(void) const noexcept { return m_Enemy; }
	auto& GetEnemyType(void) const noexcept { return m_EnemyType; }
	void SetActive(void) noexcept {
		m_Enemy = std::make_shared<Enemy>();
		ObjectManager::Instance()->InitObject(EnemyObj(), EnemyObj(), m_ObjPath);
		switch (m_EnemyType) {
		case EnemyType::Normal:
		case EnemyType::BOSS:
			EnemyObj()->SetPlanePosition(m_EnemyMove.at(0).m_Pos, m_EnemyMove.at(0).m_Rot);
			break;
		case EnemyType::AI:
			EnemyObj()->SetPlanePosition(m_EnemyMove.at(0).m_Pos, m_EnemyMove.at(0).m_Rot);
			EnemyObj()->SetRailMat(m_EnemyMove.at(0).m_Rot.Get44DX() * Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(0.f, 15.f * Scale3DRate, m_EnemyMove.at(0).m_Pos.z)));
			m_MovePoint = Util::VECTOR3D::vget(m_EnemyMove.at(0).m_Pos.x, m_EnemyMove.at(0).m_Pos.y - 15.f * Scale3DRate, 0.f) * -1.f;
			m_MovePointAdd = m_MovePoint;
			break;
		case EnemyType::Max:
		default:
			break;
		}
		for (auto& dp : EnemyObj()->SetDamagePoint()) {
			size_t index = static_cast<size_t>(&dp - &EnemyObj()->SetDamagePoint().front());
			if (EnemyObj()->HaveFrame(dp.frame)) {
				dp.SetupMaxHitPoint(m_HP[index]);
				dp.m_Radius = m_Radius[index];
			}
			else {
				dp.SetupMaxHitPoint(0);
			}
		}
		if (!EnemyObj()->HaveFrame(EnemyObj()->GetDamagePoint().at(0).frame)) {
			EnemyObj()->SetDamagePoint().at(0).SetupMaxHitPoint(m_HP[0]);
			EnemyObj()->SetDamagePoint().at(0).m_Radius = m_Radius[0];
		}

		m_IsActive = true;

		this->m_SpeedTarget = GetSpeedMax();
		this->m_Speed = this->m_SpeedTarget;
	}
	bool IsActive(void) const noexcept { return m_IsActive; }
	bool IsAlive(void) const noexcept {
		if (IsActive()) {
			for (auto& dp : EnemyObj()->GetDamagePoint()) {
				if (dp.GetHitPoint() > 0) {
					return true;
				}
			}
		}
		return false;
	}
public:
	void Init(std::string Path) noexcept {
		//
		{
			File::InputFileStream FileStream;
			FileStream.Open("data/Enemy/" + Path + "/Data.txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::string Func;
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Func, &Args);
				//
				{
					if (Func == "SetModel") {
						m_ObjPath = Args.at(0);
						ObjectManager::Instance()->LoadModel(m_ObjPath);
					}
					if (Func == "HitPoint1") {
						m_HP[0] = std::stoi(Args.at(0));
					}
					if (Func == "HitPoint2") {
						m_HP[1] = std::stoi(Args.at(0));
					}
					if (Func == "HitPoint3") {
						m_HP[2] = std::stoi(Args.at(0));
					}
					if (Func == "Radius1") {
						m_Radius[0] = std::stof(Args.at(0)) * Scale3DRate;
					}
					if (Func == "Radius2") {
						m_Radius[1] = std::stof(Args.at(0)) * Scale3DRate;
					}
					if (Func == "Radius3") {
						m_Radius[2] = std::stof(Args.at(0)) * Scale3DRate;
					}
					if (Func == "Type") {
						for (int loop = 0; loop < static_cast<int>(EnemyType::Max); ++loop) {
							if (Args.at(0) == EnemyTypeName[loop]) {
								m_EnemyType = static_cast<EnemyType>(loop);
								break;
							}
						}
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
				std::string Func;
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Func, &Args);
				//
				{
					if (Func == "SetPoint") {
						m_EnemyMove.emplace_back();
						m_EnemyMove.back().m_Frame = std::stoi(Args.at(0));//Frame
						m_EnemyMove.back().m_Pos = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
						m_EnemyMove.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(std::stof(Args.at(6)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(5)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(4))));
					}
					if (Func == "PutFixedAmmo") {
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Shooter = std::stoi(Args.at(0));
						m_EnemyAmmo.back().m_IsPlayed = false;
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(3)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(2))));
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Fixed;
						m_EnemyAmmo.back().m_Scale = std::stof(Args.at(4));
					}
					if (Func == "PutTargetAmmo") {
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Shooter = std::stoi(Args.at(0));
						m_EnemyAmmo.back().m_IsPlayed = false;
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
						m_EnemyAmmo.back().m_Scale = std::stof(Args.at(3));
					}
					if (Func == "PutTargetAmmoLoop") {
						//撃ち始めるまでの時間,撃つ間隔,何回撃つか,クールダウン時間
						auto startTime = std::stoi(Args.at(1));//Frame
						auto shotFrame = std::stoi(Args.at(2));//Frame
						auto shotCount = std::stoi(Args.at(3));
						auto shotCoolDown = std::stoi(Args.at(4));//Frame
						for (int loop = 0; loop < 1000; ++loop) {
							m_EnemyAmmo.emplace_back();
							m_EnemyAmmo.back().m_Shooter = std::stoi(Args.at(0));
							m_EnemyAmmo.back().m_IsPlayed = false;
							m_EnemyAmmo.back().m_Frame = startTime + (loop % shotCount) * shotFrame + (loop / shotCount) * shotCoolDown;
							m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
							m_EnemyAmmo.back().m_Scale = std::stof(Args.at(5));
						}
					}
					if (Func == "PutHomingAmmo") {//todo:ホーミング弾は未実装
						m_EnemyAmmo.emplace_back();
						m_EnemyAmmo.back().m_Shooter = std::stoi(Args.at(0));
						m_EnemyAmmo.back().m_IsPlayed = false;
						m_EnemyAmmo.back().m_Frame = std::stoi(Args.at(1));//Frame
						m_EnemyAmmo.back().m_Rot =
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(std::stof(Args.at(3)))) *
							Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(std::stof(Args.at(2))));
						m_EnemyAmmo.back().m_AmmoMoveType = AmmoMoveType::Target;
						m_EnemyAmmo.back().m_Scale = std::stof(Args.at(4));
					}
					if (Func == "Delete") {
						m_EndFrame = std::stof(Args.at(0));//Frame
					}
				}
			}
			FileStream.Close();
		}

		m_Frame = 0.f;
	}
	float			GetSpeedMax(void) const noexcept {
		return 1.f * Scale3DRate;
	}
	void Update() noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (IsAlive()) {
			switch (m_EnemyType) {
			case EnemyType::Normal:
			case EnemyType::BOSS:
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
					if (m_Frame > static_cast<float>(m_EnemyAmmo.at(loop).m_Frame)) {
						if (!m_EnemyAmmo.at(loop).m_IsPlayed) {
							m_EnemyAmmo.at(loop).m_IsPlayed = true;
						}
						else {
							continue;
						}
						switch (m_EnemyAmmo.at(loop).m_AmmoMoveType) {
						case AmmoMoveType::Fixed:
							EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_Shooter, m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, m_EnemyAmmo.at(loop).m_Rot, m_EnemyAmmo.at(loop).m_Scale);
							break;
						case AmmoMoveType::Target:
						{
							auto& Player = PlayerManager::Instance()->SetPlane();
							Util::VECTOR3D Pos = Player->GetMat().pos() + Player->GetMat().zvec() * -(10.f * Scale3DRate);//todo:みこし射撃
							Util::Matrix3x3 Rot = Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), (EnemyObj()->GetMat().pos() - Pos).normalized());
							EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_Shooter, m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, Rot, m_EnemyAmmo.at(loop).m_Scale);
						}
						break;
						case AmmoMoveType::Homing:
							EnemyObj()->SetAmmo(m_EnemyAmmo.at(loop).m_Shooter, m_EnemyAmmo.at(loop).m_AmmoMoveType == AmmoMoveType::Homing, m_EnemyAmmo.at(loop).m_Rot, m_EnemyAmmo.at(loop).m_Scale);
							break;
						default:
							break;
						}
						break;
					}
				}
				break;
			case EnemyType::AI:
			{
				Util::VECTOR3D MoveVec = Util::VECTOR3D::forward();
				{
					auto& Player = PlayerManager::Instance()->SetPlane();

					auto Diff = Player->GetMovePoint() - m_MovePoint;
					Diff.z = (Player->GetMat().pos() - EnemyObj()->GetMat().pos()).z;

					bool UpKey = false;
					bool DownKey = false;
					bool LeftKey = false;
					bool RightKey = false;
					{
						switch (GameRule::Instance()->GetGameType()) {
						case GameType::Normal:
							if (Diff.z < 0.f) {
								m_OverTime = 0.f;
								m_RandomMovePoint.Set(GetRandf(36.f * Scale3DRate), GetRandf(24.f * Scale3DRate), 0.f);
							}
							else {
								m_OverTime += DrawerMngr->GetDeltaTime();
								if (m_OverTime > 1.f) {
									m_OverTime -= 1.f;
									m_RandomMovePoint.Set(GetRandf(36.f * Scale3DRate), GetRandf(24.f * Scale3DRate), 0.f);
								}
							}
							UpKey = Diff.y < 3.f * Scale3DRate;
							DownKey = Diff.y > -3.f * Scale3DRate;
							LeftKey = Diff.x < 3.f * Scale3DRate;
							RightKey = Diff.x > -3.f * Scale3DRate;
							if (m_OverTime > 0.f) {
								//ランダムに逃げる
								auto Diff2 = m_RandomMovePoint - m_MovePoint;
								UpKey = Diff2.y < 3.f * Scale3DRate;
								DownKey = Diff2.y > -3.f * Scale3DRate;
								LeftKey = Diff2.x < 3.f * Scale3DRate;
								RightKey = Diff2.x > -3.f * Scale3DRate;
							}
							break;
						case GameType::AllRange:
							m_OverTime += DrawerMngr->GetDeltaTime();
							if (m_OverTime > 1.f) {
								m_OverTime -= 1.f;
								m_RandomMovePoint.Set(GetRandf(36.f * Scale3DRate), GetRandf(24.f * Scale3DRate), 0.f);
							}
							if (m_OverTime > 0.f) {
								//ランダムに逃げる
								auto Diff2 = m_RandomMovePoint - m_MovePoint;
								UpKey = Diff2.y < 3.f * Scale3DRate;
								DownKey = Diff2.y > -3.f * Scale3DRate;
								LeftKey = Diff2.x < 3.f * Scale3DRate;
								RightKey = Diff2.x > -3.f * Scale3DRate;
							}
							break;
						case GameType::Max:
						default:
							break;
						}
					}
					//上下
					{
						float prev = m_MovePointAdd.y;
						if (UpKey && !DownKey) {
							m_MovePointAdd.y -= 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
							MoveVec.y = -0.3f;
						}
						if (DownKey && !UpKey) {
							m_MovePointAdd.y += 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
							MoveVec.y = 0.3f;
						}
						m_MovePointAdd.y = std::clamp(m_MovePointAdd.y, -48.f * Scale3DRate, 12.f * Scale3DRate);
						if (prev == m_MovePointAdd.y) {
							MoveVec.y = 0.0f;
						}
					}
					//ロール
					{
						float prev = m_MovePointAdd.x;
						if (LeftKey && !RightKey) {
							m_MovePointAdd.x -= 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
							MoveVec.x = -0.6f;
						}
						if (RightKey && !LeftKey) {
							m_MovePointAdd.x += 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
							MoveVec.x = 0.6f;
						}
						m_MovePointAdd.x = std::clamp(m_MovePointAdd.x, -18.f * Scale3DRate, 18.f * Scale3DRate);
						if (prev == m_MovePointAdd.x) {
							MoveVec.x = 0.0f;
						}

						float RollPer = 0.f;
						RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
						auto YVec = (EnemyObj()->GetMat() * EnemyObj()->GetRailMat().inverse()).yvec();
						if (YVec.y > 0.f) {
							RollPer *= YVec.x;
						}
						else {
							RollPer *= (YVec.x > 0.f) ? 1.f : -1.f;
						}
						if (prev != m_MovePointAdd.x) {
							if (LeftKey && !RightKey) {
								RollPer = Util::deg2rad(-30);
							}
							if (RightKey && !LeftKey) {
								RollPer = Util::deg2rad(30);
							}
						}
						if (m_OutsidePer > 0.f) {
							float X = (m_OutsideMatAfter * m_OutsideMatBefore.inverse()).zvec2().x;
							LeftKey = X > 0.f;
							RightKey = X < 0.f;
							if (LeftKey && !RightKey) {
								RollPer = Util::deg2rad(-90);
							}
							if (RightKey && !LeftKey) {
								RollPer = Util::deg2rad(90);
							}
						}

						Util::Easing(&m_RollPer, RollPer, 0.95f);

						this->m_Roll = Util::Matrix3x3::RotAxis(this->m_Roll.zvec(), m_RollPer);
					}
					// 進行方向に前進
					{
						bool AccelKey = !m_OverHeat && false;
						bool BrakeKey = !m_Stall && false;
						if (!AccelKey && !BrakeKey) {
							this->m_SpeedTarget = GetSpeedMax();
						}
						if (AccelKey && !BrakeKey) {
							this->m_SpeedTarget += 5.f * DrawerMngr->GetDeltaTime();
							m_BoostPer += DrawerMngr->GetDeltaTime() / 3.f;
						}
						else {
							if (!m_OverHeat) {
								m_BoostPer -= DrawerMngr->GetDeltaTime() / 3.f;
							}
							else {
								m_BoostPer -= DrawerMngr->GetDeltaTime() / 3.f;
							}
						}
						m_BoostPer = std::clamp(m_BoostPer, 0.f, 1.f);
						if (!m_OverHeat) {
							if (m_BoostPer == 1.f) {
								m_OverHeat = true;
							}
						}
						else {
							if (m_BoostPer == 0.f) {
								m_OverHeat = false;
							}
						}

						if (!AccelKey && BrakeKey) {
							this->m_SpeedTarget -= 5.f * DrawerMngr->GetDeltaTime();
							m_StallPer += DrawerMngr->GetDeltaTime() / 5.f;
						}
						else {
							if (!m_Stall) {
								m_StallPer -= DrawerMngr->GetDeltaTime() / 2.f;
							}
							else {
								m_StallPer -= DrawerMngr->GetDeltaTime() / 2.f;
							}
						}
						m_StallPer = std::clamp(m_StallPer, 0.f, 1.f);
						if (!m_Stall) {
							if (m_StallPer == 1.f) {
								m_Stall = true;
							}
						}
						else {
							if (m_StallPer == 0.f) {
								m_Stall = false;
							}
						}
						this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedMax() * 3.f / 4.f, GetSpeedMax() * 3.f / 2.f);
						Util::Easing(&this->m_Speed, this->m_SpeedTarget, 0.95f);
					}
					//射撃
					{
						m_ShootTimer = std::clamp(m_ShootTimer + DrawerMngr->GetDeltaTime(),0.f,10.f);
						if (m_ShootTimer >= 10.f) {
							bool IsInsight = true;
							auto vec1 = EnemyObj()->GetMat().zvec2();
							auto vec2 = Player->GetMat().pos() - EnemyObj()->GetMat().pos();
							auto vec3 = Player->GetMat().zvec2();
							float dot = Util::VECTOR3D::Dot(vec1, vec2.normalized());
							float dot2 = Util::VECTOR3D::Dot(vec1, vec3.normalized());
							if (dot < cos(Util::deg2rad(75))) { IsInsight = false; }//敵の位置が自分の前方左右75度以内
							if (dot2 < cos(Util::deg2rad(75))) { IsInsight = false; }//彼我の向きが左右75度以内
							if (IsInsight) {
								EnemyObj()->SetAmmo(0, true, Util::Matrix3x3::Get33DX(EnemyObj()->GetMat()), 1.f);
								m_ShootTimer = 0.f;
							}
						}
					}
				}
				Util::Easing(&m_MoveVec, MoveVec, 0.95f);
				Util::Easing(&m_MovePoint, m_MovePointAdd, 0.9f);

				Util::VECTOR3D PosAfter = EnemyObj()->GetRailMat().pos() + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), EnemyObj()->GetRailMat().rotation());

				switch (GameRule::Instance()->GetGameType()) {
				case GameType::Normal:
					Util::Easing(&m_RotRail, 0.f, 0.95f);
					EnemyObj()->SetRailMat(Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_RotRail * Util::deg2rad(45.f)* DrawerMngr->GetDeltaTime()) * EnemyObj()->GetRailMat());
					break;
				case GameType::AllRange:
					if (m_MovePointAdd.x < -5.f * Scale3DRate) {
						m_RotRail = std::clamp(m_RotRail - 1.f * DrawerMngr->GetDeltaTime(), -1.f, 1.f);
					}
					else if (m_MovePointAdd.x > 5.f * Scale3DRate) {
						m_RotRail = std::clamp(m_RotRail + 1.f * DrawerMngr->GetDeltaTime(), -1.f, 1.f);
					}
					else {
						Util::Easing(&m_RotRail, 0.f, 0.95f);
					}
					EnemyObj()->SetRailMat(Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_RotRail * Util::deg2rad(45.f)* DrawerMngr->GetDeltaTime()) * EnemyObj()->GetRailMat());
					//範囲外なら真ん中を向く
					if (m_OutsidePer <= 0.f && (PosAfter.magnitude() > 200.f * Scale3DRate)) {
						m_OutsidePer = 1.f;
						m_OutsideMatBefore = EnemyObj()->GetRailMat().rotation();

						auto Pos = PosAfter; Pos.y = 0.f; Pos = Pos.normalized();
						if (Pos.x == 0.f) {
							Pos.x = 0.01f;
						}
						m_OutsideMatAfter = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), std::atan2f(Pos.x, Pos.z)).rotation();
					}
					if (m_OutsidePer > 0.f) {
						m_OutsidePer = std::max(m_OutsidePer - DrawerMngr->GetDeltaTime() / 0.5f, 0.f);
						auto Mat = EnemyObj()->GetRailMat().rotation();
						Util::Easing(&Mat, m_OutsideMatAfter, 0.95f);
						EnemyObj()->SetRailMat(Mat.rotation() * Util::Matrix4x4::Mtrans(PosAfter));
					}
					break;
				case GameType::Max:
				default:
					break;
				}

				EnemyObj()->SetRailMat(EnemyObj()->GetRailMat().rotation() * Util::Matrix4x4::Mtrans(PosAfter));

				Util::Matrix3x3 Rot = this->m_Roll * (Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), m_MoveVec) * Util::Matrix3x3::Get33DX(EnemyObj()->GetRailMat().rotation()));
				Util::VECTOR3D Pos = EnemyObj()->GetRailMat().pos() - Util::Matrix4x4::Vtrans(m_MovePoint, EnemyObj()->GetRailMat().rotation());

				EnemyObj()->UpdatePlanePosition(Pos, Rot);
			}
			break;
			case EnemyType::Max:
			default:
				break;
			}
			if (m_EndFrame != -1.f && m_Frame >= m_EndFrame) {
				for (auto& dp : EnemyObj()->SetDamagePoint()) {
					dp.SetDamage(dp.GetHitPoint());
				}
			}
		}
		else {
			if (EnemyObj()->GetMat().pos().y > 0.f) {
				//死んだムーブ
				Util::VECTOR3D Pos = EnemyObj()->GetMat().pos() + Util::VECTOR3D::up() * (-10.f * Scale3DRate * DrawerMngr->GetDeltaTime());
				Util::Matrix3x3 Rot = Util::Matrix3x3::Get33DX(EnemyObj()->GetMat().rotation()
					* Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(360.f * DrawerMngr->GetDeltaTime())));
				EnemyObj()->UpdatePlanePosition(Pos, Rot);
			}
		}
		m_Frame += 60.f * DrawerMngr->GetDeltaTime();;
	}
	void Dispose() noexcept {
		m_Enemy.reset();
	}
};

struct EnemyPop {
	bool			m_IsPlayed{ false };
	char		padding[3]{};
	int				m_Frame{};
	EnemyScript		m_EnemyScript;
};
class StageScript {
	std::vector<EnemyPop>	m_EnemyPop;
	//char		padding[4]{};
	std::string				m_SetStartEvent {};
	std::string				m_SetEndEvent{};
	std::string				m_NextStage{};
	Util::VECTOR3D			m_StartPos{};
public:
	auto& EnemyPop(void) noexcept { return m_EnemyPop; }

	const auto& GetStartPos(void) const noexcept { return m_StartPos; }
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
				std::string Func;
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Func, &Args);
				//
				{
					if (Func == "SetStageModel") {
						GameRule::Instance()->SetStageModel(Args.at(0));
					}
					else if (Func == "SetGameType") {
						for (int loop = 0; loop < static_cast<int>(GameType::Max); ++loop) {
							if (Args.at(0) == GameTypeName[loop]) {
								GameRule::Instance()->SetGameType(static_cast<GameType>(loop));
								break;
							}
						}
					}
					else if (Func == "GoNextStageNormal") {
						m_NextStage = Args.at(0);
					}
					else if (Func == "GoNextStageAllRange") {
						m_NextStage = Args.at(0);
					}
					else if (Func == "StartEvent") {
						m_SetStartEvent = Args.at(0);
					}
					else if (Func == "StartPosition") {
						m_StartPos = Util::VECTOR3D::vget(std::stof(Args.at(0)), std::stof(Args.at(1)), std::stof(Args.at(2))) * Scale3DRate;
					}
					else if (Func == "EndEvent") {
						m_SetEndEvent = Args.at(0);
					}
					else if (Func == "SetEnemy") {
						m_EnemyPop.emplace_back();
						m_EnemyPop.back().m_Frame = std::stoi(Args.at(0));//Frame
						m_EnemyPop.back().m_EnemyScript.Init(Args.at(1));
					}
				}
			}
			FileStream.Close();
		}
	}
	void Update() noexcept {
		auto& Player = PlayerManager::Instance()->SetPlane();
		for (size_t loop = 0; loop < m_EnemyPop.size(); ++loop) {
			if (m_EnemyPop.at(loop).m_EnemyScript.IsActive()) {
				m_EnemyPop.at(loop).m_EnemyScript.Update();
			}
			if (Player->GetFrame() > static_cast<float>(m_EnemyPop.at(loop).m_Frame)) {
				if (!m_EnemyPop.at(loop).m_IsPlayed) {
					m_EnemyPop.at(loop).m_IsPlayed = true;
				}
				else {
					continue;
				}
				m_EnemyPop.at(loop).m_EnemyScript.SetActive();
				continue;
			}
		}
	}
	void Dispose() noexcept {
		for (size_t loop = 0; loop < m_EnemyPop.size(); ++loop) {
			m_EnemyPop.at(loop).m_EnemyScript.Dispose();
		}
	}
};

