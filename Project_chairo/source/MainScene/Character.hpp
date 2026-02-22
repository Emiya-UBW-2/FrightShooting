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
#include <thread>


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
		auto* DrawerMngr = Draw::MainDraw::Instance();
		this->m_SmokePer = std::clamp(this->m_SmokePer + DrawerMngr->GetDeltaTime() / 0.5f, 0.f, 1.f);
		AnimPer = std::clamp(AnimPer + DrawerMngr->GetDeltaTime() / 0.1f, 0.f, 1.f);
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
	const Draw::GraphHandle* m_SmokeGraph{};
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	char		padding[4]{};
public:
	void Set(const Util::VECTOR3D& Pos, const Util::VECTOR3D& Normal) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetMatrix(Util::Matrix4x4::Mtrans(Pos));
		this->Vector = Normal;
		this->Vector =
			Util::Matrix4x4::Vtrans(this->Vector,
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::forward(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f))) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::up(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f)))
			) *
			(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate * DrawerMngr->GetDeltaTime());
		this->YVecAdd = 0.f;
		this->Timer = 1.f;

	}
public:
	void Load_Sub(void) noexcept override {
		this->m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Image/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
		//this->YVecAdd -= DrawerMngr->GetGravAccel();
		//this->Vector.y += this->YVecAdd;
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
	void SetShadowDraw_Sub(void) const noexcept override {}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->Timer), 0, 255));
		SetDrawBright(64 * 2, 32 * 2, 16 * 2);
		DxLib::DrawBillboard3D(
			GetMat().pos().get(),
			0.5f,
			0.5f,
			0.8f * Scale3DRate,
			Util::deg2rad(0.f),
			this->m_SmokeGraph->get(),
			true
		);
		DxLib::SetUseLighting(TRUE);
		SetDrawBright(255, 255, 255);
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
	int Shooter{ InvalidID };
	char		padding[4]{};
public:
	void Set(const Util::Matrix4x4& Muzzle, int ID, float Speed) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetMatrix(Muzzle);
		this->Vector = Muzzle.zvec() * -(Speed * DrawerMngr->GetDeltaTime());
		this->YVecAdd = 0.f;
		this->Timer = 5.f;
		this->DrawTimer = this->Timer + 0.1f;
		Shooter = ID;
	}
	bool IsActive() const noexcept {
		return this->Timer != 0.f;
	}
	auto GetVector() const noexcept {
		return this->Vector;
	}
public:
	void SetHit(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 0.1f;
		for (auto& ae : this->m_AmmoEffectPer) {
			ae->Set(
				pos,
				this->Vector.normalized() * -1.f
			);
		}
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(pos, 500.f * Scale3DRate);
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
	void Update_Sub(void) noexcept override;
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
			0.45f * Scale3DRate / 2.f,
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
class DamageEffect : public BaseObject {
public:
	DamageEffect(void) noexcept {}
	DamageEffect(const DamageEffect&) = delete;
	DamageEffect(DamageEffect&&) = delete;
	DamageEffect& operator=(const DamageEffect&) = delete;
	DamageEffect& operator=(DamageEffect&&) = delete;
	virtual ~DamageEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	const Draw::GraphHandle* m_SmokeGraph{};
	Util::VECTOR3D Vector{};
	float YVecAdd{};
	float Timer{};
	char		padding[4]{};
public:
	void Set(const Util::VECTOR3D& Pos, const Util::VECTOR3D& Normal) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetMatrix(Util::Matrix4x4::Mtrans(Pos));
		this->Vector = Normal;
		this->Vector =
			Util::Matrix4x4::Vtrans(this->Vector,
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::forward(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f))) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(Util::VECTOR3D::up(), this->Vector), Util::deg2rad(45.f * (static_cast<float>(-50 + GetRand(100)) / 100.f)))
			) *
			(static_cast<float>(250 + GetRand(100)) / 100.f * Scale3DRate * DrawerMngr->GetDeltaTime());
		this->YVecAdd = 0.f;
		this->Timer = 1.f;

	}
public:
	void Load_Sub(void) noexcept override {
		this->m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Image/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (this->Timer == 0.f) { return; }
		this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
		Util::VECTOR3D Target = GetMat().pos() + this->Vector;
		Util::VECTOR3D Normal;
		SetMatrix(GetMat().rotation() * Util::Matrix4x4::Mtrans(Target));
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
		DxLib::SetUseLighting(FALSE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->Timer), 0, 255));
		SetDrawBright(64 * 2, 32 * 2, 16 * 2);
		DxLib::DrawBillboard3D(
			GetMat().pos().get(),
			0.5f,
			0.5f,
			1.6f * Scale3DRate,
			Util::deg2rad(0.f),
			this->m_SmokeGraph->get(),
			true
		);
		DxLib::SetUseLighting(TRUE);
		SetDrawBright(255, 255, 255);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};

