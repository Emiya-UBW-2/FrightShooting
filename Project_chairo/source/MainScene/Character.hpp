#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BackGround.hpp"
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Sound.hpp"
#include "../Draw/MV1.hpp"
#include "../File/FileStream.hpp"

#include "BaseObject.hpp"
#include <thread>

#include "Common.hpp"
#include "Ammo.hpp"

enum class CharaAnim {
	Anim0,
	Anim1,
	Anim2,
	Rolling,
	/*
	Roll,
	Pitch,
	Yaw,
	//*/
	Max,
};

enum class CharaFrame {
	Center,
	Eye,
	Gun1,
	Gun2,
	LWingtip,
	RWingtip,
	Nozzle1,
	Nozzle2,
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"目",
	"機銃1",
	"機銃2",
	"左翼端",
	"右翼端",
	"ノズル1",
	"ノズル2",
};

class MyPlane :public BaseObject {
	Util::Matrix3x3		m_Roll;
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };
	float				m_ShootTimer{};
	float				m_RollPer{};
	float				m_Frame{};
	float				m_BoostPer{};
	float				m_StallPer{};

	bool				m_OverHeat{ false };
	bool				m_Stall{ false };
	char		padding[6]{};

	size_t					m_CockPitIndex{};
	Sound::SoundUniqueID	m_CockPitID{ InvalidID };
	size_t					m_EngineIndex{};
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
	Sound::SoundUniqueID	m_ShotID{ InvalidID };
	Sound::SoundUniqueID	ShotSoundID{ InvalidID };

	Util::Matrix4x4			RailMat;
	Util::Matrix4x4			EyeMat;
	Util::VECTOR3D			m_MovePoint;
	Util::VECTOR3D			m_MovePointAdd;
	Util::VECTOR3D			m_MoveVec;

	Util::VECTOR3D			m_RePos;

	int						m_HitPoint{ m_HitPointMax };
	static constexpr int	m_HitPointMax{ 100 };

	int					DamageID{};

	float					m_RollingTimer1{ 0.f };
	float					m_RollingTimer2{ 0.f };
	float					m_RollingInputTimer1{ 0.f };
	float					m_RollingInputTimer2{ 0.f };

	int						m_ManeuverID{};
	int						m_ManeuverIDRe{};
	float					m_ManeuverPer{ 0.f };
	float					m_ManeuverInputTimer{ 0.f };

	float					m_RollingCam{ 0.f };

	float					m_DamageInterval{ 0.f };

	float					m_RotRail{ 0.f };

	float					m_OutsidePer{ };

	float					m_RollingBarrier{ 0.f };
	//char		padding3[4]{};

	Util::Matrix4x4			m_OutsideMatBefore;
	Util::Matrix4x4			m_OutsideMatAfter;

	LineEffect				m_LineEffect1;
	LineEffect				m_LineEffect2;

	LineEffect				m_LineEffect3;
	LineEffect				m_LineEffect4;
	char		padding4[4]{};

	Draw::MV1				m_barrier{};
public:
	MyPlane(void) noexcept {}
	MyPlane(const MyPlane&) = delete;
	MyPlane(MyPlane&&) = delete;
	MyPlane& operator=(const MyPlane&) = delete;
	MyPlane& operator=(MyPlane&&) = delete;
	virtual ~MyPlane(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
public:
	bool			GetIsManeuver(void) const noexcept { return m_ManeuverIDRe != InvalidID; }
	int				GetHitPoint(void) const noexcept { return m_HitPoint; }
	float			GetHitPointPer(void) const noexcept { return static_cast<float>(m_HitPoint) / static_cast<float>(m_HitPointMax); }
	const auto&		GetMovePoint(void) const noexcept { return m_MovePoint; }

	const auto& GetBoostPer(void) const noexcept { return m_BoostPer; }
	const auto& IsOverHeat(void) const noexcept { return m_OverHeat; }
	const auto& GetStallPer(void) const noexcept { return m_StallPer; }
	const auto& IsStall(void) const noexcept { return m_Stall; }

	const auto& GetRePos(void) const noexcept { return m_RePos; }
	
	const auto& GetManeuverID(void) const noexcept { return m_ManeuverID; }

	bool			IsDraw() const {
		if ((m_DamageInterval != 0.f) && (static_cast<int>(m_DamageInterval * 50.f) % 10 > 5)) { return false; }
		return true;
	}

	bool			IsRollingActive() const { return this->m_RollingTimer1 > 0.f || this->m_RollingTimer2 > 0.f; }

	float			GetSpeed() const { return this->m_Speed; }
	float			GetFrame() const { return this->m_Frame; }
	float			GetSpeedMax(void) const noexcept {
		return 1.f * Scale3DRate;
	}
	void			SetManeuverTargetID(int ManeuverID) noexcept {
		m_ManeuverID = ManeuverID;
	}
	void			SetPlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		EyeMat = RailMat;
		m_Roll = Util::Matrix3x3::identity();

		SetMatrix(
			(this->m_Roll * Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), m_MoveVec) * Util::Matrix3x3::Get33DX(RailMat.rotation())).Get44DX() *
			Util::Matrix4x4::Mtrans(RailMat.pos() - Util::Matrix4x4::Vtrans(m_MovePoint, RailMat.rotation())));

		m_RePos = GetMat().pos();

		m_LineEffect1.Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::LWingtip)).pos());
		m_LineEffect2.Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RWingtip)).pos());
		m_LineEffect3.Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Nozzle1)).pos());
		m_LineEffect4.Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Nozzle2)).pos());
	}
	auto			GetEyeMatrix(void) const noexcept {
		return EyeMat;
	}
	void			SetDamageOff() noexcept {
		DamageID = InvalidID;
	}
	void			SetHitPoint(int hp) noexcept {
		m_HitPoint = std::clamp(hp, 0, m_HitPointMax);
	}
	void			SetDamageOn(int damage) noexcept {
		if (m_DamageInterval != 0.f) { return; }
		m_DamageInterval = 3.0f;
		DamageID = 0;
		SetHitPoint(m_HitPoint - damage);
	}
	bool				IsDamageOn(void) const noexcept { return DamageID != InvalidID; }
public:
	void Load_Sub(void) noexcept override {
		this->m_CockPitID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/fighter-cockpit1.wav", true);
		this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun.wav", true);
		ShotSoundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/rolling_rocket.wav", true);

		Draw::MV1::Load("data/model/barrier/model.mv1", &m_barrier, DX_LOADMODEL_PHYSICS_DISABLE);
	}
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override {
		if (!IsDraw()) { return; }
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) const noexcept override {
		if (!IsDraw()) { return; }
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (!GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}
	}
	void DrawFront_Sub(void) const noexcept override {
		if (!IsDraw()) { return; }
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}

		m_LineEffect1.Draw();
		m_LineEffect2.Draw();

		m_LineEffect3.Draw();
		m_LineEffect4.Draw();

		if (m_RollingBarrier > 0.f) {
			m_barrier.DrawModel();
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (!IsDraw()) { return; }
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		m_barrier.Dispose();
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_CockPitID)->StopAll();
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();
		SetModel().Dispose();
	}
};
