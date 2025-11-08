#include "Character.hpp"

#include "Gun.hpp"

namespace HB {
	void HitBoxControl::Update(const BaseObject* ptr, float SizeRate) noexcept {
		SizeRate *= Scale3DRate;
		auto GetFrameWorldMat = [&](CharaFrame frame) {
			return ptr->GetModel().GetFrameLocalWorldMatrix(ptr->GetFrame(static_cast<int>(frame)));
			};
		size_t ID = 0;
		this->m_HitBox[ID].Update(GetFrameWorldMat(CharaFrame::Head).pos(), 0.13f * SizeRate, HitType::Head); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat(CharaFrame::Head).pos() + GetFrameWorldMat(CharaFrame::Upper).pos()) / 2.0f, 0.16f * SizeRate, HitType::Body); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat(CharaFrame::Upper).pos(), 0.13f * SizeRate, HitType::Body); ++ID;

		this->m_HitBox[ID].Update((GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat((CharaFrame::RightFoot1)).pos()) / 2.0f, 0.13f * SizeRate, HitType::Body); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat(CharaFrame::Upper).pos() + GetFrameWorldMat((CharaFrame::LeftFoot1)).pos()) / 2.0f, 0.13f * SizeRate, HitType::Body); ++ID;

		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightArm)).pos() + GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.0f, 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::RightArm2)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightWrist)).pos() + GetFrameWorldMat((CharaFrame::RightArm2)).pos()) / 2.0f, 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::RightWrist)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;

		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftArm)).pos() + GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.0f, 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::LeftArm2)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftWrist)).pos() + GetFrameWorldMat((CharaFrame::LeftArm2)).pos()) / 2.0f, 0.06f * SizeRate, HitType::Arm); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::LeftWrist)).pos(), 0.06f * SizeRate, HitType::Arm); ++ID;

		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::RightFoot1)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightFoot1)).pos() + GetFrameWorldMat((CharaFrame::RightFoot2)).pos()) / 2.0f, 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::RightFoot2)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.25f + GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.75f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.5f + GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.5f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::RightFoot)).pos() * 0.75f + GetFrameWorldMat((CharaFrame::RightFoot2)).pos() * 0.25f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::RightFoot)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;

		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::LeftFoot1)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftFoot1)).pos() + GetFrameWorldMat((CharaFrame::LeftFoot2)).pos()) / 2.0f, 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::LeftFoot2)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.25f + GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.75f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.5f + GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.5f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update((GetFrameWorldMat((CharaFrame::LeftFoot)).pos() * 0.75f + GetFrameWorldMat((CharaFrame::LeftFoot2)).pos() * 0.25f), 0.095f * SizeRate, HitType::Leg); ++ID;
		this->m_HitBox[ID].Update(GetFrameWorldMat((CharaFrame::LeftFoot)).pos(), 0.095f * SizeRate, HitType::Leg); ++ID;
	}
}

void GunParam::Update(int* pTotalAmmo) noexcept {
	auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(GetUniqueID()));
	if (this->m_IsEquip) {
		switch (this->m_EquipPhase) {
		case 0:
			this->m_Per = std::clamp(this->m_Per + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_Per >= 1.f) {
				this->m_EquipPhase = 1;
			}
			break;
		case 1:
			this->m_GunPer = std::clamp(this->m_GunPer + DeltaTime / 0.1f, 0.f, 1.f);
			this->m_Per = std::clamp(this->m_Per - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_PullPer = std::clamp(this->m_PullPer + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_PullPer >= 1.f) {
				this->m_EquipPhase = 2;
			}
			break;
		case 2:
			this->m_Per = 0.f;
			this->m_PullPer = std::clamp(this->m_PullPer - DeltaTime / 0.1f, 0.f, 1.f);
			break;
		default:
			break;
		}
		if (this->m_IsGunLoad) {
			this->m_GunLoadTimer = std::clamp(this->m_GunLoadTimer + DeltaTime, 0.f, this->m_GunLoadTimerMax);
			if (0.f <= GetReloadPer() && GetReloadPer() <= 0.1f) {
				this->m_GunLoadHandPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.f, 0.1f, GetReloadPer()));
			}
			if (0.1f <= GetReloadPer() && GetReloadPer() <= 0.3f) {
				this->m_GunLoadPer = Util::Lerp(0.f, 1.f, Util::GetPer01(0.1f, 0.3f, GetReloadPer()));
			}
			if (0.6f <= GetReloadPer() && GetReloadPer() <= 0.85f) {
				this->m_GunLoadPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.6f, 0.85f, GetReloadPer()));
			}
			if (0.9f <= GetReloadPer() && GetReloadPer() <= 1.f) {
				this->m_GunLoadHandPer = Util::Lerp(1.f, 0.f, Util::GetPer01(0.9f, 1.f, GetReloadPer()));
			}
			if (GetReloadPer() == 1.f) {
				this->m_IsGunLoad = false;
				//コッキングが必要な時はコッキングする
				if (!gun->CanShot()) {
					this->m_IsCocking = true;
				}
			}
		}
		else {
			this->m_GunLoadTimer = 0.f;
			this->m_GunLoadPer = 0.f;
			this->m_GunLoadHandPer = 0.f;
		}
		if (this->m_IsCocking) {
			this->m_CockingTimer = std::clamp(this->m_CockingTimer + DeltaTime, 0.f, this->m_CockingTimerMax);
			if (GetCockingPer() == 1.f) {
				this->m_IsCocking = false;
			}
		}
		else {
			this->m_CockingTimer = 0.f;
		}
		Util::Easing(&m_GunPullPer, this->m_IsCocking ? 1.f : 0.f, 0.9f);
	}
	else {
		switch (this->m_EquipPhase) {
		case 2:
			this->m_PullPer = std::clamp(this->m_PullPer + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_PullPer >= 1.f) {
				this->m_EquipPhase = 1;
			}
			break;
		case 1:
			this->m_GunPer = std::clamp(this->m_GunPer - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_PullPer = std::clamp(this->m_PullPer - DeltaTime / 0.1f, 0.f, 1.f);
			this->m_Per = std::clamp(this->m_Per + DeltaTime / 0.1f, 0.f, 1.f);
			if (this->m_Per >= 1.f) {
				this->m_EquipPhase = 0;
			}
			break;
		case 0:
			this->m_PullPer = 0.f;
			this->m_Per = std::clamp(this->m_Per - DeltaTime / 0.1f, 0.f, 1.f);
			break;
		default:
			break;
		}
		this->m_GunLoadPer = 0.f;
		this->m_GunLoadHandPer = 0.f;
	}
	gun->SetMagPer(this->m_GunLoadPer, GetReloadPer(), pTotalAmmo);
	gun->SetCockingPer(GetCockingPer());
}

