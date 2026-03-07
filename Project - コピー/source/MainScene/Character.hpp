#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BackGround.hpp"
#include "../Util/Enum.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Sound.hpp"
#include "../Util/Util.hpp"
#include "../Draw/MV1.hpp"
#include "../File/FileStream.hpp"

#include "BaseObject.hpp"

#include "Ammo.hpp"



class Target :public BaseObject {
public:
	Target(void) noexcept {}
	Target(const Target&) = delete;
	Target(Target&&) = delete;
	Target& operator=(const Target&) = delete;
	Target& operator=(Target&&) = delete;
	virtual ~Target(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int id) noexcept override { return nullptr; }
public:
	void Load_Sub(void) noexcept override {}
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void DrawFront_Sub(void) const noexcept override {}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
};



enum class CharaAnim {
	Stand,//立ち
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
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"目",
	"機銃1",
	"機銃2",
};

struct PlaneParam {
	bool w = false;
	bool s = false;
	bool a = false;
	bool d = false;
	bool q = false;
	bool e = false;
	bool attack = false;
	bool AccelKey = false;
	bool BrakeKey = false;
	bool IsAuto = false;
	char		padding3[6]{};
	Util::Matrix4x4 TargetMat;
};

class Gun {
	std::array<std::shared_ptr<ShotEffect>, 10>			m_ShotEffect{};
	int													m_ShotEffectID{};
	char		padding2[4]{};

	std::array<std::shared_ptr<Ammo>, 30>				m_AmmoPer{};
	int													m_AmmoID{};
	char		padding3[4]{};

	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	float				m_ShootTimer{};
	char		padding[4]{};
public:
	void Load() noexcept {
		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);
		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
	}
	void Init() noexcept {
		for (auto& s : this->m_ShotEffect) {
			s = std::make_shared<ShotEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
		}
		for (auto& s : this->m_AmmoPer) {
			s = std::make_shared<Ammo>();
			ObjectManager::Instance()->InitObject(s);
		}
	}
	bool Update(bool TriggerPull, bool IsUpdateAnim, const Util::Matrix4x4& Mat, int PlayerID) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();

		m_ShootTimer = std::max(m_ShootTimer - DrawerMngr->GetDeltaTime(), 0.f);
		if (m_ShootTimer == 0.f && TriggerPull) {
			if (IsUpdateAnim) {
				this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(Mat);
				++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());
			}
			this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(Mat, PlayerID);
			++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());

			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(Mat.pos(), 500.f * Scale3DRate);

			m_ShootTimer = 0.2f;

			return true;
		}
		return false;
	}
	void Dispose() noexcept {
		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
	}
};
class PlaneCommon :public BaseObject {
protected:
	Util::ThreadJobs	m_Jobs;
private:
	std::array<float, static_cast<int>(CharaAnim::Max)>		m_AnimPer{};
	//
	Util::VECTOR3D		m_MyPosTarget = Util::VECTOR3D::zero();
	Util::VECTOR3D		m_Vector = Util::VECTOR3D::zero();
	//
	Util::Matrix3x3		m_Rot;
	//
	float				m_Speed = 0.f;
	float				m_SpeedTarget = 0.f;
	//
	float				m_YawPer{};
	float				m_PtichPer{};
	float				m_RollPer{};
	char		padding[4]{};
	//
	float				m_ShootTimer{};
	std::array<Gun, 2>	m_Gun;
	//
	size_t				m_PropellerIndex{};
	size_t				m_EngineIndex{};
	Sound::SoundUniqueID	m_PropellerID{ InvalidID };
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
	//
	int					PlayerID{ InvalidID };
	int					DamageID{};
	//
	int					m_CanWatchBitField{};
	bool				m_ShotSwitch{};
	bool				m_IsInCloud{};
	char		padding2[2]{};
	//
	std::array<std::shared_ptr<DamageEffect>, 30>	m_DamageEffect{};
	size_t				m_DamageEffectNow{};
	float				m_DamageEffectTimer{};
	//
	float				m_HealTimer{};
	char		padding3[4]{};
	//
	int					m_HitPoint{ m_HitPointMax };
	static constexpr int			m_HitPointMax{ 100 };
	//
	PlaneParam			m_PlaneParam{};
	//
	Util::VECTOR3D											m_AimPoint;
	Util::VECTOR2D											m_AimPoint2D;
	char		padding4[4]{};
public:
	PlaneCommon(void) noexcept {}
	PlaneCommon(const PlaneCommon&) = delete;
	PlaneCommon(PlaneCommon&&) = delete;
	PlaneCommon& operator=(const PlaneCommon&) = delete;
	PlaneCommon& operator=(PlaneCommon&&) = delete;
	virtual ~PlaneCommon(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(CharaFrame::Max); }
	const char*		GetFrameStr(int id) noexcept override { return CharaFrameName[id]; }
public:
	int				GetHitPoint(void) const noexcept { return m_HitPoint; }
	float			GetHitPointPer(void) const noexcept { return static_cast<float>(m_HitPoint) / static_cast<float>(m_HitPointMax); }
	bool			GetHitPointLow(void) const noexcept { return GetHitPointPer() < 0.3f; }
	bool			IsAlive(void) const noexcept { return m_HitPoint > 0; }

