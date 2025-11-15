#include "Character.hpp"
#include "PlayerManager.hpp"

void Plane::CheckDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	this->m_AimPoint = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Center)).pos();
	auto Pos2D = ConvWorldPosToScreenPos(this->m_AimPoint.get());
	if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
		this->m_AimPoint2D.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
		this->m_AimPoint2D.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
	}
}

inline void PlaneCommon::Update(bool w, bool s, bool a, bool d, bool q, bool e, bool attack, bool AccelKey, bool BrakeKey, bool IsAuto, const Util::Matrix4x4& TargetMat) noexcept {
	bool LeftKey = a;
	bool RightKey = d;
	bool UpKey = w;
	bool DownKey = s;
	bool QKey = q;
	bool EKey = e;
	bool AtrtackKey = attack;


	Util::Matrix4x4 Mat = TargetMat * GetMat().rotation().inverse();
	{
		float YawPer = 0.f;
		if (IsAuto) {
			YawPer = -Mat.zvec2().x * Util::deg2rad(20.f * DeltaTime);
		}
		if (QKey && !EKey) {
			YawPer = Util::deg2rad(-20.f * DeltaTime);
		}
		if (EKey && !QKey) {
			YawPer = Util::deg2rad(20.f * DeltaTime);
		}
		if (QKey && EKey) {
			YawPer = Util::deg2rad(0.f * DeltaTime);
		}
		Util::Easing(&m_YawPer, YawPer * (m_Speed / GetSpeedMax()), 0.95f);
	}
	{
		float PitchPer = 0.f;
		if (IsAuto) {
			PitchPer = Mat.zvec2().y * Util::deg2rad(50.f * DeltaTime);
		}
		if (UpKey && !DownKey) {
			PitchPer = Util::deg2rad(-50.f * DeltaTime);
		}
		if (DownKey && !UpKey) {
			PitchPer = Util::deg2rad(50.f * DeltaTime);
		}
		if (DownKey && UpKey) {
			PitchPer = Util::deg2rad(0.f * DeltaTime);
		}
		Util::Easing(&m_PtichPer, PitchPer * (m_Speed / GetSpeedMax()), 0.95f);
	}
	{
		float RollPer = 0.f;
		if (IsAuto) {
			if (m_PtichPer <= Util::deg2rad(-5.f * DeltaTime)) {
				RollPer = Util::deg2rad(100.f * DeltaTime) * std::clamp(m_YawPer / Util::deg2rad(20.f * DeltaTime), -1.f, 1.f);
			}
			if (std::fabsf(m_YawPer) >= Util::deg2rad(2.f * DeltaTime)) {
				RollPer = Util::deg2rad(100.f * DeltaTime) * std::clamp(m_YawPer / Util::deg2rad(20.f * DeltaTime), -1.f, 1.f);
			}
			if (std::fabsf(m_PtichPer) <= Util::deg2rad(5.f * DeltaTime) && std::fabsf(m_YawPer) <= Util::deg2rad(2.f * DeltaTime)) {
				RollPer = Util::deg2rad(-100.f * DeltaTime) * std::clamp(Mat.yvec().x / 1.f, -1.f, 1.f);
			}
		}
		if (LeftKey && !RightKey) {
			RollPer = Util::deg2rad(-100.f * DeltaTime);
		}
		if (RightKey && !LeftKey) {
			RollPer = Util::deg2rad(100.f * DeltaTime);
		}
		if (LeftKey && RightKey) {
			RollPer = Util::deg2rad(0.f * DeltaTime);
		}
		Util::Easing(&m_RollPer, RollPer * (m_Speed / GetSpeedMax()), 0.95f);
	}
	// 左右回転
	{
		//ヨー
		{
			float Per = std::clamp(m_YawPer, Util::deg2rad(-20.f * DeltaTime), Util::deg2rad(20.f * DeltaTime));
			this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.yvec(), Per);
		}
		//ピッチ
		{
			float Per = std::clamp(m_PtichPer, Util::deg2rad(-50.f * DeltaTime), Util::deg2rad(50.f * DeltaTime));
			this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.xvec(), Per);
		}
		//ロール
		{
			float Per = std::clamp(m_RollPer, Util::deg2rad(-100.f * DeltaTime), Util::deg2rad(100.f * DeltaTime));
			this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.zvec(), Per);
		}
	}

	// 進行方向に前進
	{
		if (AccelKey) {
			this->m_SpeedTarget += DeltaTime;
		}
		if (BrakeKey) {
			this->m_SpeedTarget -= DeltaTime;
		}


		auto Y = this->m_Rot.zvec2().y;
		if (std::fabsf(Y) > 0.1f) {
			this->m_SpeedTarget -= DeltaTime * ((std::fabsf(Y) - 0.1f) * (Y > 0.f ? 1.f : -1.f));

			float Speedkmh = GetSpeed() / GetSpeedMax();
			if ((Speedkmh < 200.f / 200.f)) {
				this->m_SpeedTarget += DeltaTime * 0.15f;
			}
		}
		else {
			float Speedkmh = GetSpeed() / GetSpeedMax();
			if ((Speedkmh < 200.f / 200.f)) {
				this->m_SpeedTarget += DeltaTime * 0.5f;
			}
			else if ((Speedkmh > 200.f / 200.f)) {
				this->m_SpeedTarget -= DeltaTime * 0.5f;
			}
		}

		if (std::fabsf(m_YawPer / Util::deg2rad(200.f * DeltaTime)) > 0.1f) {
			this->m_SpeedTarget -= DeltaTime * (std::fabsf(m_YawPer / Util::deg2rad(200.f * DeltaTime)) - 0.1f) * 0.5f;
		}
		if (std::fabsf(m_PtichPer / Util::deg2rad(200.f * DeltaTime)) > 0.1f) {
			this->m_SpeedTarget -= DeltaTime * (std::fabsf(m_PtichPer / Util::deg2rad(200.f * DeltaTime)) - 0.1f) * 0.5f;
		}
		if (std::fabsf(m_RollPer / Util::deg2rad(200.f * DeltaTime)) > 0.1f) {
			this->m_SpeedTarget -= DeltaTime * (std::fabsf(m_RollPer / Util::deg2rad(200.f * DeltaTime)) - 0.1f) * 0.5f;
		}

		this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedMax() / 2.f, GetSpeedMax() * 3.f / 2.f);
	}
	Util::Easing(&m_Speed, this->m_SpeedTarget, 0.95f);

	// 移動ベクトルを加算した仮座標を作成
	Util::VECTOR3D PosBefore = GetTargetPos();
	Util::VECTOR3D PosAfter;
	{
		PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -this->m_Speed, this->m_Rot);
	}
	//ヒット判定
	//TODO

	// 仮座標を反映
	this->m_Speed = std::clamp((PosAfter - PosBefore).magnitude(), 0.f, this->m_Speed);
	this->m_MyPosTarget = PosAfter;
	Util::VECTOR3D MyPos = GetMat().pos();
	Util::Easing(&MyPos, PosAfter, 0.9f);

	//this->m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
	SetMatrix(
		this->m_Rot.Get44DX() *
		Util::Matrix4x4::Mtrans(MyPos)
	);
	//
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = 1.f;

	this->m_AnimPer[static_cast<size_t>(CharaAnim::Roll)] = -std::clamp(m_RollPer, Util::deg2rad(-100.f * DeltaTime), Util::deg2rad(100.f * DeltaTime)) / Util::deg2rad(100.f * DeltaTime);
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Pitch)] = -std::clamp(m_PtichPer, Util::deg2rad(-50.f * DeltaTime), Util::deg2rad(50.f * DeltaTime)) / Util::deg2rad(50.f * DeltaTime);
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Yaw)] = std::clamp(m_YawPer, Util::deg2rad(-20.f * DeltaTime), Util::deg2rad(20.f * DeltaTime)) / Util::deg2rad(20.f * DeltaTime);


	//アニメアップデート
	for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
		SetAnim(loop).SetPer(this->m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
	SetModel().FlipAnimAll();

	//射撃
	{
		for (auto& se : this->m_ShotEffect) {
			se->SetMuzzleMat(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
		}
	}
	{
		for (auto& se : this->m_ShotEffect2) {
			se->SetMuzzleMat(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)));
		}
	}

	m_ShotSwitch = false;
	if (!AtrtackKey) {
		m_ShootTimer = 0.2f * 0.f;
		m_ShootTimer2 = 0.2f * 0.5f;
	}
	else {
		//
		if (m_ShootTimer == 0.f) {
			this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
			++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());

			this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), PlayerID);
			++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());

			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);

			m_ShootTimer = 0.2f;

			m_ShotSwitch = true;
		}
		m_ShootTimer = std::max(m_ShootTimer - DeltaTime, 0.f);
		//
		if (m_ShootTimer2 == 0.f) {
			this->m_ShotEffect2.at(static_cast<size_t>(this->m_ShotEffect2ID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)));
			++m_ShotEffect2ID %= static_cast<int>(this->m_ShotEffect2.size());

			this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)), PlayerID);
			++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());

			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);

			m_ShootTimer2 = 0.2f;

			m_ShotSwitch = true;
		}
		m_ShootTimer2 = std::max(m_ShootTimer2 - DeltaTime, 0.f);
	}
}

