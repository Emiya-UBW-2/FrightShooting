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

class ShotEffectPool : public Util::SingletonBase<ShotEffectPool> {
private:
	friend class Util::SingletonBase<ShotEffectPool>;
private:
	std::array<std::shared_ptr<ShotEffect>, 64>			m_ShotEffect{};
	int													m_ShotEffectID{};
	char		padding3[4]{};
private:
	ShotEffectPool(void) noexcept {}
	ShotEffectPool(const ShotEffectPool&) = delete;
	ShotEffectPool(ShotEffectPool&&) = delete;
	ShotEffectPool& operator=(const ShotEffectPool&) = delete;
	ShotEffectPool& operator=(ShotEffectPool&&) = delete;
	virtual ~ShotEffectPool(void) noexcept { Dispose(); }
public:
	void			Shot(Util::Matrix4x4 Mat) noexcept {
		this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(Mat);
		++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());
	}
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
	}
	void Init() noexcept {
		for (auto& s : this->m_ShotEffect) {
			s = std::make_shared<ShotEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
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
	auto GetShooterID() const noexcept {
		return this->Shooter;
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
		DxLib::SetUseZBufferFlag(TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->DrawTimer / 0.1f), 0, 255));
		DxLib::DrawCapsule3D(
			(GetMat().pos() - this->Vector * std::clamp(this->DrawTimer / 0.1f, 0.f, 1.f)).get(),
			GetMat().pos().get(),
			0.45f * Scale3DRate / 2.f,
			6,
			DxLib::GetColor(192, 192, 255),
			DxLib::GetColor(0, 255, 255),
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

struct LineParam {
	Util::VECTOR3D m_Pos{};
	float m_Per{};
};
class LineDraw {
private:
	std::array<LineParam, 32>	m_Line;
public:
	void Set(const Util::VECTOR3D& Point) noexcept {
		for (auto& l : m_Line) {
			l.m_Pos = Point;
			l.m_Per = 0.f;
		}
	}
	void Update(const Util::VECTOR3D& Point, float Time) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		for (auto& l : m_Line) {
			l.m_Per = std::max(l.m_Per - DrawerMngr->GetDeltaTime() / Time, 0.f);
		}
		for (size_t loop = m_Line.size() - 1; loop >= 1; --loop) {
			m_Line.at(loop) = m_Line.at(loop - 1);
		}
		m_Line.at(0).m_Pos = Point;
		m_Line.at(0).m_Per = 1.f;

	}
	void Draw(float Radius, unsigned int Color) const noexcept {
		DxLib::SetUseZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);
		for (size_t loop = 0; loop < m_Line.size() - 1; ++loop) {
			if ((255.f * m_Line.at(loop).m_Per) <= 0.f) { continue; }
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * m_Line.at(loop).m_Per));
			DxLib::DrawCapsule3D(
				m_Line.at(loop).m_Pos.get(),
				m_Line.at(loop + 1).m_Pos.get(),
				Radius,
				3,
				Color,
				Color,
				true
			);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DxLib::SetUseLighting(TRUE);
	}
};
class Bomb : public BaseObject {
public:
	Bomb(void) noexcept {}
	Bomb(const Bomb&) = delete;
	Bomb(Bomb&&) = delete;
	Bomb& operator=(const Bomb&) = delete;
	Bomb& operator=(Bomb&&) = delete;
	virtual ~Bomb(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	LineDraw				m_LineDraw;
	const Draw::GraphHandle* m_Graph{};
	Util::VECTOR3D Vector{};
	char		padding0[4]{};
	float YVecAdd{};
	float Timer{};
	float DrawTimer{};
	float m_Scale{};
	Sound::SoundUniqueID HitGroundID{ InvalidID };
	Sound::SoundUniqueID HitHumanID{ InvalidID };
	int Shooter{ InvalidID };
	char		padding[4]{};

	bool m_IsHoming{};
	char		padding2[7]{};
	Util::VECTOR3D m_HomingTarget{};
	char		padding3[4]{};
public:
	void Set(const Util::Matrix4x4& Muzzle, int ID, float Speed) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetMatrix(Muzzle);
		this->Vector = Muzzle.zvec() * -(Speed * DrawerMngr->GetDeltaTime());
		this->YVecAdd = 0.f;
		this->Timer = 5.f;
		this->DrawTimer = this->Timer + 0.25f;
		Shooter = ID;

		m_LineDraw.Set(GetMat().pos());
	}
	bool IsActive() const noexcept {
		return this->Timer != 0.f;
	}
	auto GetVector() const noexcept {
		return this->Vector;
	}
	auto GetShooterID() const noexcept {
		return this->Shooter;
	}

