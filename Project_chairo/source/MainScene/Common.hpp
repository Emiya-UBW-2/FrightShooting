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
	const Draw::GraphHandle*	m_SmokeGraph{};
	float						m_FireOpticalPer{};
	float						m_SmokePer{};
	float						AnimPer = 0.f;
	float						m_Scale{};
	//char		padding[4]{};
public:
	void SetMuzzleMat(const Util::Matrix4x4& Muzzle) noexcept {
		this->m_SmokeMat = Muzzle;
	}
	void Set(const Util::Matrix4x4& Muzzle, float Scale) noexcept {
		this->m_FireMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(GetRand(90)));
		this->m_SmokeMat = Muzzle;
		this->m_SmokePer = 0.f;
		this->m_Scale = Scale;
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
			Util::Matrix4x4::GetScale(AnimPer * 3.f * this->m_Scale) *
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
	}
	void DrawFront_Sub(void) const noexcept override {
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
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
};

class CannonEffect : public BaseObject {
public:
	CannonEffect(void) noexcept {}
	CannonEffect(const CannonEffect&) = delete;
	CannonEffect(CannonEffect&&) = delete;
	CannonEffect& operator=(const CannonEffect&) = delete;
	CannonEffect& operator=(CannonEffect&&) = delete;
	virtual ~CannonEffect(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
private:
	Util::Matrix4x4				m_FireMat{};
	Util::Matrix4x4				m_SmokeMat{};
	const Draw::GraphHandle*	m_SmokeGraph{};
	float						m_FireOpticalPer{};
	float						m_SmokePer{};
	float						AnimPer = 0.f;
	float						m_Scale{};
public:
	void Set(const Util::Matrix4x4& Muzzle) noexcept {
		this->m_Scale = 0.f;
		this->m_SmokePer = 0.f;
		SetMatrix(Muzzle);
		this->m_SmokeMat = Muzzle;
		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, this->m_Scale, this->m_Scale) * 50.f) * GetMat());
	}
public:
	void Load_Sub(void) noexcept override {
		this->m_SmokeGraph = Draw::GraphPool::Instance()->Get("data/Image/Smoke.png")->Get();
	}
	void Init_Sub(void) noexcept override {
		this->m_Scale = 10.f;
		this->m_SmokePer = 10.f;
	}
	void Update_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		this->m_SmokePer = std::clamp(this->m_SmokePer + DrawerMngr->GetDeltaTime() / 2.5f, 0.f, 1.f);
		float Alpha = 0.f;
		if (m_Scale < 0.05f) {
			Alpha = 1.f;
		}
		else if (m_Scale < 0.25f) {
			Alpha = 1.f - (m_Scale - 0.05f) / (0.25f - 0.05f);
		}
		GetModel().SetOpacityRate(Alpha);

		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, this->m_Scale, this->m_Scale) * 50.f * ((Alpha == 0.f) ? 0.f : 1.f)) * GetMat());
		this->m_Scale += DrawerMngr->GetDeltaTime();
	}
	void SetShadowDraw_Sub(void) const noexcept override {
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
	}
	void DrawFront_Sub(void) const noexcept override {
		GetModel().DrawModel();
		DxLib::SetUseLighting(FALSE);
		float Per = std::sin(Util::deg2rad(180.f * this->m_SmokePer));
		if (Per > 0.f) {
			for (int loop = 0; loop < 3; ++loop) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(64.f * Per));
				DxLib::DrawBillboard3D(
					(this->m_SmokeMat.pos() - this->m_SmokeMat.zvec() * (static_cast<float>(10 + (2 - loop) * 10) / 100.f * Scale3DRate * this->m_SmokePer)).get(),
					0.5f,
					0.5f,
					50.f * Scale3DRate * this->m_SmokePer,
					Util::deg2rad(180.f * this->m_SmokePer) * ((loop % 2 == 0) ? 1.f : -1.f),
					this->m_SmokeGraph->get(),
					true
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		DxLib::SetUseLighting(TRUE);
	}
	void ShadowDraw_Sub(void) const noexcept override {
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
};

class EffectPool : public Util::SingletonBase<EffectPool> {
private:
	friend class Util::SingletonBase<EffectPool>;
private:
	std::array<std::shared_ptr<ShotEffect>, 64>			m_ShotEffect{};
	int													m_ShotEffectID{};
	char		padding[4]{};
	std::array<std::shared_ptr<CannonEffect>, 64>		m_CannonEffect{};
	int													m_CannonEffectID{};
	char		padding3[4]{};
private:
	EffectPool(void) noexcept {}
	EffectPool(const EffectPool&) = delete;
	EffectPool(EffectPool&&) = delete;
	EffectPool& operator=(const EffectPool&) = delete;
	EffectPool& operator=(EffectPool&&) = delete;
	virtual ~EffectPool(void) noexcept { Dispose(); }
public:
	void			Shot(Util::Matrix4x4 Mat, float Scale) noexcept {
		this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(Mat, Scale);
		++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());
	}
	void			Cannon(Util::Matrix4x4 Mat) noexcept {
		this->m_CannonEffect.at(static_cast<size_t>(this->m_CannonEffectID))->Set(Mat);
		++m_CannonEffectID %= static_cast<int>(this->m_CannonEffect.size());
	}
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/model/FireEffect/");
		ObjectManager::Instance()->LoadModel("data/model/Cannon/");
	}
	void Init() noexcept {
		for (auto& s : this->m_ShotEffect) {
			s = std::make_shared<ShotEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
		}
		for (auto& s : this->m_CannonEffect) {
			s = std::make_shared<CannonEffect>();
			ObjectManager::Instance()->InitObject(s, s, "data/model/Cannon/");
		}
	}
	void Dispose(void) noexcept {
		for (auto& s : this->m_ShotEffect) {
			s.reset();
		}
		for (auto& s : this->m_CannonEffect) {
			s.reset();
		}
	}
};

