#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BaseObject.hpp"
#include "BackGround.hpp"

#include "../Util/Util.hpp"
#include "../Util/Sound.hpp"

#include "../MainScene/PlayerManager.hpp"

class AmmoBox : public BaseObject {
public:
	AmmoBox(void) noexcept {}
	AmmoBox(const AmmoBox&) = delete;
	AmmoBox(AmmoBox&&) = delete;
	AmmoBox& operator=(const AmmoBox&) = delete;
	AmmoBox& operator=(AmmoBox&&) = delete;
	virtual ~AmmoBox(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char*		GetFrameStr(int) noexcept override { return nullptr; }
private:
	float	m_CoolDown{};
	bool	m_IsActive{ false };
	char		padding[3]{};
public:
	float GetCoolDown(void) const noexcept { return this->m_CoolDown; }
	void SetCoolDown(float value) noexcept {
		m_CoolDown = value;
	}
	bool IsActive(void) const noexcept { return this->m_IsActive; }
	void SetActive(bool value) noexcept {
		m_IsActive = value;
	}
	void Set(const Util::VECTOR3D& Case) noexcept {
		SetMatrix(Util::Matrix4x4::Mtrans(Case));
	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		Util::VECTOR3D Target = this->GetMat().pos();
		this->SetMatrix(
			this->GetMat().rotation() *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(180.f) * DeltaTime) *
			Util::Matrix4x4::Mtrans(Target)
		);
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (!m_IsActive) { return; }
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (!m_IsActive) { return; }
		GetModel().DrawModel();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (!m_IsActive) { return; }
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		SetModel().Dispose();
	}
};

struct GetItemStr {
	Util::VECTOR3D	Pos;
	Util::VECTOR2D	Pos2D;
	float			Timer{};
	std::string		Str;
	unsigned int	Color;
	char		padding[4]{};
};

class AmmoBoxPool : public Util::SingletonBase<AmmoBoxPool> {
private:
	friend class Util::SingletonBase<AmmoBoxPool>;
private:
	std::array<GetItemStr, 6>							m_GetItemStr{};
	std::array<std::shared_ptr<AmmoBox>, 64>			m_Ammo{};
	Sound::SoundUniqueID								m_GetID{ InvalidID };
	Sound::SoundUniqueID								m_FullID{ InvalidID };
private:
	AmmoBoxPool(void) noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Box/");
		m_GetID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/GetItem.wav", false);
		m_FullID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/ItemFull.wav", false);
	}
	AmmoBoxPool(const AmmoBoxPool&) = delete;
	AmmoBoxPool(AmmoBoxPool&&) = delete;
	AmmoBoxPool& operator=(const AmmoBoxPool&) = delete;
	AmmoBoxPool& operator=(AmmoBoxPool&&) = delete;
	virtual ~AmmoBoxPool(void) noexcept {
		for (auto& a : this->m_Ammo) {
			a.reset();
		}
	}
private:
	void AddStr(const Util::VECTOR3D& Case,std::string_view value, unsigned int Color) noexcept {
		for (auto& s : this->m_GetItemStr) {
			if (s.Timer > 0.f) { continue; }
			s.Pos = Case;
			s.Str = value;
			s.Timer = 2.f;
			s.Color = Color;
			break;
		}
	}
public:
	void AddOne(const Util::VECTOR3D& Case) noexcept {
		for (auto& a : this->m_Ammo) {
			if (a->IsActive()) { continue; }
			a->SetActive(true);
			a->SetCoolDown(0.f);
			a->Set(Case);
			break;
		}
	}
public:
	void Init(void) noexcept {
		for (auto& a : this->m_Ammo) {
			a = std::make_shared<AmmoBox>();
			ObjectManager::Instance()->InitObject(a, a, "data/model/Box/");
			a->SetActive(false);
		}
	}
	void Update(void) noexcept {
		auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));

		for (auto& s : this->m_GetItemStr) {
			if (s.Timer <= 0.f) { continue; }
			s.Timer = std::max(s.Timer - DeltaTime, 0.f);
			s.Pos += Util::VECTOR3D::up() * (0.1f * Scale3DRate * DeltaTime);
		}
		for (auto& a : this->m_Ammo) {
			if (!a->IsActive()) { continue; }
			auto Vec = (a->GetMat().pos() - Player->GetMat().pos());
			auto height = Vec.y; Vec.y = 0.f;
			if (Vec.magnitude() <= 0.5f * Scale3DRate && std::fabsf(height) <= 0.5f * Scale3DRate) {
				auto Ammo = std::min(Player->CanHaveAmmo() - Player->TotalAmmo(), 10);
				if (Ammo > 0) {
					AddStr(a->GetMat().pos() + Util::VECTOR3D::up() * (0.5f * Scale3DRate), "Ammo +" + std::to_string(Ammo), ColorPalette::White);
					a->SetActive(false);
					Player->AddAmmo(Ammo);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_GetID)->Play(DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					if (a->GetCoolDown() <= 0.f) {
						a->SetCoolDown(3.f);
						AddStr(a->GetMat().pos() + Util::VECTOR3D::up() * (0.5f * Scale3DRate), "Ammo Full", ColorPalette::Red);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_FullID)->Play(DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
			a->SetCoolDown(std::max(a->GetCoolDown() - DeltaTime, 0.f));
		}
	}
	void SetPos2D(void) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		for (auto& s : this->m_GetItemStr) {
			if (s.Timer <= 0.f) { continue; }
			auto Pos = ConvWorldPosToScreenPos(s.Pos.get());
			if (0.0f < Pos.z && Pos.z < 1.0f) {
				s.Pos2D.x = Pos.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				s.Pos2D.y = Pos.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
			}
		}
	}
	void DrawUI(void) noexcept {
		for (auto& s : this->m_GetItemStr) {
			if (s.Timer <= 0.f) { continue; }
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * (s.Timer * 3.f)), 0, 255));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, 24, 3)->DrawString(
				Draw::FontXCenter::MIDDLE, Draw::FontYCenter::BOTTOM,
				static_cast<int>(s.Pos2D.x), static_cast<int>(s.Pos2D.y),
				s.Color, ColorPalette::Black, Util::SjistoUTF8(s.Str));
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};
