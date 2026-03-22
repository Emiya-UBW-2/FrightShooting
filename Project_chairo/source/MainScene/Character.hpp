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

enum class CharaAnim {
	Stand,
	Roll,
	Pitch,
	Yaw,
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

class Enemy :public BaseObject {
	Util::Matrix3x3		m_Roll;
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };
	float				m_ShootTimer{};
	float				m_RollPer{};
	char		padding[4]{};

	size_t					m_EngineIndex{};
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
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
	int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
public:
	int				GetHitPoint(void) const noexcept { return m_HitPoint; }
	float			GetHitPointPer(void) const noexcept { return static_cast<float>(m_HitPoint) / static_cast<float>(m_HitPointMax); }

	float			GetSpeed() const { return this->m_Speed; }
	float			GetSpeedMax(void) const noexcept {
		return 100.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f;
	}
	void			SetPlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		m_Roll = Util::Matrix3x3::identity();
	}
	void			SetAmmo(bool IsHoming, Util::Matrix3x3 Mat) noexcept {
		ShotEffectPool::Instance()->Shot(Mat.Get44DX() * GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
		AmmoPool::Instance()->Shot(Mat.Get44DX() * GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)),
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
		this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);

		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);
	}
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
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

		m_LineDraw1.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64));
		m_LineDraw2.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64));

		m_LineDraw3.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 0));
		m_LineDraw4.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 0));
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();

		SetModel().Dispose();
	}
};

class MyPlane :public BaseObject {
	Util::Matrix3x3		m_Roll;
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };
	float				m_ShootTimer{};
	float				m_RollPer{};
	char		padding[4]{};

	size_t					m_EngineIndex{};
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	Util::Matrix4x4			RailMat;
	Util::Matrix4x4			EyeMat;
	Util::VECTOR3D			m_MovePoint;
	Util::VECTOR3D			m_MovePointAdd;
	Util::VECTOR3D			m_MoveVec;

	int						m_HitPoint{ m_HitPointMax };
	static constexpr int	m_HitPointMax{ 100 };

	int					DamageID{};

	float					m_RollingTimer1{ 0.f };
	float					m_RollingTimer2{ 0.f };
	float					m_RollingInputTimer1{ 0.f };
	float					m_RollingInputTimer2{ 0.f };

	float					m_RollingCam{ 0.f };

	float					m_DamageInterval{ 0.f };

	float					m_RotRail{ 0.f };

	float					m_OutsidePer{ };
	char		padding3[4]{};

	Util::Matrix4x4			m_OutsideMatBefore;
	Util::Matrix4x4			m_OutsideMatAfter;

	LineDraw				m_LineDraw1;
	LineDraw				m_LineDraw2;

	LineDraw				m_LineDraw3;
	LineDraw				m_LineDraw4;
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
	int				GetHitPoint(void) const noexcept { return m_HitPoint; }
	float			GetHitPointPer(void) const noexcept { return static_cast<float>(m_HitPoint) / static_cast<float>(m_HitPointMax); }

	bool			IsDraw() const {
		if ((m_DamageInterval != 0.f) && (static_cast<int>(m_DamageInterval * 50.f) % 10 > 5)) { return false; }
		return true;
	}

	bool			IsRollingActive() const { return this->m_RollingTimer1 > 0.f || this->m_RollingTimer2 > 0.f; }

	float			GetSpeed() const { return this->m_Speed; }
	float			GetSpeedMax(void) const noexcept {
		return 200.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f;
	}
	void			SetPlanePosition(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		EyeMat = RailMat;
		m_Roll = Util::Matrix3x3::identity();
	}
	auto			GetEyeMatrix(void) const noexcept {
		return EyeMat;
	}
	void			SetDamage(int ID) noexcept {
		if (ID != InvalidID) {
			if (m_DamageInterval != 0.f) { return; }
			m_DamageInterval = 1.0f;
		}

		DamageID = ID;
		if (DamageID != InvalidID) {
			//m_HitPoint = std::clamp(m_HitPoint - 10, 0, m_HitPointMax);
		}
	}
	int				GetDamageID(void) const noexcept { return DamageID; }
public:
	void Load_Sub(void) noexcept override {
		this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);
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

		m_LineDraw1.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64));
		m_LineDraw2.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64));

		m_LineDraw3.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 0));
		m_LineDraw4.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 0));
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (!IsDraw()) { return; }
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();
		SetModel().Dispose();
	}
};
