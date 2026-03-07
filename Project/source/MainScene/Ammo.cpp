#include "Ammo.hpp"
#include "PlayerManager.hpp"

void Ammo::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (m_AimPointIsDraw) {
		m_AimPointDrawTimer += DrawerMngr->GetDeltaTime();
	}
	else {
		m_AimPointDrawTimer = 0.f;
	}
	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DrawerMngr->GetDeltaTime(), 0.f);
	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
	//this->YVecAdd -= DrawerMngr->GetGravAccel();
	this->Vector.y += this->YVecAdd;
	Util::VECTOR3D Target = GetMat().pos() + this->Vector;
	//if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target)) 
	for (auto& c : PlayerManager::Instance()->SetPlane()) {
		int index = static_cast<int>(&c - &PlayerManager::Instance()->SetPlane().front());
		if (Shooter == index) { continue; }
		SEGMENT_SEGMENT_RESULT Result;
		Util::GetSegmenttoSegment(c->GetMat().pos(), c->GetMat().pos(), GetMat().pos(), Target,&Result);
		if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
			Target = Result.SegB_MinDist_Pos;
			SetAmmo(Target);
			for (auto& ae : this->m_AmmoEffectPer) {
				ae->Set(
					c->GetMat().pos(),
					this->Vector.normalized() * -1.f
				);
			}
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitGroundID)->Play3D(Target, 500.f * Scale3DRate);
			c->SetDamage(Shooter);
			break;
		}
	}
	for (auto& c : PlayerManager::Instance()->SetTarget()) {
		SEGMENT_SEGMENT_RESULT Result;
		Util::GetSegmenttoSegment(c->GetMat().pos(), c->GetMat().pos(), GetMat().pos(), Target, &Result);
		if (Result.SegA_SegB_MinDist_Square < (50.f * Scale3DRate) * (50.f * Scale3DRate)) {
			Target = Result.SegB_MinDist_Pos;
			SetAmmo(Target);
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play();

			m_AimPoint = c->GetMat().pos();
			m_AimPointIsDraw = true;

			c->SetMatrix(Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(
				GetRandf(1500.f * Scale3DRate),
				GetRandf(500.f * Scale3DRate),
				GetRandf(1500.f * Scale3DRate)
			)));
			break;
		}
	}
	SetMatrix(GetMat().rotation() *
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DrawerMngr->GetDeltaTime()) *
		Util::Matrix4x4::Mtrans(Target));
}