	void SetHomingTarget(bool IsHoming, Util::VECTOR3D& pos) noexcept {
		m_IsHoming = IsHoming;
		m_HomingTarget = pos;
	}
public:
	void SetHit(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 0.25f;
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(pos, 500.f * Scale3DRate);
	}
public:
	void Load_Sub(void) noexcept override {
		HitGroundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitGround.wav", true);
		HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);
		this->m_Graph = Draw::GraphPool::Instance()->Get("data/Image/Light.png")->Get();
	}
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		DxLib::SetUseZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);

		m_LineDraw.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64));

		float Per = std::sin(Util::deg2rad(90.f));
		if (Per > 0.f) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			for (int loop = 0; loop < 2; ++loop) {
				DxLib::DrawBillboard3D(
					GetMat().pos().get(),
					0.5f,
					0.5f,
					10.f * Scale3DRate,
					Util::deg2rad(90 * loop + static_cast<int>(this->Timer*180.f)),
					this->m_Graph->get(),
					true
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		DxLib::SetUseLighting(TRUE);

		GetModel().DrawModel();
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};
class MultiBomb : public BaseObject {
public:
	MultiBomb(void) noexcept {}
	MultiBomb(const MultiBomb&) = delete;
	MultiBomb(MultiBomb&&) = delete;
	MultiBomb& operator=(const MultiBomb&) = delete;
	MultiBomb& operator=(MultiBomb&&) = delete;
	virtual ~MultiBomb(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	const Draw::GraphHandle* m_Graph{};
	Util::VECTOR3D Vector{};
	char		padding0[4]{};
	float YVecAdd{};
	float Timer{};
	float DrawTimer{};
	float m_Scale{};
	Sound::SoundUniqueID HitGroundID{ InvalidID };
	Sound::SoundUniqueID HitHumanID{ InvalidID };
	int Shooter{ InvalidID };
	char		padding[4]{};

	bool m_IsHoming{};
	char		padding2[7]{};
	Util::VECTOR3D m_HomingTarget{};
	char		padding3[4]{};
public:
	void Set(const Util::Matrix4x4& Muzzle, int ID, float Speed) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		SetMatrix(Muzzle);
		this->Vector = Muzzle.zvec() * -(Speed * DrawerMngr->GetDeltaTime());
		this->YVecAdd = 0.f;
		this->Timer = 0.25f;
		this->DrawTimer = this->Timer + 0.25f;
		Shooter = ID;
	}
	bool IsActive() const noexcept {
		return this->Timer != 0.f;
	}
	auto GetVector() const noexcept {
		return this->Vector;
	}
	auto GetShooterID() const noexcept {
		return this->Shooter;
	}
	
	void SetHomingTarget(bool IsHoming, Util::VECTOR3D& pos) noexcept {
		m_IsHoming = IsHoming;
		m_HomingTarget = pos;
	}
public:
	void SetHit(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 0.25f;
		Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(pos, 500.f * Scale3DRate);
	}
public:
	void Load_Sub(void) noexcept override {
		HitGroundID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitGround.wav", true);
		HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);
		this->m_Graph = Draw::GraphPool::Instance()->Get("data/Image/Light.png")->Get();
	}
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) const noexcept override {
		if (this->Timer == 0.f) { return; }
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		DxLib::SetUseZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);
		float Per = std::sin(Util::deg2rad(90.f));
		if (Per > 0.f) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			for (int loop = 0; loop < 2; ++loop) {
				DxLib::DrawBillboard3D(
					GetMat().pos().get(),
					0.5f,
					0.5f,
					10.f * Scale3DRate,
					Util::deg2rad(90 * loop + static_cast<int>(this->Timer * 180.f)),
					this->m_Graph->get(),
					true
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		DxLib::SetUseLighting(TRUE);

		GetModel().DrawModel();
	}
	void DrawFront_Sub(void) const noexcept override {
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
	}
};

