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
			Util::Matrix4x4::GetScale(AnimPer * 3.f * m_Scale) *
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

class EffectPool : public Util::SingletonBase<EffectPool> {
private:
	friend class Util::SingletonBase<EffectPool>;
private:
	std::array<std::shared_ptr<ShotEffect>, 64>			m_ShotEffect{};
	int													m_ShotEffectID{};
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
	void Draw(float Radius, unsigned int Color, int BLENDMODE) const noexcept {
		DxLib::SetUseZBufferFlag(true);
		DxLib::SetUseLighting(FALSE);
		for (size_t loop = 0; loop < m_Line.size() - 1; ++loop) {
			if ((255.f * m_Line.at(loop).m_Per) <= 0.f) { continue; }
			DxLib::SetDrawBlendMode(BLENDMODE, static_cast<int>(255.f * m_Line.at(loop).m_Per));
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
