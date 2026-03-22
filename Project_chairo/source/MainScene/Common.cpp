#pragma warning(disable:5259)

#include "Common.hpp"
#include "PlayerManager.hpp"

const ShotEffectPool* Util::SingletonBase<ShotEffectPool>::m_Singleton = nullptr;

const AmmoPool* Util::SingletonBase<AmmoPool>::m_Singleton = nullptr;
const BombPool* Util::SingletonBase<BombPool>::m_Singleton = nullptr;
const MultiBombPool* Util::SingletonBase<MultiBombPool>::m_Singleton = nullptr;

const GameRule* Util::SingletonBase<GameRule>::m_Singleton = nullptr;

void Ammo::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DrawerMngr->GetDeltaTime(), 0.f);
	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
	//this->YVecAdd -= DrawerMngr->GetGravAccel();
	this->Vector.y += this->YVecAdd;
	Util::VECTOR3D Target = GetMat().pos() + this->Vector;
	auto Ret = BackGround::Instance()->GetCol().CollCheck_Line(GetMat().pos(), Target);
	if (Ret.HitFlag == TRUE) {
		Target = Ret.HitPosition;
		SetHit(Target);
	}
	SetMatrix(GetMat().rotation() * Util::Matrix4x4::Mtrans(Target));
}

void Bomb::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DrawerMngr->GetDeltaTime(), 0.f);

	m_LineDraw.Update(GetMat().pos(), 0.5f);

	if (!IsActive()) {
		float Alpha = 0.f;
		if (m_Scale < 0.1f) {
			Alpha = 1.f;
		}
		else if (m_Scale < 0.25f) {
			Alpha = 1.f - (m_Scale - 0.1f) / (0.25f - 0.1f);
		}
		GetModel().SetOpacityRate(Alpha);

		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, m_Scale, m_Scale) * 50.f * ((Alpha == 0.f) ? 0.f : 1.f)) * GetMat());
		m_Scale += DrawerMngr->GetDeltaTime();
	}
	else {
		m_Scale = 0.f;
		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, m_Scale, m_Scale) * 50.f) * GetMat());
	}

	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);

	if (m_IsHoming) {
		float Length = this->Vector.magnitude();
		Util::Easing(
			&this->Vector,
			(m_HomingTarget - GetMat().pos()).normalized() * Length,
			0.95f);
		this->Vector = this->Vector.normalized() * Length;
	}

	//this->YVecAdd -= DrawerMngr->GetGravAccel()*0.5f;
	this->Vector.y += this->YVecAdd;
	Util::VECTOR3D Target = GetMat().pos() + this->Vector;
	auto Ret = BackGround::Instance()->GetCol().CollCheck_Line(GetMat().pos(), Target);
	if (Ret.HitFlag == TRUE) {
		Target = Ret.HitPosition;
		SetHit(Target);
	}
	MyMat = GetMat().rotation() * Util::Matrix4x4::Mtrans(Target);
}

void MultiBomb::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DrawerMngr->GetDeltaTime(), 0.f);

	if (!IsActive()) {
		float Alpha = 0.f;
		if (m_Scale < 0.1f) {
			Alpha = 1.f;
		}
		else if (m_Scale < 0.25f) {
			Alpha = 1.f - (m_Scale - 0.1f) / (0.25f - 0.1f);
		}
		GetModel().SetOpacityRate(Alpha);

		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, m_Scale, m_Scale) * 50.f * ((Alpha == 0.f) ? 0.f : 1.f)) * GetMat());
		m_Scale += DrawerMngr->GetDeltaTime();
	}
	else {
		m_Scale = 0.f;
		SetModel().SetMatrix(Util::Matrix4x4::GetScale(Util::VECTOR3D::vget(m_Scale, m_Scale, m_Scale) * 50.f) * GetMat());
	}

	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
	if (this->Timer == 0.f) {
		SetHit(GetMat().pos());
		int max = 8;
		for (int loop = 0; loop < max; ++loop) {
			BombPool::Instance()->Shot(
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(10)) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(360) * static_cast<float>(loop) / static_cast<float>(max)) *
				GetMat(), 100.f
				, Shooter
			);
		}
	}

	if (m_IsHoming) {
		Util::Easing(
			&this->Vector,
			(m_HomingTarget - GetMat().pos()).normalized() * this->Vector.magnitude(),
			0.95f);
	}

	//this->YVecAdd -= DrawerMngr->GetGravAccel()*0.5f;
	this->Vector.y += this->YVecAdd;
	Util::VECTOR3D Target = GetMat().pos() + this->Vector;
	auto Ret = BackGround::Instance()->GetCol().CollCheck_Line(GetMat().pos(), Target);
	if (Ret.HitFlag == TRUE) {
		Target = Ret.HitPosition;
		SetHit(Target);
	}
	MyMat = GetMat().rotation() * Util::Matrix4x4::Mtrans(Target);
}
