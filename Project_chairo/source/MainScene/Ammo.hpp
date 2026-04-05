#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "Common.hpp"

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
			1.8f * Scale3DRate,
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
	}
public:
	void Load_Sub(void) noexcept override {
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
	const Draw::GraphHandle*	m_Graph{};
	Util::VECTOR3D			Vector{};
	char		padding0[4]{};
	float					YVecAdd{};
	float					Timer{};
	float					DrawTimer{};
	float					m_Scale{};
	int						Shooter{ InvalidID };
	char		padding[4]{};

	bool					m_SeekerFlag{};
	bool					m_IsHoming{};
	char		padding2[6]{};
	Util::VECTOR3D			m_HomingTarget{};
	std::pair<int, int>		m_HomingID{};
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
		m_SeekerFlag = true;
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
	auto GetHomingID() const noexcept {
		return this->m_HomingID;
	}
	auto IsSeeker() const noexcept {
		return this->m_SeekerFlag;
	}

	void SetHomingTarget(bool IsHoming,int ID, int second) noexcept {
		m_IsHoming = IsHoming;
		m_HomingID.first = ID;
		m_HomingID.second = second;
		if (m_IsHoming) {
			m_SeekerFlag = false;
		}
	}
public:
	void SetHit(const Util::VECTOR3D& pos) noexcept {
		this->Vector = pos - GetMat().pos();
		this->Timer = 0.f;
		this->DrawTimer = this->Timer + 0.25f;
	}
public:
	void Load_Sub(void) noexcept override {
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
	}
	void DrawFront_Sub(void) const noexcept override {
		if (this->DrawTimer == 0.f) { return; }
		DxLib::SetUseZBufferFlag(true);
		//DxLib::SetWriteZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);

		m_LineDraw.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);

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
	}
public:
	void Load_Sub(void) noexcept override {
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
	char		padding1[4]{};
	std::array<std::shared_ptr<Bomb>, 64>				m_BombPer{};
	int													m_BombID{};
	char		padding2[4]{};
	std::array<std::shared_ptr<MultiBomb>, 64>			m_MultiBombPer{};
	int													m_MultiBombID{};
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
	auto& GetBombPer(void) noexcept { return m_BombPer; }
	auto& GetMultiBombPer(void) noexcept { return m_MultiBombPer; }
public:
	void			ShotAmmo(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());
	}
	void			ShotBomb(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_BombPer.at(static_cast<size_t>(this->m_BombID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_BombID %= static_cast<int>(this->m_BombPer.size());
	}
	void			ShotMultiBomb(Util::Matrix4x4 Mat, float speed, int ShooterID) noexcept {
		this->m_MultiBombPer.at(static_cast<size_t>(this->m_MultiBombID))->Set(Mat, ShooterID, speed * Scale3DRate);
		++m_MultiBombID %= static_cast<int>(this->m_MultiBombPer.size());
	}
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Bomb/");
	}
	void Init() noexcept {
		for (auto& s : this->m_AmmoPer) {
			s = std::make_shared<Ammo>();
			ObjectManager::Instance()->InitObject(s);
		}
		for (auto& s : this->m_BombPer) {
			s = std::make_shared<Bomb>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/Bomb/");
		}
		for (auto& s : this->m_MultiBombPer) {
			s = std::make_shared<MultiBomb>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/Bomb/");
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_AmmoPer) {
			s.reset();
		}
		for (auto& s : this->m_BombPer) {
			s.reset();
		}
		for (auto& s : this->m_MultiBombPer) {
			s.reset();
		}
	}
};
