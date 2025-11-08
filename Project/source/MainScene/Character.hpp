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
#include "../Draw/MV1.hpp"
#include "../File/FileStream.hpp"

#include "BaseObject.hpp"


enum class CharaAnim {
	Stand,//立ち
	Max,
};

enum class CharaFrame {
	Center,
	Eye,
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"目",
};

class PlaneCommon :public BaseObject {
protected:
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
	void Load_Sub(void) noexcept override {
		Load_Chara();
	}
	void Init_Sub(void) noexcept override {
		Init_Chara();
	}
	void Update_Sub(void) noexcept override {
		Update_Chara();
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Draw_Sub(void) const noexcept override {
		//hitbox描画
		GetModel().DrawModel();
		Draw_Chara();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
		Dispose_Chara();
	}
public:
	virtual bool IsPlayer(void) noexcept = 0;
	virtual void Load_Chara(void) noexcept = 0;
	virtual void Init_Chara(void) noexcept = 0;
	virtual void Update_Chara(void) noexcept = 0;
	virtual void Draw_Chara(void) const noexcept = 0;
	virtual void Dispose_Chara(void) noexcept = 0;
};

class Plane :public PlaneCommon {
	std::array<float, static_cast<int>(CharaAnim::Max)>		m_AnimPer{};

	Util::VECTOR3D											m_AimPoint;
	Util::VECTOR2D											m_AimPoint2D;
	Util::VECTOR3D		m_MyPosTarget = Util::VECTOR3D::zero();
	Util::VECTOR2D		m_Rad = Util::VECTOR2D::zero();
	Util::VECTOR2D		m_RadAdd = Util::VECTOR2D::zero();
	Util::VECTOR2D		m_RadR = Util::VECTOR2D::zero();
	Util::VECTOR3D		m_Vector = Util::VECTOR3D::zero();

	Util::Matrix3x3		m_Rot;

	bool				m_PrevIsFPSView{};
	bool				m_IsFPS{ false };
	int					m_TotalAmmo{ 0 };//予備弾数
	int					m_CanHaveAmmo{ 17 * 2 };//予備弾数
	float				m_Speed = 0.f;
	float				m_MovePer = 0.f;
	int					m_FootSoundID{};

	float				m_YawPer{};
	float				m_PtichPer{};
	float				m_RollPer{};


	Sound::SoundUniqueID	m_heartID{ InvalidID };
	Sound::SoundUniqueID	m_PunchID{ InvalidID };
	Sound::SoundUniqueID	m_KickID{ InvalidID };
	Sound::SoundUniqueID	HitHumanID{ InvalidID };
	Sound::SoundUniqueID	DownHumanID{ InvalidID };
	Sound::SoundUniqueID	ArmlockStartID{ InvalidID };
	Sound::SoundUniqueID	ArmlockID{ InvalidID };
	Sound::SoundUniqueID	StimID{ InvalidID };
public:
	Plane(void) noexcept {}
	Plane(const Plane&) = delete;
	Plane(Plane&&) = delete;
	Plane& operator=(const Plane&) = delete;
	Plane& operator=(Plane&&) = delete;
	virtual ~Plane(void) noexcept {}
public:
	float GetSpeed() const { return this->m_Speed; }
	auto GetMovePer01() const { return this->m_MovePer; }
	auto GetTargetPos() const { return this->m_MyPosTarget; }
	auto GetEyeMatrix(void) const noexcept {
		return 
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_Rad.x) * Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), this->m_Rad.y) *
			Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye)).pos());
	}
	const auto& GetAimPoint2D() const noexcept { return this->m_AimPoint2D; }

	bool IsFPSView(void) const noexcept { return this->m_IsFPS; }

	int TotalAmmo(void) const noexcept { return this->m_TotalAmmo; }
	int CanHaveAmmo(void) const noexcept { return this->m_CanHaveAmmo; }
	bool IsFreeView(void) const noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		return KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim) && !IsFPSView();
	}
	float GetSpeedMax(void) const noexcept {
		return 2.5f * Scale3DRate * DeltaTime;
	}
	void		AddAmmo(int value) noexcept { m_TotalAmmo += value; }
	void SetPos(Util::VECTOR3D MyPos) noexcept {
		this->m_MyPosTarget = MyPos;
		MyMat = Util::Matrix4x4::Mtrans(GetTargetPos());
		m_Rot = Util::Matrix3x3::Get33DX(MyMat);
	}
public:
	void CheckDraw_Sub(void) noexcept override;
public:
	bool IsPlayer(void) noexcept override { return true; }

	void Load_Chara(void) noexcept override {
		DownHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/DownHuman.wav", true);

		this->m_heartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/heart.wav", true);
		this->HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);

		this->ArmlockStartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/ArmlockStart.wav", true);
		this->ArmlockID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Armlock.wav", true);
		this->StimID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Stim.wav", true);

		this->m_PunchID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Punch.wav", true);
		this->m_KickID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Kick.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
	}
	void Init_Chara(void) noexcept override {
		this->m_Speed = 0.f;
		this->m_TotalAmmo = this->m_CanHaveAmmo;
	}
	void Update_Chara(void) noexcept override;
	void Draw_Chara(void) const noexcept override {}
	void Dispose_Chara(void) noexcept override {}
};