void Plane::Update_Chara(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	m_IsFreeView = KeyMngr->GetBattleKeyPress(Util::EnumBattle::ChangeWeapon);
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Aim)) {
		m_IsFPS ^= 1;
	}

	int LookX = 0;
	int LookY = 0;

	if (IsFPSView()) {
		if (this->m_PrevIsFPSView != IsFPSView()) {
			DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
		}
	}
	this->m_PrevIsFPSView = IsFPSView();

	int MX = DrawerMngr->GetMousePositionX();
	int MY = DrawerMngr->GetMousePositionY();
	DxLib::GetMousePoint(&MX, &MY);

	auto* pOption = Util::OptionParam::Instance();
	LookX = (MX - DrawerMngr->GetWindowDrawWidth() / 2) * pOption->GetParam(pOption->GetOptionType(Util::OptionType::XSensing))->GetSelect() / 100;
	LookY = (MY - DrawerMngr->GetWindowDrawHeight() / 2)* pOption->GetParam(pOption->GetOptionType(Util::OptionType::YSensing))->GetSelect() / 100;

	DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);
	{


		{
			this->m_RadAdd.y = Util::deg2rad(static_cast<float>(LookX) / 30.f);
			this->m_RadAdd.x = Util::deg2rad(static_cast<float>(-LookY) / 30.f);

			this->m_RadR.y = Util::AngleRange360(this->m_RadR.y + this->m_RadAdd.y);
			this->m_RadR.x = Util::AngleRange360(this->m_RadR.x + this->m_RadAdd.x);
		}
		if (IsFPSView()) {
			Util::Matrix4x4 Matt = GetMat().rotation();

			auto YT = Matt.zvec(); YT.z = std::hypotf(YT.x, YT.z); YT.x = 0.f; YT = YT.normalized();
			auto XZ = Matt.zvec(); XZ.y = 0.f; XZ = XZ.normalized();

			this->m_RadR.x = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), YT, Util::VECTOR3D::right());
			this->m_RadR.y = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), XZ, Util::VECTOR3D::up());
		}
	}

	bool IsOut = false;
	auto Matt = GetEyeMatrix().rotation();
	if (
		(GetMat().pos().x > 2000.f * Scale3DRate) ||
		(GetMat().pos().x < -2000.f * Scale3DRate) ||
		(GetMat().pos().y > 1000.f * Scale3DRate) ||
		(GetMat().pos().y < -1000.f * Scale3DRate) ||
		(GetMat().pos().z > 2000.f * Scale3DRate) ||
		(GetMat().pos().z < -2000.f * Scale3DRate)
		) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, 0.f, 0.f)).normalized());

		auto YT = Matt.zvec(); YT.z = std::hypotf(YT.x, YT.z); YT.x = 0.f; YT = YT.normalized();
		auto XZ = Matt.zvec(); XZ.y = 0.f; XZ = XZ.normalized();

		this->m_RadR.x = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), YT, Util::VECTOR3D::right());
		this->m_RadR.y = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), XZ, Util::VECTOR3D::up());

		IsOut = true;
	}

	Update(
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::W) || (m_IsFPS && LookY > 1),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::S) || (m_IsFPS && LookY < -1),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::A),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::D),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q) || (m_IsFPS && LookX < -3),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::E) || (m_IsFPS && LookX > 3),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Jump),
		IsOut || !m_IsFreeView && !m_IsFPS, Matt);
	{
		{
			float Per = 0.f;
			{
				float RadDif = this->m_RadR.y - this->m_Rad.y;
				if (RadDif > 0.f) {
					while (true) {
						if (RadDif < DX_PI_F) { break; }
						RadDif -= DX_PI_F * 2.f;
					}
				}
				if (RadDif < 0.f) {
					while (true) {
						if (RadDif > -DX_PI_F) { break; }
						RadDif += DX_PI_F * 2.f;
					}
				}
				Per = std::clamp(RadDif / Util::deg2rad(15.f), -1.f, 1.f);
			}
			if (std::fabsf(Per) > 0.01f) {
				float Power = 0.5f;
				this->m_Rad.y += Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			else {
				this->m_Rad.y = this->m_RadR.y;
			}
		}
		{
			this->m_RadR.x = std::clamp(this->m_RadR.x, Util::deg2rad(-89), Util::deg2rad(89));

			float Per = 0.f;
			{
				float RadDif = this->m_RadR.x - this->m_Rad.x;
				if (RadDif > 0.f) {
					while (true) {
						if (RadDif < DX_PI_F) { break; }
						RadDif -= DX_PI_F * 2.f;
					}
				}
				if (RadDif < 0.f) {
					while (true) {
						if (RadDif > -DX_PI_F) { break; }
						RadDif += DX_PI_F * 2.f;
					}
				}
				Per = std::clamp(RadDif / Util::deg2rad(15.f), -1.f, 1.f);
			}
			if (std::fabsf(Per) > 0.01f) {
				float Power = 0.5f;
				this->m_Rad.x += Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			else {
				this->m_Rad.x = this->m_RadR.x;
			}
		}
	}
}

void EnemyPlane::CheckDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	this->m_AimPoint = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Center)).pos();
	auto Pos2D = ConvWorldPosToScreenPos(this->m_AimPoint.get());
	if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
		this->m_AimPoint2D.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
		this->m_AimPoint2D.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
	}
}

