#pragma warning(disable:5259)

#include "Character.hpp"
#include "GameRule.hpp"

#include "Script.hpp"

void MyPlane::Init_Sub(void) noexcept {
	this->m_SpeedTarget = GetSpeedMax();
	this->m_Speed = this->m_SpeedTarget;

	this->m_CockPitIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_CockPitID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_CockPitID)->SetLocalVolume(0);

	this->m_EngineIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(0);

	this->m_LineEffect1.Init(0.25f, 0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
	this->m_LineEffect2.Init(0.25f, 0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);

	this->m_LineEffect3.Init(0.05f, 0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
	this->m_LineEffect4.Init(0.05f, 0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
}
void MyPlane::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	//
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_CockPitID)->SetPosition(this->m_CockPitIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_CockPitID)->SetLocalVolume(255);
	//
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetPosition(this->m_EngineIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(255);
	//
	this->m_DamageInterval = std::max(this->m_DamageInterval - DrawerMngr->GetDeltaTime(), 0.f);
	//移動
	{
		Util::VECTOR3D MoveVec = Util::VECTOR3D::forward();
		float RollingCam = 0.f;
		//上下
		{
			bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
			bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
			float prev = this->m_MovePointAdd.y;
			if (UpKey && !DownKey) {
				this->m_MovePointAdd.y -= 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.y = -0.5f;
			}
			if (DownKey && !UpKey) {
				this->m_MovePointAdd.y += 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.y = 0.5f;
			}
			if (!UpKey && !DownKey) {
				if (this->m_MovePointAdd.y < -12.f * Scale3DRate) {
					this->m_MovePointAdd.y += 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
					MoveVec.y = 0.5f;
				}
			}
			this->m_MovePointAdd.y = std::clamp(this->m_MovePointAdd.y, -96.f * Scale3DRate, 12.f * Scale3DRate);
			if (prev == this->m_MovePointAdd.y) {
				MoveVec.y = 0.0f;
			}
		}
		//ロール
		{
			bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
			bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);

			bool Left2Key = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q);
			bool Right2Key = KeyMngr->GetBattleKeyPress(Util::EnumBattle::E);

			bool Left2Trig = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Q);
			bool Right2Trig = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::E);
			if (this->m_OutsidePer > 0.f) {
				LeftKey = false;
				RightKey = false;

				float X = (this->m_OutsideMatAfter * this->m_OutsideMatBefore.inverse()).zvec2().x;
				Left2Key = X > 0.f;
				Right2Key = X < 0.f;

				Left2Trig = false;
				Right2Trig = false;
			}
			float prev = this->m_MovePointAdd.x;
			if (LeftKey && !RightKey) {
				this->m_MovePointAdd.x -= 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = -0.6f;
				RollingCam = Util::deg2rad(-10);
			}
			if (RightKey && !LeftKey) {
				this->m_MovePointAdd.x += 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = 0.6f;
				RollingCam = Util::deg2rad(10);
			}
			switch (GameRule::Instance()->GetGameType()) {
			case GameType::Normal:
				this->m_MovePointAdd.x = std::clamp(this->m_MovePointAdd.x, -18.f * Scale3DRate, 18.f * Scale3DRate);
				if (prev == this->m_MovePointAdd.x) {
					MoveVec.x = 0.0f;
					RollingCam = Util::deg2rad(0);
				}
				break;
			case GameType::AllRange:
				this->m_MovePointAdd.x = std::clamp(this->m_MovePointAdd.x, -10.f * Scale3DRate, 10.f * Scale3DRate);
				break;
			case GameType::Max:
			default:
				break;
			}

			float RollPer = 0.f;
			RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
			Util::VECTOR3D YVec = (GetMat() * RailMat.inverse()).yvec();
			if (YVec.y > 0.f) {
				RollPer *= YVec.x;
			}
			else {
				RollPer *= (YVec.x > 0.f) ? 1.f : -1.f;
			}
			switch (GameRule::Instance()->GetGameType()) {
			case GameType::Normal:
				if (prev != this->m_MovePointAdd.x) {
					if (LeftKey && !RightKey) {
						RollPer = Util::deg2rad(-30);
					}
					if (RightKey && !LeftKey) {
						RollPer = Util::deg2rad(30);
					}
				}
				break;
			case GameType::AllRange:
				if (LeftKey && !RightKey) {
					RollPer = Util::deg2rad(-30);
				}
				if (RightKey && !LeftKey) {
					RollPer = Util::deg2rad(30);
				}
				if (!RightKey && !LeftKey) {
					this->m_MovePointAdd.x = std::clamp(this->m_MovePointAdd.x, -5.f * Scale3DRate, 5.f * Scale3DRate);
				}
				break;
			case GameType::Max:
			default:
				break;
			}

			this->m_RollingTimer1 = std::max(this->m_RollingTimer1 - DrawerMngr->GetDeltaTime(), 0.f);
			this->m_RollingTimer2 = std::max(this->m_RollingTimer2 - DrawerMngr->GetDeltaTime(), 0.f);
			this->m_RollingInputTimer1 = std::max(this->m_RollingInputTimer1 - DrawerMngr->GetDeltaTime(), 0.f);
			this->m_RollingInputTimer2 = std::max(this->m_RollingInputTimer2 - DrawerMngr->GetDeltaTime(), 0.f);
			if (Left2Trig && !Right2Trig) {
				if (this->m_RollingInputTimer1 != 0.f) {
					this->m_RollingTimer1 = 0.5f;
				}
				this->m_RollingInputTimer1 = 0.3f;
			}
			if (Right2Trig && !Left2Trig) {
				if (this->m_RollingInputTimer2 != 0.f) {
					this->m_RollingTimer2 = 0.5f;
				}
				this->m_RollingInputTimer2 = 0.3f;
			}
			if (Left2Key && !Right2Key) {
				RollPer = Util::deg2rad(-90);
				RollingCam = Util::deg2rad(-10);
			}
			if (Right2Key && !Left2Key) {
				RollPer = Util::deg2rad(90);
				RollingCam = Util::deg2rad(10);
			}

			if (this->m_RollingTimer1 > 0.f) {
				this->m_RollPer = Util::AngleRange180(this->m_RollPer - Util::deg2rad(360 / 0.5f) * DrawerMngr->GetDeltaTime());
				Util::Easing(&m_RollingBarrier, 1.f, 0.9f);
			}
			else if (this->m_RollingTimer2 > 0.f) {
				this->m_RollPer = Util::AngleRange180(this->m_RollPer + Util::deg2rad(360 / 0.5f) * DrawerMngr->GetDeltaTime());
				Util::Easing(&m_RollingBarrier, 1.f, 0.9f);
			}
			else {
				Util::Easing(&m_RollPer, RollPer, 0.95f);
				this->m_RollingBarrier = std::clamp(this->m_RollingBarrier - DrawerMngr->GetDeltaTime() / 0.25f, 0.f, 1.f);
			}

			this->m_Roll = Util::Matrix3x3::RotAxis(this->m_Roll.zvec(), this->m_RollPer);
		}
		Util::Easing(&m_MoveVec, MoveVec, 0.95f);
		Util::Easing(&m_RollingCam, RollingCam, 0.975f);
		Util::Easing(&m_MovePoint, this->m_MovePointAdd, 0.9f);
	}
	// 進行方向に前進
	{
		this->m_ManeuverInputTimer = std::max(this->m_ManeuverInputTimer - DrawerMngr->GetDeltaTime(), 0.f);
		this->m_ManeuverPer = std::max(this->m_ManeuverPer - DrawerMngr->GetDeltaTime(), 0.f);
		if (this->m_ManeuverPer == 0.f) {
			this->m_ManeuverIDRe = InvalidID;
		}
		bool BrakeTrig = this->m_ManeuverID != InvalidID && !m_Stall && this->m_StallPer < 0.75f && KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Brake);
		if (BrakeTrig) {
			if (this->m_ManeuverInputTimer != 0.f) {
				this->m_ManeuverPer = 1.f;
				this->m_ManeuverIDRe = this->m_ManeuverID;
				this->m_StallPer += 0.75f;
			}
			this->m_ManeuverInputTimer = 0.3f;
		}

		bool AccelKey = !m_OverHeat && KeyMngr->GetBattleKeyPress(Util::EnumBattle::Throttle);
		bool BrakeKey = !m_Stall && KeyMngr->GetBattleKeyPress(Util::EnumBattle::Brake);
		if (!AccelKey && !BrakeKey) {
			this->m_SpeedTarget = GetSpeedMax();
		}
		if (AccelKey && !BrakeKey) {
			this->m_SpeedTarget += 10.f * DrawerMngr->GetDeltaTime();
			this->m_BoostPer += DrawerMngr->GetDeltaTime() / 3.f;
		}
		else {
			if (!m_OverHeat) {
				this->m_BoostPer -= DrawerMngr->GetDeltaTime() / 6.f;
			}
			else {
				this->m_BoostPer -= DrawerMngr->GetDeltaTime() / 6.f;
			}
		}
		this->m_BoostPer = std::clamp(this->m_BoostPer, 0.f, 1.f);
		if (!m_OverHeat) {
			if (this->m_BoostPer == 1.f) {
				this->m_OverHeat = true;
			}
		}
		else {
			if (this->m_BoostPer == 0.f) {
				this->m_OverHeat = false;
			}
		}

		if (!AccelKey && BrakeKey) {
			this->m_SpeedTarget -= 10.f * DrawerMngr->GetDeltaTime();
			this->m_StallPer += DrawerMngr->GetDeltaTime() / 5.f;
		}
		else {
			if (!m_Stall) {
				this->m_StallPer -= DrawerMngr->GetDeltaTime() / 5.f;
			}
			else {
				this->m_StallPer -= DrawerMngr->GetDeltaTime() / 5.f;
			}
		}
		this->m_StallPer = std::clamp(this->m_StallPer, 0.f, 1.f);
		if (!m_Stall) {
			if (this->m_StallPer == 1.f) {
				this->m_Stall = true;
			}
		}
		else {
			if (this->m_StallPer == 0.f) {
				this->m_Stall = false;
			}
		}
		this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedMax() * 3.f / 4.f, GetSpeedMax() * 3.f / 2.f);
		Util::Easing(&this->m_Speed, this->m_SpeedTarget, 0.95f);
	}
	// 移動ベクトルを加算した仮座標を作成
	{
		this->m_Frame += (this->m_Speed / Scale3DRate * (60.f * DrawerMngr->GetDeltaTime()));
		Util::VECTOR3D PosBefore = RailMat.pos();
		Util::VECTOR3D PosAfter = RailMat.pos() + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), RailMat.rotation());
		if (GetIsManeuver()) {
			auto& obj = (std::shared_ptr<Enemy>&)(*ObjectManager::Instance()->GetObj(this->m_ManeuverIDRe));

			Util::Easing(&PosAfter, obj->GetRailMat().pos() + obj->GetRailMat().zvec() * (40.f * Scale3DRate), 0.925f);
			Util::Matrix4x4 Rot = RailMat.rotation();
			Util::Easing(&Rot, obj->GetRailMat().rotation(), 0.8f);
			RailMat = Rot.rotation() * Util::Matrix4x4::Mtrans(RailMat.pos());
			Util::Easing(&m_MovePointAdd, Util::VECTOR3D::vget(0.f, 12.f * Scale3DRate, 0.f), 0.925f);
		}
		switch (GameRule::Instance()->GetGameType()) {
		case GameType::Normal:
			Util::Easing(&m_RotRail, 0.f, 0.95f);
			RailMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), this->m_RotRail * Util::deg2rad(45.f) * DrawerMngr->GetDeltaTime()) * RailMat;
			break;
		case GameType::AllRange:
			if (this->m_MovePointAdd.x < -5.f * Scale3DRate) {
				this->m_RotRail = std::clamp(this->m_RotRail - 1.f * DrawerMngr->GetDeltaTime(), -1.f, 1.f);
			}
			else if (this->m_MovePointAdd.x > 5.f * Scale3DRate) {
				this->m_RotRail = std::clamp(this->m_RotRail + 1.f * DrawerMngr->GetDeltaTime(), -1.f, 1.f);
			}
			else {
				Util::Easing(&m_RotRail, 0.f, 0.95f);
			}
			RailMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), this->m_RotRail * Util::deg2rad(45.f) * DrawerMngr->GetDeltaTime()) * RailMat;
			//範囲外なら真ん中を向く
			if (this->m_OutsidePer <= 0.f && (PosAfter.magnitude() > 400.f * Scale3DRate)) {
				this->m_OutsidePer = 1.f;
				this->m_OutsideMatBefore = RailMat.rotation();

				Util::VECTOR3D Pos = PosAfter; Pos.y = 0.f; Pos = Pos.normalized();
				if (Pos.x == 0.f) {
					Pos.x = 0.01f;
				}
				this->m_OutsideMatAfter = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), std::atan2f(Pos.x, Pos.z)).rotation();
			}
			if (this->m_OutsidePer > 0.f) {
				this->m_OutsidePer = std::max(this->m_OutsidePer - DrawerMngr->GetDeltaTime() / 0.5f, 0.f);
				Util::Matrix4x4 Mat = RailMat.rotation();
				Util::Easing(&Mat, this->m_OutsideMatAfter, 0.95f);
				RailMat = Mat.rotation() * Util::Matrix4x4::Mtrans(PosAfter);
				Util::Easing(&m_MovePointAdd, Util::VECTOR3D::vget(0.f, std::min(this->m_MovePointAdd.y, 0.f), 0.f), 0.925f);
			}
			break;
		case GameType::Max:
		default:
			break;
		}
		//当たり判定

		RailMat = RailMat.rotation() * Util::Matrix4x4::Mtrans(PosAfter);

		switch (GameRule::Instance()->GetGameType()) {
		case GameType::Normal:
			EyeMat = RailMat;
			break;
		case GameType::AllRange:
			if (this->m_OutsidePer > 0.f) {
				EyeMat = this->m_OutsideMatAfter.rotation() * Util::Matrix4x4::Mtrans(RailMat.pos());
			}
			else {
				EyeMat = RailMat;
			}
			break;
		case GameType::Max:
		default:
			break;
		}

		Util::VECTOR3D EyePos = this->m_MovePoint * -1.f;
		bool IsDownRange = false;
		if (EyePos.y > 12.f * Scale3DRate) {
			EyePos.y = 12.f * Scale3DRate + (EyePos.y - 12.f * Scale3DRate)*1.95f;
			IsDownRange = true;
		}
		EyePos *= 0.5f;

		EyeMat = Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), this->m_RollingCam) * 
			Util::Matrix4x4::Mtrans(EyePos) * 
			EyeMat;

		Util::Easing(&m_EyeRot, IsDownRange ? this->m_MoveVec : Util::VECTOR3D::forward(), 0.9f);
		EyeMat = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), m_EyeRot.normalized()) * EyeMat;

		this->m_RePos = GetMat().pos();
		SetMatrix(
			(this->m_Roll * Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), this->m_MoveVec) * Util::Matrix3x3::Get33DX(RailMat.rotation())).Get44DX() *
			Util::Matrix4x4::Mtrans(RailMat.pos() - Util::Matrix4x4::Vtrans(this->m_MovePoint, RailMat.rotation())));
		if (this->m_RollingBarrier > 0.f) {
			this->m_barrier.SetMatrix(GetMat());
			this->m_barrier.SetOpacityRate(this->m_RollingBarrier);
		}
	}
	//アニメアップデート
	{
		if (GetIsManeuver()) {
			SetAnim(static_cast<int>(CharaAnim::Rolling)).SetPer(1.f);
			SetAnim(static_cast<int>(CharaAnim::Rolling)).Update(false, 1.f);
		}
		else {
			SetAnim(static_cast<int>(CharaAnim::Rolling)).SetPer(0.f);
			SetAnim(static_cast<int>(CharaAnim::Rolling)).SetTime(0.f);
		}
		SetModel().FlipAnimAll();
	}
	//射撃
	{
		if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Missile)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, ShotSoundID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);
			AmmoPool::Instance()->ShotBomb(GetFrameLocalWorldMatrix(static_cast<int>(m_IsLeftMissile ? CharaFrame::Missile1 : CharaFrame::Missile2)), 100.f, GetObjectID());
			m_IsLeftMissile ^= 1;
		}
		if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::MultiMissile)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, ShotSoundID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);
			AmmoPool::Instance()->ShotMultiBomb(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Missile2)), 100.f, GetObjectID());
		}
		if (!KeyMngr->GetBattleKeyPress(Util::EnumBattle::Gun)) {
			this->m_ShootTimer = 0.f;
		}
		else {
			if (this->m_ShootTimer == 0.f) {
				EffectPool::Instance()->Shot(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), 1.f);
				AmmoPool::Instance()->ShotAmmo(
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), GetRandf(Util::deg2rad(1)))*
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), GetRandf(Util::deg2rad(1)))*
					GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), 200.f, GetObjectID(), 1.f);

				EffectPool::Instance()->Shot(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)), 1.f);
				AmmoPool::Instance()->ShotAmmo(
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), GetRandf(Util::deg2rad(1)))*
					Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), GetRandf(Util::deg2rad(1)))*
					GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)), 200.f, GetObjectID(), 1.f);

				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 200.f * Scale3DRate);
				this->m_ShootTimer = 0.1f;
			}
			this->m_ShootTimer = std::max(this->m_ShootTimer - DrawerMngr->GetDeltaTime(), 0.f);
		}
	}
	//
	this->m_LineEffect1.Update(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::LWingtip)).pos());
	this->m_LineEffect2.Update(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::RWingtip)).pos());

	this->m_LineEffect3.Update(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Nozzle1)).pos());
	this->m_LineEffect4.Update(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Nozzle2)).pos());
}
