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

class ShotEffect : public BaseObject {
public:
	ShotEffect(void) noexcept {}
	ShotEffect(const ShotEffect&) = delete;
	ShotEffect(ShotEffect&&) = delete;
	ShotEffect& operator=(const ShotEffect&) = delete;
	ShotEffect& operator=(ShotEffect&&) = delete;
	virtual ~ShotEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::Matrix4x4				m_FireMat{};
	Util::Matrix4x4				m_SmokeMat{};
	const Draw::GraphHandle* m_SmokeGraph{};
	float						m_FireOpticalPer{};
	float						m_SmokePer{};
	float						AnimPer = 0.f;
	char		padding[4]{};
public:
	void SetMuzzleMat(const Util::Matrix4x4& Muzzle) noexcept {
		this->m_SmokeMat = Muzzle;
	}
	void Set(const Util::Matrix4x4& Muzzle) noexcept {
		this->m_FireMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(GetRand(90)));
		this->m_SmokeMat = Muzzle;
		this->m_SmokePer = 0.f;
		AnimPer = 0.f;
	}
public:
	void Load_Sub(void) noexcept override {
		this->m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Image/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		this->m_SmokePer = std::clamp(this->m_SmokePer + DeltaTime / 0.5f, 0.f, 1.f);
		AnimPer = std::clamp(AnimPer + DeltaTime / 0.1f, 0.f, 1.f);
		if (0.0f <= AnimPer && AnimPer <= 0.3f) {
			this->m_FireOpticalPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.f, 0.3f, AnimPer));
		}
		if (0.3f <= AnimPer && AnimPer <= 1.f) {
			this->m_FireOpticalPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.3f, 1.f, AnimPer));
		}
		SetMatrix(
			Util::Matrix4x4::GetScale(AnimPer * 3.f) *
			this->m_FireMat *
			this->m_SmokeMat
		);
		SetModel().SetOpacityRate(this->m_FireOpticalPer);
	}
	void SetShadowDraw_Sub(void) const noexcept override {
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		DxLib::SetUseLighting(FALSE);
		float Per = std::sin(Util::deg2rad(180.f * this->m_SmokePer));
		if (Per > 0.f) {
			for (int loop = 0; loop < 3; ++loop) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64.f * Per));
				DxLib::DrawBillboard3D(
					(this->m_SmokeMat.pos() - this->m_SmokeMat.zvec() * (static_cast<float>(10 + (2 - loop) * 10) / 100.f * Scale3DRate * this->m_SmokePer)).get(),
					0.5f,
					0.5f,
					0.8f * Scale3DRate * this->m_SmokePer,
					Util::deg2rad(180.f * this->m_SmokePer) * ((loop % 2 == 0) ? 1.f : -1.f),
					this->m_SmokeGraph->get(),
					true
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		if (this->m_FireOpticalPer > 0.f) {
			GetModel().DrawModel();
		}
		DxLib::SetUseLighting(TRUE);
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
};
class AmmoHitEffect : public BaseObject {
public:
	AmmoHitEffect(void) noexcept {}
	AmmoHitEffect(const AmmoHitEffect&) = delete;
	AmmoHitEffect(AmmoHitEffect&&) = delete;
	AmmoHitEffect& operator=(const AmmoHitEffect&) = delete;
	AmmoHitEffect& operator=(AmmoHitEffect&&) = delete;
	virtual ~AmmoHitEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	char		padding[4]{};
public:
	void Set(const Util::VECTOR3D& Pos, const Util::VECTOR3D& Normal) noexcept {
		SetMatrix(Util::Matrix4x4::Mtrans(Pos));
		this->Vector = Normal;
		this->Vector =
			Util::Matrix4x4::Vtrans(this->Vector,
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::forward(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f))) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::up(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f)))
			) *
			(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate * DeltaTime);
		this->YVecAdd = 0.f;
		this->Timer = 1.f;

	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DeltaTime, 0.f);
		this->YVecAdd -= GravAccel;
		this->Vector.y += this->YVecAdd;
		Util::VECTOR3D Target = GetMat().pos() + this->Vector;
		Util::VECTOR3D Normal;
		/*
		if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target, &Normal)) {
			this->YVecAdd = 0.f;
			this->Vector = Util::VECTOR3D::Reflect(this->Vector, Normal) * 0.8f;
		}
		//*/
		SetMatrix(GetMat().rotation() * Util::Matrix4x4::Mtrans(Target));
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->Timer), 0, 255));
		DxLib::DrawCapsule3D(
			(GetMat().pos() - this->Vector * std::clamp(this->Timer, 0.f, 1.f)).get(),
			GetMat().pos().get(),
			0.01f * Scale3DRate / 2.f,
			6,
			DxLib::GetColor(255, 255, 128),
			DxLib::GetColor(255, 255, 0),
			true
		);
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};
class Ammo : public BaseObject {
public:
	Ammo(void) noexcept {}
	Ammo(const Ammo&) = delete;
	Ammo(Ammo&&) = delete;
	Ammo& operator=(const Ammo&) = delete;
	Ammo& operator=(Ammo&&) = delete;
	virtual ~Ammo(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	float DrawTimer{};
	Sound::SoundUniqueID HitGroundID{ InvalidID };
	Sound::SoundUniqueID HitHumanID{ InvalidID };
	std::array<std::shared_ptr<AmmoHitEffect>, 10>	m_AmmoEffectPer{};
public:
	void Set(const Util::Matrix4x4& Muzzle) noexcept {
		SetMatrix(Muzzle);
		this->Vector = Muzzle.zvec() * -((200.f / 60.f * 1000.f + 1000.f) * Scale3DRate * DeltaTime);
		this->YVecAdd = 0.f;
		this->Timer = 5.f;
		this->DrawTimer = this->Timer + 0.1f;
	}
private:
	void SetAmmo(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 0.1f;
		for (auto& ae : this->m_AmmoEffectPer) {
			ae->Set(
				pos,
				this->Vector.normalized() * -1.f
			);
		}
	}
public:
	void Load_Sub(void) noexcept override {
		HitGroundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitGround.wav", true);
		HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);
	}
	void Init_Sub(void) noexcept override {
		for (auto& ae : this->m_AmmoEffectPer) {
			ae = std::make_shared<AmmoHitEffect>();
			ObjectManager::Instance()->InitObject(ae);
		}
	}
	void Update_Sub(void) noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		this->DrawTimer = std::max(this->DrawTimer - DeltaTime, 0.f);
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DeltaTime, 0.f);
		//this->YVecAdd -= GravAccel;
		this->Vector.y += this->YVecAdd;
		Util::VECTOR3D Target = GetMat().pos() + this->Vector;
		//if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target)) 
		if(false)
		{
			SetAmmo(Target);
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(Target, 10.f * Scale3DRate);
		}
		SetMatrix(GetMat().rotation() *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DeltaTime) *
			Util::Matrix4x4::Mtrans(Target));
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->DrawTimer / 0.1f), 0, 255));
		DxLib::DrawCapsule3D(
			(GetMat().pos() - this->Vector * std::clamp(this->DrawTimer / 0.1f, 0.f, 1.f)).get(),
			GetMat().pos().get(),
			0.09f * Scale3DRate / 2.f,
			6,
			DxLib::GetColor(255, 255, 128),
			DxLib::GetColor(255, 255, 0),
			true
		);
		DxLib::SetUseLighting(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		for (auto& ae : this->m_AmmoEffectPer) {
			ae.reset();
		}
	}
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
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (!GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}
		Draw_Chara();
	}
	void DrawFront_Sub(void) const noexcept override {
		for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
			if (GetModel().GetMeshSemiTransState(loop)) {
				GetModel().DrawMesh(loop);
			}
		}
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

	float				m_ShootTimer{};

	float				m_YawPer{};
	float				m_PtichPer{};
	float				m_RollPer{};

	std::array<std::shared_ptr<ShotEffect>, 10>			m_ShotEffect{};
	int													m_ShotEffectID{};

	std::array<std::shared_ptr<Ammo>, 60>				m_AmmoPer{};
	int													m_AmmoID{};

	Sound::SoundUniqueID	m_heartID{ InvalidID };
	Sound::SoundUniqueID	m_PunchID{ InvalidID };
	Sound::SoundUniqueID	m_KickID{ InvalidID };
	Sound::SoundUniqueID	HitHumanID{ InvalidID };
	Sound::SoundUniqueID	DownHumanID{ InvalidID };
	Sound::SoundUniqueID	ArmlockStartID{ InvalidID };
	Sound::SoundUniqueID	ArmlockID{ InvalidID };
	Sound::SoundUniqueID	StimID{ InvalidID };
	Sound::SoundUniqueID								m_ShotID{ InvalidID };
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
		return 200.f / 60.f * 1000.f * Scale3DRate * DeltaTime;
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

		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);

		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
	}
	void Init_Chara(void) noexcept override {
		this->m_Speed = 0.f;
		this->m_TotalAmmo = this->m_CanHaveAmmo;

		for (auto& s : this->m_ShotEffect) {
			s = std::make_shared<ShotEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
		}
		for (auto& s : this->m_AmmoPer) {
			s = std::make_shared<Ammo>();
			ObjectManager::Instance()->InitObject(s);
		}
	}
	void Update_Chara(void) noexcept override;
	void Draw_Chara(void) const noexcept override {}
	void Dispose_Chara(void) noexcept override {
		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
	}
};