void EnemyPlane::Update_Chara(void) noexcept {
	auto& Player = ((std::shared_ptr<Plane>&)PlayerManager::Instance()->SetPlane().at(0));

	auto TargetPos = Player->GetMat().pos();


	for (auto& c : PlayerManager::Instance()->SetPlane()) {
		int index = static_cast<int>(&c - &PlayerManager::Instance()->SetPlane().front());
		if (GetPlayerID() == index) { continue; }
		auto Vec = (c->GetMat().pos() - GetMat().pos());
		if (Vec.magnitude() < (10.f * Scale3DRate)) {
			TargetPos = (GetMat().pos() + Vec);
			break;
		}
	}

	m_TargetMat = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - TargetPos).normalized());

	//m_TargetMat = GetMat().rotation();
	bool IsOut = false;
	auto Matt = m_TargetMat;
	if (
		(GetMat().pos().x > 2000.f * Scale3DRate) ||
		(GetMat().pos().x < -2000.f * Scale3DRate) ||
		(GetMat().pos().y > 1000.f * Scale3DRate) ||
		(GetMat().pos().y < -1000.f * Scale3DRate) ||
		(GetMat().pos().z > 2000.f * Scale3DRate) ||
		(GetMat().pos().z < -2000.f * Scale3DRate)
		) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, 0.f, 0.f)).normalized());

		IsOut = true;
	}

	bool IsShot = false;
	if (!IsOut) {
		float angle = Util::VECTOR3D::Angle((Player->GetMat().pos() - GetMat().pos()).normalized(), GetMat().zvec2());
		IsShot = angle < Util::deg2rad(5);
	}

	m_AccelTimer += DeltaTime;
	if (m_AccelTimer > 5.f) {
		m_AccelTimer -= 5.f;
		float Speedkmh = GetSpeed() / GetSpeedMax();
		m_Accel = (Speedkmh < 170.f / 200.f);
		m_Brake = (Speedkmh > 250.f / 200.f);
	}
	m_AutoTimer += DeltaTime;
	if (m_AutoTimer > 5.f) {
		m_AutoTimer -= 5.f;
	}
	Update(
		false, false, false, false, false, false,
		IsShot,
		m_Accel,
		m_Brake,
		IsOut || (m_AutoTimer > 2.5f), Matt);

}

void Ammo::Update_Sub(void) noexcept {
	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DeltaTime, 0.f);
	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DeltaTime, 0.f);
	//this->YVecAdd -= GravAccel;
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
	SetMatrix(GetMat().rotation() *
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DeltaTime) *
		Util::Matrix4x4::Mtrans(Target));
}