class LineEffect {
	struct LineParam {
		Util::VECTOR3D	m_Pos{};
		float			m_Per{};
	};
private:
	float			m_Time{};
	float			m_Radius{};
	unsigned int	m_Color{};
	int				m_BLENDMODE{};
	std::array<LineParam, 32>	m_Line;
public:
	void Init(float Time, float Radius, unsigned int Color, int BLENDMODE) noexcept {
		this->m_Time = Time;
		this->m_Radius = Radius;
		this->m_Color = Color;
		this->m_BLENDMODE = BLENDMODE;
	}
	void Set(const Util::VECTOR3D& Point) noexcept {
		for (auto& l : this->m_Line) {
			l.m_Pos = Point;
			l.m_Per = 0.f;
		}
	}
	void Update(const Util::VECTOR3D& Point) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		for (auto& l : this->m_Line) {
			l.m_Per = std::max(l.m_Per - DrawerMngr->GetDeltaTime() / this->m_Time, 0.f);
		}
		for (size_t loop = this->m_Line.size() - 1; loop >= 1; --loop) {
			this->m_Line.at(loop) = this->m_Line.at(loop - 1);
		}
		this->m_Line.at(0).m_Pos = Point;
		this->m_Line.at(0).m_Per = 1.f;

	}
	void Draw(void) const noexcept {
		DxLib::SetUseZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);
		for (size_t loop = 0; loop < this->m_Line.size() - 1; ++loop) {
			if ((255.f * this->m_Line.at(loop).m_Per) <= 0.f) { continue; }
			DxLib::SetDrawBlendMode(m_BLENDMODE, static_cast<int>(255.f * this->m_Line.at(loop).m_Per));
			DxLib::DrawCapsule3D(
				this->m_Line.at(loop).m_Pos.get(),
				this->m_Line.at(loop + 1).m_Pos.get(),
				this->m_Radius,
				3,
				this->m_Color,
				this->m_Color,
				true
			);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		DxLib::SetUseLighting(TRUE);
	}
};