class Enemy :public BaseObject {
	Util::Matrix3x3		m_Roll;
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };
	float				m_ShootTimer{};
	float				m_RollPer{};

	std::array<std::shared_ptr<ShotEffect>, 10>			m_ShotEffect{};
	int													m_ShotEffectID{};
	char		padding1[4]{};

	std::array<std::shared_ptr<Ammo>, 60>				m_AmmoPer{};
	int													m_AmmoID{};
	char		padding3[4]{};

	size_t					m_PropellerIndex{};
	size_t					m_EngineIndex{};
	Sound::SoundUniqueID	m_PropellerID{ InvalidID };
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	Util::Matrix4x4			RailMat;

	int						m_HitPoint{ m_HitPointMax };
	static constexpr int	m_HitPointMax{ 100 };
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
	void			SetPos(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		m_Roll = Util::Matrix3x3::identity();
	}
	void			SetAmmo(bool IsHoming, Util::Matrix3x3 Mat) noexcept {
		this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(Mat.Get44DX() * GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
		++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());

		this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(Mat.Get44DX() * GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), 0,
			(25.f) * Scale3DRate
			);
		++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());

		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);

		if (IsHoming) {
			//TODO:ホーミング
		}
	}
	auto			GetRailMat(void) const noexcept {
		return RailMat;
	}
	auto& GetAmmoPer(void) noexcept { return m_AmmoPer; }
public:
	void Load_Sub(void) noexcept override {
		this->m_PropellerID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/Propeller.wav", true);
		this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);

		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);

		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
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
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->StopAll();
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();

		SetModel().Dispose();

		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
	}
};

class MyPlane :public BaseObject {
	Util::Matrix3x3		m_Roll;
	float				m_Speed{ 0.f };
	float				m_SpeedTarget{ 0.f };
	float				m_ShootTimer{};
	float				m_RollPer{};

	std::array<std::shared_ptr<ShotEffect>, 10>			m_ShotEffect{};
	int													m_ShotEffectID{};
	char		padding1[4]{};

	std::array<std::shared_ptr<Ammo>, 60>				m_AmmoPer{};
	int													m_AmmoID{};
	char		padding3[4]{};

	size_t					m_PropellerIndex{};
	size_t					m_EngineIndex{};
	Sound::SoundUniqueID	m_PropellerID{ InvalidID };
	Sound::SoundUniqueID	m_EngineID{ InvalidID };
	Sound::SoundUniqueID	m_ShotID{ InvalidID };

	Util::Matrix4x4			RailMat;
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

	
	bool			IsRollingActive() const { return this->m_RollingTimer1 > 0.f || this->m_RollingTimer2 > 0.f; }

	float			GetSpeed() const { return this->m_Speed; }
	float			GetSpeedMax(void) const noexcept {
		return 100.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f;
	}
	void			SetPos(Util::VECTOR3D MyPos, Util::Matrix3x3 Mat) noexcept {
		RailMat = Mat.Get44DX() * Util::Matrix4x4::Mtrans(MyPos);
		m_Roll = Util::Matrix3x3::identity();
	}
	auto			GetEyeMatrix(void) const noexcept {
		return RailMat;
	}
	auto& GetAmmoPer(void) noexcept { return m_AmmoPer; }

	void			SetDamage(int ID) noexcept {
		DamageID = ID;
		if (DamageID != InvalidID) {
			//m_HitPoint = std::clamp(m_HitPoint - 10, 0, m_HitPointMax);
		}
	}
	int				GetDamageID(void) const noexcept { return DamageID; }

	void			Shot(Util::Matrix4x4 Mat) noexcept {
		this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(Mat);
		++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());

		this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(Mat, 0,
			(200.f / 60.f * 1000.f + 1000.f) * Scale3DRate
		);
		++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());
	}
public:
	void Load_Sub(void) noexcept override {
		this->m_PropellerID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/Propeller.wav", true);
		this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
		//Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, heartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);

		this->m_ShotID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/gun/auto1911/2.wav", true);

		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
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
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->StopAll();
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();

		SetModel().Dispose();

		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
	}
};