Util::Matrix4x4 Character::GetPlayerEyeMat(void) const noexcept {
	Util::Matrix4x4 Mat = GetEyeMatrix();

	Mat = Mat.rotation() *
		Util::Matrix4x4::Mtrans(
			Util::Matrix4x4::Vtrans(
				Util::VECTOR3D::vget(
					std::cosf(this->m_WalkEyeRad),
					std::fabsf(std::sinf(this->m_WalkEyeRad)) * -0.25f,
					0.f
				) * (GetMovePer01() * (0.03f * Scale3DRate)),
				Mat.rotation()
			) +
			Mat.pos());
	{
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		Mat = Util::Lerp(Mat, gun->GetScopeMat(), this->m_Handgun.GetADSPer());
	}
	{
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		Mat = Util::Lerp(Mat, gun->GetScopeMat(), this->m_Maingun.GetADSPer());
	}
	return Mat;
}

bool Character::HasLens() const noexcept{
	if (this->m_Handgun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		return gun->HasLens();
	}
	if (this->m_Maingun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		return gun->HasLens();
	}
	return false;
}

Util::Matrix4x4 Character::GetLensPos(void) const noexcept {
	if (this->m_Handgun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		return gun->GetLensPos();
	}
	if (this->m_Maingun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		return gun->GetLensPos();
	}
	return GetEyeMatrix();
}

Util::Matrix4x4 Character::GetLensSize(void) const noexcept {
	if (this->m_Handgun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		return gun->GetLensSize();
	}
	if (this->m_Maingun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		return gun->GetLensSize();
	}
	return GetEyeMatrix();
}
const Draw::GraphHandle* Character::GetReticlePtr(void) const noexcept {
	if (this->m_Handgun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		return gun->GetReticlePtr();
	}
	if (this->m_Maingun.GetIsReady()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		return gun->GetReticlePtr();
	}
	return nullptr;
}

bool Character::NeedReload(void) const noexcept {
	if (this->m_Handgun.GetIsEquip()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
		return (gun->GetAmmoNum() < gun->GetAmmoTotal() * 3 / 10);
	}
	if (this->m_Maingun.GetIsEquip()) {
		auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
		return (gun->GetAmmoNum() < gun->GetAmmoTotal() * 3 / 10);
	}
	return false;
}

void Character::CheckDraw_Sub(void) noexcept {
	if (!IsFPSView()) {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		float MX = static_cast<float>(DrawerMngr->GetMousePositionX()) * static_cast<float>(DrawerMngr->GetRenderDispWidth()) / static_cast<float>(DrawerMngr->GetDispWidth());
		float MY = static_cast<float>(DrawerMngr->GetMousePositionY()) * static_cast<float>(DrawerMngr->GetRenderDispHeight()) / static_cast<float>(DrawerMngr->GetDispHeight());
		Util::VECTOR3D Near = ConvScreenPosToWorldPos(VGet(MX, MY, 0.f));
		Util::VECTOR3D Far = ConvScreenPosToWorldPos(VGet(MX, MY, 1.f));
		Util::VECTOR3D Now = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
		auto Target = Util::Lerp(Near, Far, (Now.y - Near.y) / (Far.y - Near.y));
		this->m_AimPoint = Target;
		//この点に一番近い敵を狙う
		float Len = (1.f * Scale3DRate) * (1.f * Scale3DRate);
		for (auto& c : PlayerManager::Instance()->GetCharacter()) {
			if (c->IsPlayer()) { continue; }
			if (((std::shared_ptr<EarlyCharacter>&)c)->IsDown()) { continue; }
			auto Pos = c->GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			if (std::abs(Pos.y - Now.y) >= 0.5f * Scale3DRate) { continue; }
			//if (Util::VECTOR3D::Dot((Pos - GetMat().pos()).normalized(), GetMat().zvec()) > 0.f) { continue; }
			auto Vec = this->m_AimPoint - Pos; Vec.y = 0.f;
			if (Len > Vec.sqrMagnitude()) {
				Len = Vec.sqrMagnitude();
				Target = Pos;
				this->m_IsAutoAim = true;
			}
		}
		this->m_AimPoint = Target;
		auto Pos2D = ConvWorldPosToScreenPos(this->m_AimPoint.get());
		this->m_AimPoint2D.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
		this->m_AimPoint2D.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
	}
}