class AmmoPool : public Util::SingletonBase<AmmoPool> {
private:
	friend class Util::SingletonBase<AmmoPool>;
private:
	std::array<std::shared_ptr<Ammo>, 64>				m_AmmoPer{};
	int													m_AmmoID{};
	char		padding3[4]{};
private:
	AmmoPool(void) noexcept {}
	AmmoPool(const AmmoPool&) = delete;
	AmmoPool(AmmoPool&&) = delete;
	AmmoPool& operator=(const AmmoPool&) = delete;
	AmmoPool& operator=(AmmoPool&&) = delete;
	virtual ~AmmoPool(void) noexcept { Dispose(); }
public:
	auto& GetAmmoPer(void) noexcept { return m_AmmoPer; }

	void			Shot(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());
	}
public:
	void Init() noexcept {
		for (auto& s : this->m_AmmoPer) {
			s = std::make_shared<Ammo>();
			ObjectManager::Instance()->InitObject(s);
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
	}
};
class BombPool : public Util::SingletonBase<BombPool> {
private:
	friend class Util::SingletonBase<BombPool>;
private:
	std::array<std::shared_ptr<Bomb>, 64>				m_BombPer{};
	int													m_BombID{};
	char		padding2[4]{};
private:
	BombPool(void) noexcept {}
	BombPool(const BombPool&) = delete;
	BombPool(BombPool&&) = delete;
	BombPool& operator=(const BombPool&) = delete;
	BombPool& operator=(BombPool&&) = delete;
	virtual ~BombPool(void) noexcept { Dispose(); }
public:
	auto& GetBombPer(void) noexcept { return m_BombPer; }

	void			Shot(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_BombPer.at(static_cast<size_t>(this->m_BombID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_BombID %= static_cast<int>(this->m_BombPer.size());
	}
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Bomb/");
	}
	void Init() noexcept {
		for (auto& s : this->m_BombPer) {
			s = std::make_shared<Bomb>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/Bomb/");
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_BombPer) {
			s.reset();
		}
	}
};
class MultiBombPool : public Util::SingletonBase<MultiBombPool> {
private:
	friend class Util::SingletonBase<MultiBombPool>;
private:
	std::array<std::shared_ptr<MultiBomb>, 64>				m_MultiBombPer{};
	int													m_MultiBombID{};
	char		padding2[4]{};
private:
	MultiBombPool(void) noexcept {}
	MultiBombPool(const MultiBombPool&) = delete;
	MultiBombPool(MultiBombPool&&) = delete;
	MultiBombPool& operator=(const MultiBombPool&) = delete;
	MultiBombPool& operator=(MultiBombPool&&) = delete;
	virtual ~MultiBombPool(void) noexcept { Dispose(); }
public:
	auto& GetMultiBombPer(void) noexcept { return m_MultiBombPer; }

	void			Shot(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_MultiBombPer.at(static_cast<size_t>(this->m_MultiBombID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_MultiBombID %= static_cast<int>(this->m_MultiBombPer.size());
	}
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Bomb/");
	}
	void Init() noexcept {
		for (auto& s : this->m_MultiBombPer) {
			s = std::make_shared<MultiBomb>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/Bomb/");
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_MultiBombPer) {
			s.reset();
		}
	}
};

enum class GameType : size_t {
	Normal,
	AllRange,
	Max,
};
static const char* GameTypeName[static_cast<int>(GameType::Max)] = {
	"Normal",
	"AllRange",
};

class GameRule : public Util::SingletonBase<GameRule> {
private:
	friend class Util::SingletonBase<GameRule>;
private:
	std::string			m_ModelName;
	std::string			m_StageName;
	GameType			m_GameType{ GameType::AllRange };
private:
	GameRule(void) noexcept {}
	GameRule(const GameRule&) = delete;
	GameRule(GameRule&&) = delete;
	GameRule& operator=(const GameRule&) = delete;
	GameRule& operator=(GameRule&&) = delete;
	virtual ~GameRule(void) noexcept { Dispose(); }
public:
	void SetStageModel(std::string_view str) noexcept {
		m_ModelName = str;
	}
	const auto& GetStageModel(void) const noexcept { return m_ModelName; }

	void SetNextStage(std::string_view str) noexcept {
		m_StageName = str;
	}
	const auto& GetNextStage(void) const noexcept { return m_StageName; }

	void SetGameType(GameType type) noexcept {
		m_GameType = type;
	}
	const auto& GetGameType(void) const noexcept { return m_GameType; }
public:
	void Dispose(void) noexcept {
	}
};