	void			SetPlayerID(int ID) noexcept { PlayerID = ID; }
	int				GetPlayerID(void) const noexcept { return PlayerID; }

	void			SetDamage(int ID) noexcept {
		DamageID = ID;
		if (DamageID != InvalidID) {
			m_HitPoint = std::clamp(m_HitPoint - 10, 0, m_HitPointMax);
		}
	}
	int				GetDamageID(void) const noexcept { return DamageID; }
	bool			GetShotSwitch(void) const noexcept { return m_ShotSwitch; }
	bool			GetCanWatchPlane(int index) const noexcept { return (m_CanWatchBitField & (1 << index)) != 0; }
	auto			GetTargetPos() const { return this->m_MyPosTarget; }
	float			GetSpeed() const { return this->m_Speed; }
	float			GetSpeedBase(void) const noexcept {
		return 200.f * 2.f / 3.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f;
	}
	void			SetPos(Util::VECTOR3D MyPos, float yRad) noexcept {
		this->m_MyPosTarget = MyPos;
		MyMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), yRad) * Util::Matrix4x4::Mtrans(GetTargetPos());
		m_Rot = Util::Matrix3x3::Get33DX(MyMat);
	}
	bool			IsBorderOut() const noexcept {
		return
			(GetMat().pos().x > 2000.f * Scale3DRate) ||
			(GetMat().pos().x < -2000.f * Scale3DRate) ||
			(GetMat().pos().y > 500.f * Scale3DRate) ||
			(GetMat().pos().y < -1000.f * Scale3DRate) ||
			(GetMat().pos().z > 2000.f * Scale3DRate) ||
			(GetMat().pos().z < -2000.f * Scale3DRate);
	}
	void			SetParam(bool w, bool s, bool a, bool d, bool q, bool e, bool attack, bool AccelKey, bool BrakeKey, bool IsAuto, const Util::Matrix4x4& TargetMat) noexcept {
		m_PlaneParam.w = w;
		m_PlaneParam.s = s;
		m_PlaneParam.a = a;
		m_PlaneParam.d = d;
		m_PlaneParam.q = q;
		m_PlaneParam.e = e;
		m_PlaneParam.attack = attack;
		m_PlaneParam.AccelKey = AccelKey;
		m_PlaneParam.BrakeKey = BrakeKey;
		m_PlaneParam.IsAuto = IsAuto;
		m_PlaneParam.TargetMat = TargetMat;
	}
public:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override;
	void CheckDraw_Sub(void) noexcept override;
	void Draw_Sub(void) const noexcept override;
	void DrawFront_Sub(void) const noexcept override;
	void ShadowDraw_Sub(void) const noexcept override;
	void Dispose_Sub(void) noexcept override;
public:
	virtual bool IsPlayer(void) noexcept = 0;
	virtual Util::Matrix4x4 GetEyeMatrix(void) const noexcept = 0;
	virtual bool IsFPSView(void) const noexcept = 0;

	virtual void Update_Chara(void) noexcept = 0;
};

class Plane :public PlaneCommon {
	Util::VECTOR2D		m_Rad = Util::VECTOR2D::zero();
	Util::VECTOR2D		m_RadAdd = Util::VECTOR2D::zero();
	Util::VECTOR2D		m_RadR = Util::VECTOR2D::zero();
	bool				m_IsFreeView{ false };
	bool				m_IsFPS{ false };
	char		padding[6]{};
public:
	Plane(void) noexcept {}
	Plane(const Plane&) = delete;
	Plane(Plane&&) = delete;
	Plane& operator=(const Plane&) = delete;
	Plane& operator=(Plane&&) = delete;
	virtual ~Plane(void) noexcept {}
public:
	Util::Matrix4x4 GetEyeMatrix(void) const noexcept override {
		return 
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_Rad.x) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), this->m_Rad.y) *
			Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye)).pos());
	}
	bool IsFPSView(void) const noexcept override { return this->m_IsFPS; }
	bool IsPlayer(void) noexcept override { return true; }
	void Update_Chara(void) noexcept override;
};

class EnemyPlane :public PlaneCommon {
	Util::Matrix4x4				m_TargetMat;
	float						m_AutoTimer{};
	float						m_AccelTimer{};
	bool						m_Accel{ false };
	bool						m_Brake{ false };
	char		padding[6]{};
public:
	EnemyPlane(void) noexcept {}
	EnemyPlane(const EnemyPlane&) = delete;
	EnemyPlane(EnemyPlane&&) = delete;
	EnemyPlane& operator=(const EnemyPlane&) = delete;
	EnemyPlane& operator=(EnemyPlane&&) = delete;
	virtual ~EnemyPlane(void) noexcept {}
public:
	Util::Matrix4x4 GetEyeMatrix(void) const noexcept override { return Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye)).pos()); }
	bool IsFPSView(void) const noexcept override { return false; }
	bool IsPlayer(void) noexcept override { return false; }
	void Update_Chara(void) noexcept override;
};