void Character::Update_Chara(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
	bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);
	bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
	bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
	uint8_t				MoveKey = 0;
	MoveKey |= LeftKey ? (1 << 0) : 0;
	MoveKey |= RightKey ? (1 << 1) : 0;
	MoveKey |= UpKey ? (1 << 2) : 0;
	MoveKey |= DownKey ? (1 << 3) : 0;
	//
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Walk)) {
		this->m_IsFPS ^= 1;
	}
	//
	if (!m_Dive.IsActive() && (this->m_CharaStyle != CharaStyle::Prone) && !(this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive() || this->m_WakeBottom)) {
		if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Jump)) {
			this->m_Dive.SetActive();
			this->m_DivePer = 1.f;
			SetAnim(static_cast<int>(CharaAnim::Dive)).SetTime(0.f);

			this->m_CharaStyle = CharaStyle::Prone;
			for (size_t loop = 0; loop < static_cast<size_t>(CharaStyle::Max); ++loop) {
				this->m_StylePer.at(loop) = (this->m_CharaStyle == static_cast<CharaStyle>(loop)) ? 1.f : 0.f;
			}

			Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
		}
	}
	//
	if (this->m_IsActive) {
		if (!this->m_Punch.IsActive() && !this->m_Armlock.IsActive() && !this->m_Dive.IsActive()) {
			if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Prone)) {
				if (this->m_CharaStyle == CharaStyle::Stand || this->m_CharaStyle == CharaStyle::Squat || this->m_CharaStyle == CharaStyle::Prone) {
					PlayMoveSound();
					if (this->m_CharaStyle == CharaStyle::Stand) {
						this->m_CharaStyle = CharaStyle::Squat;
					}
					else if (this->m_CharaStyle == CharaStyle::Squat) {
						this->m_CharaStyle = CharaStyle::Prone;
					}
					else {
						this->m_CharaStyle = CharaStyle::Squat;
					}
				}
			}
			else if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Squat)) {
				if (this->m_CharaStyle == CharaStyle::Stand || this->m_CharaStyle == CharaStyle::Squat) {
					PlayMoveSound();
				}
				if (this->m_CharaStyle != CharaStyle::Squat) {
					this->m_CharaStyle = CharaStyle::Squat;
				}
				else {
					this->m_CharaStyle = CharaStyle::Stand;
				}
			}
			else {
				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Run)) {
					if (this->m_CharaStyle == CharaStyle::Prone) {
						PlayMoveSound();
						this->m_CharaStyle = CharaStyle::Squat;
					}
					else {
						if (this->m_CharaStyle == CharaStyle::Squat) {
							PlayMoveSound();
						}
						this->m_CharaStyle = CharaStyle::Run;
					}
				}
				if (KeyMngr->GetBattleKeyReleaseTrigger(Util::EnumBattle::Run)) {
					if (this->m_CharaStyle != CharaStyle::Squat) {
						this->m_CharaStyle = CharaStyle::Stand;
					}
				}
			}
		}

		if (this->m_Handgun.GetIsReload() || this->m_Handgun.GetIsCocking()) {
			SetArmAnim(this->m_ReloadHandgunAnimIndex);
		}
		else if (this->m_Maingun.GetIsReload() || this->m_Maingun.GetIsCocking()) {
			SetArmAnim(this->m_ReloadRifleAnimIndex);
		}
		else if (this->m_Handgun.GetIsEquip()) {
			SetArmAnim(this->m_HaveHandgunAnimIndex);
		}
		else if (this->m_Maingun.GetIsEquip()) {
			SetArmAnim(this->m_HaveRifleAnimIndex);
		}
		else {
			if (GetMovePer01() > 0.5f) {
				if (this->m_CharaStyle == CharaStyle::Run) {
					SetArmAnim(this->m_RunAnimIndex);
				}
				else {
					SetArmAnim(this->m_WalkAnimIndex);
				}
			}
			else {
				SetArmAnim(this->m_StandAnimIndex);
			}
		}
	}
	//
	{
		bool IsMoving = false;
		switch (this->m_CharaStyle) {
		case CharaStyle::Run:
			if (SetAnim(static_cast<int>(CharaAnim::Run)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::Run)).GetTime();

				//L
				if ((9.0f / 35.f * 16.f < Time && Time < 10.0f / 35.f * 16.f)) {
					if (this->m_FootSoundID != 0) {
						this->m_FootSoundID = 0;
						PlayFootSound(255);
					}
				}
				//R
				if ((27.0f / 35.f * 16.f < Time && Time < 28.0f / 35.f * 16.f)) {
					if (this->m_FootSoundID != 1) {
						this->m_FootSoundID = 1;
						PlayFootSound(255);
					}
				}
			}
			break;
		case CharaStyle::Squat:
			if (SetAnim(static_cast<int>(CharaAnim::SquatWalk)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::SquatWalk)).GetTime();

				//L
				if ((9.0f < Time && Time < 10.0f)) {
					if (this->m_FootSoundID != 2) {
						this->m_FootSoundID = 2;
						PlayFootSound(92);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 3) {
						this->m_FootSoundID = 3;
						PlayFootSound(92);
					}
				}
			}
			break;
		case CharaStyle::Stand:
			if (SetAnim(static_cast<int>(CharaAnim::Walk)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(CharaAnim::Walk)).GetTime();

				//L
				if ((9.0f < Time && Time < 10.0f)) {
					if (this->m_FootSoundID != 4) {
						this->m_FootSoundID = 4;
						PlayFootSound(192);
					}
				}
				//R
				if ((27.0f < Time && Time < 28.0f)) {
					if (this->m_FootSoundID != 5) {
						this->m_FootSoundID = 5;
						PlayFootSound(192);
					}
				}
			}
			break;
		case CharaStyle::Prone:
			break;
		case CharaStyle::Max:
		default:
			break;
		}
		if (!IsMoving) {
			this->m_FootSoundID = -1;
		}
	}
	// 左右回転

	int LookX = 0;
	int LookY = 0;

	if (IsFPSView()) {
		if (this->m_PrevIsFPSView != IsFPSView()) {
			DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
		}
		int MX = DrawerMngr->GetMousePositionX();
		int MY = DrawerMngr->GetMousePositionY();
		DxLib::GetMousePoint(&MX, &MY);
		LookX = MX - DrawerMngr->GetWindowDrawWidth() / 2;
		LookY = MY - DrawerMngr->GetWindowDrawHeight() / 2;
		DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
	}
	this->m_PrevIsFPSView = IsFPSView();

	float				PrevYradAdd = this->m_RadAdd.y;
	float				PrevXradAdd = this->m_RadAdd.x;

	float RadLimit = Util::deg2rad(80);
	if (this->m_CharaStyle == CharaStyle::Prone) {
		RadLimit = Util::deg2rad(10);
	}
	Util::Easing(&m_RadLimit, RadLimit, 0.9f);

	bool IsMove = (MoveKey != 0);

	Util::VECTOR2D InputVec = Util::VECTOR2D::zero();
	if (this->m_IsActive) {
		if ((MoveKey & (1 << 0)) != 0) {
			InputVec += Util::VECTOR2D::left();
		}
		if ((MoveKey & (1 << 1)) != 0) {
			InputVec += Util::VECTOR2D::right();
		}
		if ((MoveKey & (1 << 2)) != 0) {
			InputVec += Util::VECTOR2D::up();
		}
		if ((MoveKey & (1 << 3)) != 0) {
			InputVec += Util::VECTOR2D::down();
		}
		if (InputVec.sqrMagnitude() > 0.f) {
			InputVec = InputVec.normalized();
		}
	}
	if (this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Dive.IsActive()) {
		InputVec = Util::VECTOR2D::zero();
	}

	if (IsFPSView()) {
		this->m_RadAdd.y = Util::deg2rad(static_cast<float>(LookX) / 30.f);
		this->m_RadAdd.x = Util::deg2rad(static_cast<float>(LookY) / 30.f);

		if (this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive() || this->m_WakeBottom) {
			this->m_RadAdd.y = 0.f;
			this->m_RadAdd.x = 0.f;
		}

		this->m_YradR = Util::AngleRange360(this->m_YradR + this->m_RadAdd.y);

		if (this->m_CharaStyle == CharaStyle::Prone) {
			float Per = CalcYradDiff(this->m_YradR);

			if (std::fabsf(Per) > 0.01f) {
				float Power = 0.3f;
				this->m_Rad.y += Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			else {
				this->m_Rad.y = this->m_YradR;
			}
		}
		else {
			this->m_Rad.y = this->m_YradR;
		}

		this->m_Rad.x = std::clamp(this->m_Rad.x + this->m_RadAdd.x, -m_RadLimit, this->m_RadLimit);
		this->m_InputVec = Util::VECTOR2D::zero();
	}
	else {
		//Camera::Camera3D::Instance()->StopCamShake();
		//歩くより早く移動する場合
		if (GetSpeed() > (3.f * Scale3DRate * DeltaTime)) {
			Util::Easing(&m_InputVec, InputVec, 0.975f);
		}
		else {
			Util::Easing(&m_InputVec, InputVec, 0.9f);
		}
		if (std::fabsf(this->m_HitPower) > 0.5f) {
			this->m_InputVec.x = -m_HitVec.x;
			this->m_InputVec.y = -m_HitVec.z;
		}
		if (this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive() || this->m_WakeBottom) {
			this->m_InputVec.x = 0.f;
			this->m_InputVec.y = 0.f;
		}

		this->m_RadAdd.y = 0.f;
		this->m_RadAdd.x = 0.f;

		if (this->m_InputVec.sqrMagnitude() > 0.f) {
			float Per = CalcYradDiff(std::atan2f(-m_InputVec.x, -m_InputVec.y));
			if (std::fabsf(Per) > 0.01f) {
				float Power = 1.f;
				switch (this->m_CharaStyle) {
				case CharaStyle::Run:
					Power = 1.5f;
					break;
				case CharaStyle::Squat:
					Power = 0.5f;
					break;
				case CharaStyle::Stand:
					Power = 1.f;
					break;
				case CharaStyle::Prone:
					Power = 0.1f;
					break;
				case CharaStyle::Max:
				default:
					break;
				}
				this->m_RadAdd.y = Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			{
				float Power = 1.f;
				switch (this->m_CharaStyle) {
				case CharaStyle::Run:
					if (GetSpeed() > (3.f * Scale3DRate * DeltaTime)) {
						Power = 1.f;
					}
					else {
						Power = 0.1f;
					}
					break;
				case CharaStyle::Squat:
					Power = 0.1f;
					break;
				case CharaStyle::Stand:
					Power = 0.25f;
					break;
				case CharaStyle::Prone:
					Power = 0.1f;
					break;
				case CharaStyle::Max:
				default:
					break;
				}
				Util::Easing(&m_Rad.z, this->m_RadAdd.y * Power, 0.9f);
			}
		}

		this->m_Rad.y += this->m_RadAdd.y;
		this->m_Rad.y = Util::AngleRange360(this->m_Rad.y);

		this->m_YradR = this->m_Rad.y;

		Util::Easing(&this->m_Rad.x, 0.f, 0.9f);
	}

	Util::Easing(&m_HandRad.y, std::clamp(this->m_RadAdd.y - PrevYradAdd, -1.f, 1.f) * 30.f, 0.95f);
	Util::Easing(&m_HandRad.x, std::clamp(this->m_RadAdd.x - PrevXradAdd, -1.f, 1.f) * 30.f, 0.95f);

	Util::Easing(&m_HandRad2.y, this->m_HandRad.y, 0.95f);
	Util::Easing(&m_HandRad2.x, this->m_HandRad.x, 0.95f);

	bool IsAim = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Aim);
	if (this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneWalk)] > 0.05f) {
		IsAim = false;
	}
	if(this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive() || this->m_WakeBottom) {
		IsAim = false;
	}

	if (IsFPSView()) {
		Util::Easing(&m_Handgun.m_GunReadyPer,
			!m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && this->m_Handgun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			0.9f);

		Util::Easing(&m_Maingun.m_GunReadyPer,
			!m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && this->m_Maingun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) ? 1.f : 0.f,
			0.9f);
	}
	else {
		Util::Easing(&m_Handgun.m_GunReadyPer,
			(this->m_Handgun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) && IsAim) ? 1.f : 0.f,
			0.9f);

		Util::Easing(&m_Maingun.m_GunReadyPer,
			(this->m_Maingun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) && IsAim) ? 1.f : 0.f,
			0.9f);
	}

	Util::Easing(&m_Handgun.m_GunADSPer,
		!m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && this->m_Handgun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) && IsFPSView() && IsAim ? 1.f : 0.f,
		0.9f);
	Util::Easing(&m_Maingun.m_GunADSPer,
		!m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && this->m_Maingun.GetIsEquip() && (this->m_CharaStyle != CharaStyle::Run) && IsFPSView() && IsAim ? 1.f : 0.f,
		0.9f);
	//
	bool IsReload = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Reload);
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack)) {
		if (this->m_Handgun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
			if (gun->GetAmmoNum() == 0 && this->m_Handgun.CanShot()) {
				IsReload = true;
			}
		}
		if (this->m_Maingun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
			if (gun->GetAmmoNum() == 0 && this->m_Maingun.CanShot()) {
				IsReload = true;
			}
		}
	}
	//
	this->m_ShotSwitch = false;

	int ID = InvalidID;
	if (!this->m_Handgun.GetIsEquip() && !this->m_Maingun.GetIsEquip()
		 && !this->m_Armlocked.IsActive() && !this->m_WakeBottom
		) {
		Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
		Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);
		for (auto& c : PlayerManager::Instance()->SetCharacter()) {
			if (c->IsPlayer()) { continue; }
			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
			if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
				Util::VECTOR3D A = c->GetMat().zvec(); A.y = 0.f;
				Util::VECTOR3D B = Target - Base; B.y = 0.f;
				if (Util::VECTOR3D::Dot(A, B) > 0.f) {
				}
				else {
					ID = static_cast<int>(&c - &PlayerManager::Instance()->SetCharacter().front());
					break;
				}
			}
		}
	}
	this->m_CanArmlock = (ID != InvalidID);

	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack)) {
		if (this->m_Handgun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
			if (gun->CanShot() && this->m_Handgun.CanShot()) {
				gun->ShotStart();
				this->m_ShotSwitch = true;
				if (IsFPSView()) {
					Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				}
			}
		}
		if (this->m_Maingun.GetIsReady()) {
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
			if (gun->CanShot() && this->m_Maingun.CanShot()) {
				gun->ShotStart();
				this->m_ShotSwitch = true;
				if (IsFPSView()) {
					Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				}
			}
		}
		if (this->m_CharaStyle == CharaStyle::Stand || this->m_CharaStyle == CharaStyle::Squat) {
			if (!(this->m_Handgun.GetIsEquip() || this->m_Maingun.GetIsEquip())) {
				if (!m_CanArmlock) {
					if (!IsFreeView()) {
						//パンチ
						if (!this->m_Punch.IsActive()) {
							this->m_Punch.SetActive();
							SetAnim(static_cast<int>(CharaAnim::Combo)).SetTime(0.f);
						}
					}
				}
				else {
					if (!this->m_Armlock.IsActive()) {
						this->m_Armlock.SetActive();
						this->m_Armlock.m_Time = 2.f;
						SetAnim(static_cast<int>(CharaAnim::ArmlockStart)).SetTime(0.f);
						//IDの相手に羽交い絞めを開始させる
						this->m_ArmlockID = ID;
						if (this->m_ArmlockID != InvalidID) {
							((std::shared_ptr<EarlyCharacter>&)PlayerManager::Instance()->SetCharacter().at(static_cast<size_t>(this->m_ArmlockID)))->SetArmlocked(this->GetObjectID());
						}

						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockID)->Play3D(GetMat().pos(), 10.f * Scale3DRate, DX_PLAYTYPE_LOOP);
						Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
					}
				}
			}
		}
	}

	if (this->m_Armlock.IsActive() && SetAnim(static_cast<int>(CharaAnim::ArmlockStart)).GetTimePer() >= 1.f) {
		if (KeyMngr->GetBattleKeyRelease(Util::EnumBattle::Attack) && this->m_Armlock.m_Time == 0.f) {
			if (!this->m_Armlock.m_End && !this->m_ArmlockInjector) {
				this->m_Armlock.m_End = true;
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockID)->StopAll();
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				SetAnim(static_cast<int>(CharaAnim::ArmlockEnd)).SetTime(0.f);
				if (this->m_ArmlockID != InvalidID) {
					((std::shared_ptr<EarlyCharacter>&)PlayerManager::Instance()->SetCharacter().at(static_cast<size_t>(this->m_ArmlockID)))->SetArmlockedEnd();
					this->m_ArmlockID = InvalidID;
				}
			}
		}
		if (!this->m_Armlock.m_End) {
			if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Aim)) {
				if (!this->m_ArmlockInjector) {
					this->m_ArmlockInjector = true;
					SetAnim(static_cast<int>(CharaAnim::ArmlockInjector)).SetTime(0.f);
					if (this->m_ArmlockID != InvalidID) {
						((std::shared_ptr<EarlyCharacter>&)PlayerManager::Instance()->SetCharacter().at(static_cast<size_t>(this->m_ArmlockID)))->SetArmlockedInjector();
					}
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->StimID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
					Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				}
			}
		}
	}

	this->m_Armlock.m_Time = std::max(this->m_Armlock.m_Time - DeltaTime, 0.f);
	this->m_Armlocked.m_Time = std::max(this->m_Armlocked.m_Time - DeltaTime, 0.f);
	{
		bool IsShot = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack);
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
			gun->SetTrigger(this->m_Handgun.GetIsReady() && !m_Handgun.GetIsReload() && !m_Handgun.GetIsCocking() && IsShot);
		}
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
			gun->SetTrigger(this->m_Maingun.GetIsReady() && !m_Maingun.GetIsReload() && !m_Maingun.GetIsCocking() && IsShot);
		}
	}

	if (this->m_PrevEquip != this->m_Equip) {
		PlayMoveSound();
		switch (this->m_Equip) {
		case InvalidID:
			this->m_Handgun.SetIsEquip(false);
			this->m_Maingun.SetIsEquip(false);
			break;
		case 0:
			this->m_Handgun.SetIsEquip(false);
			this->m_Maingun.SetIsEquip(true);
			break;
		case 1:
			this->m_Handgun.SetIsEquip(true);
			this->m_Maingun.SetIsEquip(false);
			break;
		default:
			break;
		}
	}
	this->m_PrevEquip = this->m_Equip;
	if (IsReload) {
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
			if (this->m_Handgun.GetCanReload() && gun->CanReload() && this->m_TotalAmmo > 0) {
				this->m_Handgun.ReloadStart();
			}
		}
		{
			auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
			if (this->m_Maingun.GetCanReload() && gun->CanReload() && this->m_TotalAmmo > 0) {
				this->m_Maingun.ReloadStart();
			}
		}
	}

	this->m_Handgun.Update(&m_TotalAmmo);
	this->m_Maingun.Update(&m_TotalAmmo);

	bool NeedAim = false;
	{
		{
			NeedAim |= this->m_Handgun.GetReadyPer() > 0.5f;
			NeedAim |= this->m_Handgun.GetLoadHandPer() > 0.5f;
			NeedAim |= this->m_Handgun.GetGunPullPer() > 0.5f;
		}
		{
			NeedAim |= this->m_Maingun.GetReadyPer() > 0.5f;
			NeedAim |= this->m_Maingun.GetLoadHandPer() > 0.5f;
			NeedAim |= this->m_Maingun.GetGunPullPer() > 0.5f;
		}
		if (this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneWalk)] > 0.5f) {
			NeedAim = false;
		}
	}

	// 進行方向に前進
	Util::Easing(&m_Speed, (IsMove || !m_IsActive) ? GetSpeedMax() : 0.f, 0.9f);

	if (this->m_Armlocked.IsActive() && !this->m_Armlocked.m_End) {
		auto& Target = (*ObjectManager::Instance()->GetObj(this->m_ArmlockedPos));
		this->m_MyPosTarget = Target->GetMat().pos();
		MyMat = Target->GetMat();
		this->m_Rot = Util::Matrix3x3::Get33DX(MyMat);
		this->m_Rad.y = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), this->m_Rot.zvec(), Util::VECTOR3D::up());
	}
	// 移動ベクトルを加算した仮座標を作成
	Util::VECTOR3D PosBefore = GetTargetPos();
	Util::VECTOR3D PosAfter;
	{
		Util::VECTOR3D Vec;
		if (IsFPSView()) {
			Vec.x = InputVec.x;
			Vec.z = InputVec.y;
		}
		else {
			Vec = Util::VECTOR3D::forward();
		}
		if (this->m_Armlocked.IsActive() || this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_WakeBottom) {
			Vec = Util::VECTOR3D::zero();
		}
		PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Vec * -GetSpeed(), this->m_Rot);

		PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(8.f * Scale3DRate * DeltaTime) * this->m_PunchPower, this->m_Rot);

		PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(5.f * Scale3DRate * DeltaTime) * this->m_DownPower, this->m_Rot);

		PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (5.f * Scale3DRate * DeltaTime) * this->m_HitPower,
			Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), this->m_HitVec));

		PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(25.f * Scale3DRate * DeltaTime) * this->m_DivePer, this->m_Rot);


		Util::Easing(&this->m_HitBack, 0.f, 0.95f);
		Util::Easing(&this->m_HitPower, 0.f, 0.9f);
		Util::Easing(&this->m_DownPower, 0.f, 0.9f);

		Util::Easing(&this->m_DivePer, 0.f, 0.9f);

		Util::Easing(&m_PunchPower, 0.f, 0.7f);
	}
	//他キャラとのヒット判定
	if (!this->m_Armlocked.IsActive() && !this->m_Armlock.IsActive() && !this->m_WakeBottom) {
		float Radius = 2.0f * 0.3f * Scale3DRate;
		for (auto& c : PlayerManager::Instance()->SetCharacter()) {
			if (c->GetObjectID() == this->GetObjectID()) { continue; }
			if (((std::shared_ptr<EarlyCharacter>&)c)->IsDown()) { continue; }
			//
			auto Vec = c->GetMat().pos() - GetMat().pos();
			float Height = std::fabsf(Vec.y);
			Vec.y = 0.f;
			float Len = Vec.magnitude();
			if (Len < Radius && Height < Radius) {
				PosAfter = PosAfter + Vec.normalized() * (Len - Radius);
			}
		}
	}
	// 壁判定
	CheckWall(PosBefore, &PosAfter, Util::VECTOR3D::zero(), Util::VECTOR3D::up()* (0.7f * Scale3DRate), Util::VECTOR3D::up()* (1.6f * Scale3DRate), 0.35f * Scale3DRate);
	if (this->m_CharaStyle == CharaStyle::Prone || this->m_WakeBottom || this->m_Dive.IsActive()) {
		Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f; PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
		CheckWall(PosBefore, &PosAfter, PosAdd, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
	}
	// 地面判定
	bool IsFall = true;
	if (this->m_CharaStyle == CharaStyle::Prone || this->m_Dive.IsActive()) {
		Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f;
		PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
		{
			Util::VECTOR3D EndPos = PosAfter + PosAdd - Util::VECTOR3D::up() * Scale3DRate;
			if (BackGround::Instance()->CheckLine(PosAfter + PosAdd + Util::VECTOR3D::up() * Scale3DRate, &EndPos)) {
				PosAfter = EndPos - PosAdd;

				Util::VECTOR3D Pos2 = PosAfter - Util::VECTOR3D::up() * Scale3DRate;
				if (BackGround::Instance()->CheckLine(PosAfter + Util::VECTOR3D::up() * Scale3DRate, &Pos2)) {
					Util::VECTOR3D Vec = (EndPos - Pos2).normalized();
					this->m_Normal = Util::VECTOR3D::Cross(Util::VECTOR3D::Cross(PosAdd, Vec), Vec).normalized();
					if (this->m_Normal.y <= 0.f) {
						this->m_Normal = Util::VECTOR3D::up();
					}
				}
				IsFall = false;
			}
		}
		{
			Util::VECTOR3D EndPos = PosAfter - Util::VECTOR3D::up() * Scale3DRate;
			if (BackGround::Instance()->CheckLine(PosAfter + Util::VECTOR3D::up() * Scale3DRate, &EndPos)) {
				EndPos += Util::VECTOR3D::up() * 0.1f * Scale3DRate;
				PosAfter = EndPos;

				if (IsFall) {
					Util::VECTOR3D Pos2 = PosAfter - PosAdd - Util::VECTOR3D::up() * Scale3DRate;
					if (BackGround::Instance()->CheckLine(PosAfter - PosAdd + Util::VECTOR3D::up() * Scale3DRate, &Pos2)) {
						Pos2 = Pos2 + PosAdd;

						Util::VECTOR3D Vec = (EndPos - Pos2).normalized() * -1.f;
						this->m_Normal = Util::VECTOR3D::Cross(Util::VECTOR3D::Cross(PosAdd, Vec), Vec).normalized();
						if (this->m_Normal.y <= 0.f) {
							this->m_Normal = Util::VECTOR3D::up();
						}
					}
				}
				IsFall = false;
			}
		}
		m_IsFall = false;
	}
	/*
	else if (this->m_CharaStyle == CharaStyle::Prone) {
		Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f; PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
		{
			if (CheckGround(&PosAfter, PosAdd, 1.f, 1.f)) {
				if (IsFall) {
					Util::VECTOR3D Pos2 = PosAfter;
					if (CheckGround(&Pos2, Util::VECTOR3D::zero(), 1.f, 1.f)) {
						Util::VECTOR3D Vec = (PosAfter - Pos2).normalized();
						this->m_Normal = Util::VECTOR3D::Cross(Util::VECTOR3D::Cross(PosAdd, Vec), Vec).normalized();
						if (this->m_Normal.y <= 0.f) {
							this->m_Normal = Util::VECTOR3D::up();
						}
					}
				}
				IsFall = false;
			}
		}
		{
			if (CheckGround(&PosAfter, Util::VECTOR3D::zero(), 1.f, 0.1f)) {
				if (IsFall) {
					Util::VECTOR3D Pos2 = PosAfter;
					if (CheckGround(&Pos2, PosAdd * -1.f, 1.f, 1.f)) {
						Util::VECTOR3D Vec = (PosAfter - Pos2).normalized() * -1.f;
						this->m_Normal = Util::VECTOR3D::Cross(Util::VECTOR3D::Cross(PosAdd * -1.f, Vec), Vec).normalized();
						if (this->m_Normal.y <= 0.f) {
							this->m_Normal = Util::VECTOR3D::up();
						}
					}
				}
				IsFall = false;
			}
		}
	}
	//*/
	else {
		this->m_Normal = Util::VECTOR3D::up();
		if (CheckGround(&PosAfter, Util::VECTOR3D::zero(), 1.f, 0.5f)) {
			IsFall = false;
		}
		m_IsFall = IsFall;
	}
	Util::Easing(&m_NormalR, this->m_Normal, 0.9f);
	bool IsFallEnd = false;
	if (IsFall) {
		// ヒットしていない際は落下させる
		this->m_Vector.y -= GravAccel * 8.f;
		PosAfter.y += this->m_Vector.y;
	}
	else {
		if (this->m_Vector.y < -2.f * Scale3DRate * DeltaTime) {
			IsFallEnd = true;
			Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.3f * Scale3DRate);
		}
		this->m_Vector.y = 0.f;
	}

	// 仮座標を反映
	this->m_Speed = std::clamp((PosAfter - PosBefore).magnitude(), 0.f, this->m_Speed);
	this->m_MyPosTarget = PosAfter;
	Util::VECTOR3D MyPos = GetMat().pos();
	Util::Easing(&MyPos, PosAfter, 0.9f);
	if (IsFallEnd) {
		MyPos.y = PosAfter.y;
	}

	this->m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
	SetMatrix(
		GetRotMat() *
		Util::Matrix4x4::RotVec2(Util::VECTOR3D::up(), this->m_NormalR.normalized()) *
		Util::Matrix4x4::Mtrans(MyPos)
	);

	//移動割合
	Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

	if (GetMovePer01() > 0.01f) {
		this->m_WalkEyeRad += Util::deg2rad(180) * DeltaTime * 60.f / (35.f / (GetSpeed() * 2.75f));
	}
	else {
		this->m_WalkEyeRad = 0.f;
	}

	for (size_t loop = 0; loop < static_cast<size_t>(CharaStyle::Max); ++loop) {
		this->m_StylePer.at(loop) = std::clamp(this->m_StylePer.at(loop) + ((this->m_CharaStyle == static_cast<CharaStyle>(loop)) ? DeltaTime / 0.3f : -DeltaTime / 0.3f), 0.f, 1.f);
	}
	//
	if (this->m_DiveAttack) {
		this->m_DiveAttack = false;
		Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
		Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);
		for (auto& c : PlayerManager::Instance()->SetCharacter()) {
			if (c->IsPlayer()) { continue; }
			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.5f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.5f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.5f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.5f * Scale3DRate), this->m_Rot);
			if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
				((std::shared_ptr<EarlyCharacter>&)c)->SetHit(Target - Base, 1.5f);
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Target, 10.f * Scale3DRate);
			}
		}
	}
	if (this->m_PunchAttack) {
		this->m_PunchAttack = false;
		Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
		Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);
		for (auto& c : PlayerManager::Instance()->SetCharacter()) {
			if (c->IsPlayer()) { continue; }
			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
			if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
				((std::shared_ptr<EarlyCharacter>&)c)->SetHit(Target - Base, 0.35f);
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Target, 10.f * Scale3DRate);
			}
		}
	}

	if (this->m_Punch.IsActive() && SetAnim(static_cast<int>(CharaAnim::Combo)).GetTimePer() >= 1.f) {
		this->m_Punch.m_Active = false;
	}
	if (this->m_ArmlockInjector && SetAnim(static_cast<int>(CharaAnim::ArmlockInjector)).GetTimePer() >= 1.f) {
		this->m_ArmlockInjector = false;
	}
	if (this->m_Armlock.m_End && SetAnim(static_cast<int>(CharaAnim::ArmlockEnd)).GetTimePer() >= 1.f) {
		this->m_Armlock.m_End = false;
		this->m_Armlock.m_Active = false;
	}
	if (this->m_Armlocked.m_End && SetAnim(static_cast<int>(CharaAnim::ArmlockedEnd)).GetTimePer() >= 1.f) {
		this->m_Armlocked.m_End = false;
		this->m_Armlocked.m_Active = false;

		//起き上がる
		SetAnim(static_cast<int>(CharaAnim::Wakeup)).SetTime(0.f);
		this->m_WakeBottom = true;
		this->m_Armlocked.m_Time = 3.f;
	}
	if (this->m_WakeBottom) {
		if (SetAnim(static_cast<int>(CharaAnim::Wakeup)).GetTimePer() >= 1.f) {
			this->m_WakeBottom = false;
		}
	}

	if (this->m_Dive.IsActive()) {
		if (SetAnim(static_cast<int>(CharaAnim::Dive)).GetTimePer() >= 1.f) {
			this->m_Dive.m_Active = false;
		}
	}

	//
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockedStart)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockedEnd)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockStart)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockInjector)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockEnd)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::Combo)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneAim)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Prone)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::Wakeup)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::Squat)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Stay)] = 0.f;
	//移動
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneWalk)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::SquatWalk)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Walk)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Run)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Dive)] = 0.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::Fall)] = std::clamp(this->m_AnimPer[static_cast<size_t>(CharaAnim::Fall)] + (m_IsFall ? DeltaTime : -DeltaTime) / 0.2f, 0.f, 1.f);

	if (this->m_Armlock.IsActive()) {
		if (!this->m_Armlock.m_End) {
			if (!this->m_ArmlockInjector) {
				this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockStart)] = 1.f;
				SetAnim(static_cast<int>(CharaAnim::ArmlockStart)).Update(false, 1.f);
			}
			else {
				this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockInjector)] = 1.f;
				SetAnim(static_cast<int>(CharaAnim::ArmlockInjector)).Update(false, 1.f);
			}
		}
		else {
			this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockEnd)] = 1.f;
			SetAnim(static_cast<int>(CharaAnim::ArmlockEnd)).Update(false, 1.f);
		}
	}
	else if (this->m_Armlocked.IsActive()) {
		if (!this->m_Armlocked.m_End) {
			this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockedStart)] = 1.f;
			SetAnim(static_cast<int>(CharaAnim::ArmlockedStart)).Update(false, 1.f);
		}
		else {
			this->m_AnimPer[static_cast<size_t>(CharaAnim::ArmlockedEnd)] = 1.f;
			SetAnim(static_cast<int>(CharaAnim::ArmlockedEnd)).Update(false, 1.f);

			float Now = SetAnim(static_cast<int>(CharaAnim::ArmlockedEnd)).GetTime();
			if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(this->m_Armlocked.m_AnimTimer)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, DownHumanID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			}
			this->m_Armlocked.m_AnimTimer = Now;
		}
	}
	else if (this->m_Dive.IsActive()) {
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Dive)] = 1.f;
		SetAnim(static_cast<int>(CharaAnim::Dive)).Update(false, 1.f);

		float Now = SetAnim(static_cast<int>(CharaAnim::Dive)).GetTime();
		if (static_cast<int>(Now) == 1 && static_cast<int>(Now) != static_cast<int>(this->m_Dive.m_AnimTimer)) {
			this->m_DiveAttack = true;
		}
		if (static_cast<int>(Now) == 9 && static_cast<int>(Now) != static_cast<int>(this->m_Dive.m_AnimTimer)) {
			this->m_DiveAttack = true;
		}
		this->m_Dive.m_AnimTimer = Now;
	}
	else if (this->m_WakeBottom) {
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Wakeup)] = 1.f;
		SetAnim(static_cast<int>(CharaAnim::Wakeup)).Update(false, 1.f);
	}
	else if (this->m_Punch.IsActive()) {
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Combo)] = 1.f;
		SetAnim(static_cast<int>(CharaAnim::Combo)).Update(false, 1.f);

		float Now = SetAnim(static_cast<int>(CharaAnim::Combo)).GetTime();
		if (static_cast<int>(Now) == 1 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_AnimTimer)) {
			Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			this->m_PunchPower = 0.2f;
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			this->m_PunchAttack = true;
		}
		if (static_cast<int>(Now) == 9 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_AnimTimer)) {
			Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			this->m_PunchPower = 0.2f;
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			this->m_PunchAttack = true;
		}
		if (static_cast<int>(Now) == 17 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_AnimTimer)) {
			Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
			this->m_PunchPower = 1.f;
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_KickID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			this->m_PunchAttack = true;
		}
		this->m_Punch.m_AnimTimer = Now;
	}
	else {
		//停止
		if (NeedAim) {
			this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneAim)] = (1.f - GetMovePer01()) * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
		}
		else {
			this->m_AnimPer[static_cast<size_t>(CharaAnim::Prone)] = (1.f - GetMovePer01()) * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
		}


		this->m_AnimPer[static_cast<size_t>(CharaAnim::Squat)] = (1.f - GetMovePer01()) * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = (1.f - GetMovePer01()) * std::max(this->m_StylePer.at(static_cast<size_t>(CharaStyle::Stand)), this->m_StylePer.at(static_cast<size_t>(CharaStyle::Run)));
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Stay)] = (1.f - std::max(this->m_Handgun.GetReadyPer(), this->m_Maingun.GetReadyPer()));
		//移動
		this->m_AnimPer[static_cast<size_t>(CharaAnim::ProneWalk)] = GetMovePer01() * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
		this->m_AnimPer[static_cast<size_t>(CharaAnim::SquatWalk)] = GetMovePer01() * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Squat));
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Walk)] = GetMovePer01() * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Stand));
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Run)] = GetMovePer01() * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Run));
		/*
		//回転
		{
			float Per = 0.f;
			if (!IsFPSView()) {
				if (IsFreeView() && (this->m_CharaStyle != CharaStyle::Run) && !GetIsReloading()) {
					Per = -Util::VECTOR3D::SignedAngle(GetMat().zvec() * -1.f, this->m_AimPoint - GetMat().pos(), Util::VECTOR3D::up()) / Util::deg2rad(90);
				}
				else {
					Per = this->m_YradDif / Util::deg2rad(90);
				}
			}
			this->m_CanAim = std::fabsf(Per) < 1.f;

			Per *= (1.f - this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)));

			Util::Easing(&m_SwitchPer, (Per >= -0.5f) ? 1.f : 0.f, 0.9f);
			Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::FlipLeft)], std::clamp(Per, 0.f, 1.f), 0.9f);
			Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::FlipRight)], std::clamp(Per, -1.f, 0.f) * -1.f, 0.9f);
		}
		//*/
	}
	//
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_1)] = 1.f;
	{
		float Per = 0.3f;
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
			Per = 0.45f;
		}
		if (GetIsReloading()) {
			Per = 0.0f;
		}
		Util::Easing(&m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_2)], Per, 0.8f);
	}
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_3)] = 1.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_4)] = 1.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::ReftHand_5)] = 1.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_1)] = 0.f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_2)] = 0.3f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_3)] = 0.5f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_4)] = 0.7f;
	this->m_AnimPer[static_cast<size_t>(CharaAnim::LeftHand_5)] = 0.9f;

	//アニメアップデート
	for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
		SetAnim(loop).SetPer(this->m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
	SetAnim(static_cast<int>(CharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::Run)).Update(true, GetSpeed() * 0.75f);
	SetAnim(static_cast<int>(CharaAnim::Squat)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::SquatWalk)).Update(true, GetSpeed() * 2.75f);
	SetAnim(static_cast<int>(CharaAnim::ProneWalk)).Update(true, GetSpeed() * 8.f);
	SetModel().FlipAnimAll();

	Util::Matrix4x4 HandBaseMat = GetEyeMatrix();

	HandBaseMat =
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), -m_HandRad2.y) *
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_HandRad2.x) *
		HandBaseMat.rotation() *
		Util::Matrix4x4::Mtrans(HandBaseMat.pos());

	{
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper));
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper2));
		ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Neck));

		//回転
		{
			float Per = 0.f;
			if (!IsFPSView()) {
				if (IsFreeView() && (this->m_CharaStyle != CharaStyle::Run) && !GetIsReloading()) {
					Per = -Util::VECTOR3D::SignedAngle(GetEyeMatrix().zvec() * -1.f, this->m_AimPoint - GetEyeMatrix().pos(), Util::VECTOR3D::up());
				}
				else {
					Per = -this->m_YradDif;
				}
			}
			this->m_CanAim = std::fabsf(Per / Util::deg2rad(90)) < 1.f;
			Util::Easing(&this->m_SwitchPer, (Per / Util::deg2rad(90) >= -0.5f) ? 1.f : 0.f, 0.9f);

			float Per2 = Per * this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone));
			float Per3 = Per * (1.f - this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)));
			Util::Easing(&this->m_YradProne, std::clamp(-Per2, Util::deg2rad(-30), Util::deg2rad(30)), 0.9f);
			Util::Easing(&this->m_YradUpper, std::clamp(-Per3, Util::deg2rad(-90), Util::deg2rad(90)), 0.9f);
		}

		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_YradUpper * 0.3f)*
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -m_Rad.x * 0.6f) *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), this->m_YradProne * 0.3f) *

			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_HitBack * Util::deg2rad(90.f))*

			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper))
		);
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_YradUpper * 0.3f)*
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), -m_Rad.x * 0.4f) *
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), this->m_YradProne * 0.3f)*
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper2))
		);
		SetFrameLocalMatrix(static_cast<int>(CharaFrame::Neck),
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), m_YradUpper * 0.4f)*
			Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), this->m_YradProne * 0.4f)*
			GetFrameLocalMatrix(static_cast<int>(CharaFrame::Neck))
		);
	}
	{
		Util::HandAnimPool::Instance()->Update();
		if (this->m_AnimMoving) {
			this->m_AnimChangePer = std::clamp(this->m_AnimChangePer + DeltaTime / 0.25f, 0.f, 1.f);
			if (this->m_AnimChangePer >= 1.f) {
				this->m_AnimMoving = false;
				this->m_Prev = this->m_Now;
			}
		}
		Util::VRAnim	VAnim = Util::VRAnim::LerpAnim(
			Util::HandAnimPool::Instance()->GetAnim(this->m_Prev).GetAnim(),
			Util::HandAnimPool::Instance()->GetAnim(this->m_Now).GetAnim(),
			this->m_AnimChangePer);

		Util::VRAnim Answer;
		{
			auto& gun = (*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
			auto EyePos =
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					this->m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(VAnim, AimAnim, this->m_Handgun.GetReadyPer());
		}
		{
			auto& gun = (*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
			auto EyePos =
				Util::Lerp(
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosRight)),
					gun->GetFrameBaseLocalMat(static_cast<int>(GunFrame::EyePosLeft)),
					this->m_SwitchPer
				);
			Util::VRAnim	AimAnim;
			AimAnim.m_RightHandPos = EyePos.pos() * -1.f;
			AimAnim.m_RightRot = Util::Matrix3x3::Get33DX(EyePos);

			Answer = Util::VRAnim::LerpAnim(Answer, AimAnim, this->m_Maingun.GetReadyPer());
		}
		{
			Util::Matrix4x4 RightHandMat = Answer.m_RightRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(Answer.m_RightHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());

			RightHandMat = Util::Lerp(Util::Lerp(RightHandMat, GetHolsterMat(), this->m_Handgun.GetPer()), GetHolsterPullMat(), this->m_Handgun.GetPullPer());
			RightHandMat = Util::Lerp(Util::Lerp(RightHandMat, GetSlingMat(), this->m_Maingun.GetPer()), GetSlingPullMat(), this->m_Maingun.GetPullPer());

			if ((this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)) > 0.5f && !NeedAim) || this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive()
				|| this->m_WakeBottom
				|| (this->m_AnimPer[static_cast<size_t>(CharaAnim::Fall)] > 0.5f)
				|| this->m_Dive.IsActive()
				) {
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm)));
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightArm2)));
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::RightWrist)));
			}
			else {
				Draw::IK_RightArm(
					&SetModel(),
					GetFrame(static_cast<int>(CharaFrame::RightArm)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
					GetFrame(static_cast<int>(CharaFrame::RightArm2)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
					GetFrame(static_cast<int>(CharaFrame::RightWrist)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
					RightHandMat
				);
			}
		}

		{
			Util::Matrix4x4 RightMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-90)) *
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), Util::deg2rad(-90)) *
				GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RightHandJoint));
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
				if (this->m_Handgun.GetInHolster()) {
					Mat = Util::Lerp(GetHolsterMat(), GetHolsterPullMat(), this->m_Handgun.GetGunPer());
				}
				gun->SetMatrix(Mat);
				gun->SetMagLoadMat(GetMagPouchMat());
			}
			{
				Util::Matrix4x4 Mat = RightMat;
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
				if (this->m_Maingun.GetInHolster()) {
					Mat = Util::Lerp(GetSlingMat(), GetSlingPullMat(), this->m_Maingun.GetGunPer());
				}
				gun->SetMatrix(Mat);
				gun->SetMagLoadMat(GetMagPouchMat());
			}
		}

		{
			Util::Matrix4x4 LeftHandMat = VAnim.m_LeftRot.Get44DX() * HandBaseMat.rotation() *
				Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(VAnim.m_LeftHandPos, HandBaseMat.rotation()) + HandBaseMat.pos());
			{
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Handgun.GetUniqueID()));
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetBaseLeftHandMat(), this->m_Handgun.GetReadyPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetMagLeftHandMat(), this->m_Handgun.GetLoadHandPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetPullLeftHandMat(), this->m_Handgun.GetGunPullPer());
			}
			{
				auto& gun = (std::shared_ptr<Gun>&)(*ObjectManager::Instance()->GetObj(this->m_Maingun.GetUniqueID()));
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetBaseLeftHandMat(), this->m_Maingun.GetReadyPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetMagLeftHandMat(), this->m_Maingun.GetLoadHandPer());
				LeftHandMat = Util::Lerp(LeftHandMat, gun->GetPullLeftHandMat(), this->m_Maingun.GetGunPullPer());
			}

			if ((this->m_StylePer.at(static_cast<size_t>(CharaStyle::Prone)) > 0.5f && !NeedAim) || this->m_Punch.IsActive() || this->m_Armlock.IsActive() || this->m_Armlocked.IsActive()
				|| this->m_WakeBottom
				|| (this->m_AnimPer[static_cast<size_t>(CharaAnim::Fall)] > 0.5f)
				|| this->m_Dive.IsActive()
				) {
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm)));
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftArm2)));
				SetModel().ResetFrameUserLocalMatrix(GetFrame(static_cast<int>(CharaFrame::LeftWrist)));
			}
			else {
				Draw::IK_LeftArm(
					&SetModel(),
					GetFrame(static_cast<int>(CharaFrame::LeftArm)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
					GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
					GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
					LeftHandMat
				);
			}
		}
	}
	//
	this->m_Injector.SetMatrix(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::LeftHandJoint)));

	this->m_IsAutoAim = false;
}