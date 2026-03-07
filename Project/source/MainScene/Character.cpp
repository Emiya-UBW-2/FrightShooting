#include "Character.hpp"
#include "PlayerManager.hpp"

void PlaneCommon::Load_Sub(void) noexcept {
	this->m_PropellerID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/Propeller.wav", true);
	this->m_EngineID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 10, "data/Sound/SE/engine.wav", true);
	for (auto& g : m_Gun) {
		g.Load();
	}
}
void PlaneCommon::Init_Sub(void) noexcept {
	this->m_SpeedTarget = GetSpeedBase();
	this->m_Speed = this->m_SpeedTarget;

	for (auto& g : m_Gun) {
		g.Init();
	}
	for (auto& ae : this->m_DamageEffect) {
		ae = std::make_shared<DamageEffect>();
		ObjectManager::Instance()->InitObject(ae);
	}
	m_PropellerIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	m_EngineIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(0);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(0);

	SetDamage(InvalidID);

	m_Jobs.Init(
		[&]() {
			Util::VECTOR3D Pos = GetMat().pos();
			bool IsInCloud = false;
			for (auto& c : BackGround::Instance()->GetClouds()) {
				float length = c.Scale * 626.64f;
				if ((c.Pos - Pos).sqrMagnitude() < length * length) {
					IsInCloud = true;
					break;
				}
			}
			m_IsInCloud = IsInCloud;
			//
			int CanWatchBitField = 0;
			//*
			for (auto& p : PlayerManager::Instance()->GetPlane()) {
				if (this->GetObjectID() == p->GetObjectID()) { continue; }
				int index = static_cast<int>(&p - &PlayerManager::Instance()->GetPlane().front());
				Util::VECTOR3D PPos = p->GetMat().pos();
				bool CanWatch = true;
				if (CanWatch) {
					CanWatch = (Pos - PPos).magnitude() < 1000.f * Scale3DRate;
				}
				if (CanWatch) {
					for (auto& c : BackGround::Instance()->GetClouds()) {
						float length = c.Scale * 626.64f;
						if (Util::GetMinLenSegmentToPoint(Pos, PPos, c.Pos) < length) {
							CanWatch = false;
							break;
						}
					}
				}
				if (CanWatch) {
					CanWatchBitField |= 1 << index;
				}
			}
			//*/
			m_CanWatchBitField = CanWatchBitField;
		},
		[&]() {});
}
void PlaneCommon::Update_Sub(void) noexcept {
	auto* pOption = Util::OptionParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//DamageID = InvalidID;
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetPosition(m_PropellerIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetPosition(m_EngineIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(128);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(64);
	m_Jobs.Update(true);
	if (GetHitPointLow()) {
		float UpdateTime = 2.f * DrawerMngr->GetDeltaTime();
		switch (pOption->GetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel))->GetSelect()) {
		case 0:
			UpdateTime = 6.f * DrawerMngr->GetDeltaTime();
			break;
		case 1:
			UpdateTime = 4.f * DrawerMngr->GetDeltaTime();
			break;
		case 2:
			UpdateTime = 2.f * DrawerMngr->GetDeltaTime();
			break;
		default:
			break;
		}

		m_DamageEffectTimer += DrawerMngr->GetDeltaTime();
		if (m_DamageEffectTimer > UpdateTime) {
			m_DamageEffectTimer -= UpdateTime;
			m_DamageEffect.at(m_DamageEffectNow)->Set(
				GetMat().pos(),
				GetMat().zvec2()
			);
			++m_DamageEffectNow %= m_DamageEffect.size();
		}
	}
	else {
		m_DamageEffectTimer = 0.f;
	}
	if (m_HitPoint != 0) {
		m_HealTimer += DrawerMngr->GetDeltaTime();
		if (m_HealTimer > 60.f * DrawerMngr->GetDeltaTime()) {
			m_HealTimer -= 60.f * DrawerMngr->GetDeltaTime();
			m_HitPoint = std::clamp(m_HitPoint + 1, 0, m_HitPointMax);
		}
	}

	Update_Chara();
	{
		Util::Matrix4x4 Mat = m_PlaneParam.TargetMat * GetMat().rotation().inverse();
		{
			float YawPer = 0.f;
			if (m_PlaneParam.IsAuto) {
				YawPer = -Mat.zvec2().x * Util::deg2rad(40.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.q && !m_PlaneParam.e) {
				YawPer = Util::deg2rad(-40.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.e && !m_PlaneParam.q) {
				YawPer = Util::deg2rad(40.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.q && m_PlaneParam.e) {
				YawPer = Util::deg2rad(0.f * DrawerMngr->GetDeltaTime());
			}
			Util::Easing(&m_YawPer, YawPer * (m_Speed / GetSpeedBase()), 0.95f);
		}
		{
			float PitchPer = 0.f;
			if (m_PlaneParam.IsAuto) {
				PitchPer = Mat.zvec2().y * Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.w && !m_PlaneParam.s) {
				PitchPer = Util::deg2rad(-100.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.s && !m_PlaneParam.w) {
				PitchPer = Util::deg2rad(100.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.s && m_PlaneParam.w) {
				PitchPer = Util::deg2rad(0.f * DrawerMngr->GetDeltaTime());
			}
			Util::Easing(&m_PtichPer, PitchPer * (m_Speed / GetSpeedBase()), 0.95f);
		}
		{
			float RollPer = 0.f;
			if (m_PlaneParam.IsAuto) {
				if (m_PtichPer <= Util::deg2rad(-5.f * DrawerMngr->GetDeltaTime())) {
					RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime()) * std::clamp(m_YawPer / Util::deg2rad(20.f * DrawerMngr->GetDeltaTime()), -1.f, 1.f);
				}
				if (std::fabsf(m_YawPer) >= Util::deg2rad(2.f * DrawerMngr->GetDeltaTime())) {
					RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime()) * std::clamp(m_YawPer / Util::deg2rad(20.f * DrawerMngr->GetDeltaTime()), -1.f, 1.f);
				}
				if (std::fabsf(m_PtichPer) <= Util::deg2rad(5.f * DrawerMngr->GetDeltaTime()) && std::fabsf(m_YawPer) <= Util::deg2rad(2.f * DrawerMngr->GetDeltaTime())) {
					RollPer = Util::deg2rad(-200.f * DrawerMngr->GetDeltaTime()) * std::clamp(Mat.yvec().x / 1.f, -1.f, 1.f);
				}
			}
			if (m_PlaneParam.a && !m_PlaneParam.d) {
				RollPer = Util::deg2rad(-200.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.d && !m_PlaneParam.a) {
				RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
			}
			if (m_PlaneParam.a && m_PlaneParam.d) {
				RollPer = Util::deg2rad(0.f * DrawerMngr->GetDeltaTime());
			}
			Util::Easing(&m_RollPer, RollPer * (m_Speed / GetSpeedBase()), 0.95f);
		}
		// 左右回転
		{
			//ヨー
			{
				float Per = std::clamp(m_YawPer, Util::deg2rad(-20.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(20.f * DrawerMngr->GetDeltaTime()));
				this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.yvec(), Per);
			}
			//ピッチ
			{
				float Per = std::clamp(m_PtichPer, Util::deg2rad(-50.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(50.f * DrawerMngr->GetDeltaTime()));
				this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.xvec(), Per);
			}
			//ロール
			{
				float Per = std::clamp(m_RollPer, Util::deg2rad(-100.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(100.f * DrawerMngr->GetDeltaTime()));
				this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.zvec(), Per);
			}
		}

		// 進行方向に前進
		{
			if (m_PlaneParam.AccelKey) {
				this->m_SpeedTarget += DrawerMngr->GetDeltaTime();
			}
			if (m_PlaneParam.BrakeKey) {
				this->m_SpeedTarget -= DrawerMngr->GetDeltaTime();
			}


			auto Y = this->m_Rot.zvec2().y;
			if (std::fabsf(Y) > 0.1f) {
				this->m_SpeedTarget -= DrawerMngr->GetDeltaTime() * ((std::fabsf(Y) - 0.1f) * (Y > 0.f ? 1.f : -1.f));

				float Speedkmh = GetSpeed() / GetSpeedBase();
				if ((Speedkmh < 200.f / 200.f)) {
					this->m_SpeedTarget += DrawerMngr->GetDeltaTime() * 0.15f;
				}
			}
			else {
				float Speedkmh = GetSpeed() / GetSpeedBase();
				if ((Speedkmh < 200.f / 200.f)) {
					this->m_SpeedTarget += DrawerMngr->GetDeltaTime() * 0.5f;
				}
				else if ((Speedkmh > 200.f / 200.f)) {
					this->m_SpeedTarget -= DrawerMngr->GetDeltaTime() * 0.5f;
				}
			}

			if (std::fabsf(m_YawPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) > 0.1f) {
				this->m_SpeedTarget -= DrawerMngr->GetDeltaTime() * (std::fabsf(m_YawPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) - 0.1f) * 0.5f;
			}
			if (std::fabsf(m_PtichPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) > 0.1f) {
				this->m_SpeedTarget -= DrawerMngr->GetDeltaTime() * (std::fabsf(m_PtichPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) - 0.1f) * 0.5f;
			}
			if (std::fabsf(m_RollPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) > 0.1f) {
				this->m_SpeedTarget -= DrawerMngr->GetDeltaTime() * (std::fabsf(m_RollPer / Util::deg2rad(200.f * DrawerMngr->GetDeltaTime())) - 0.1f) * 0.5f;
			}

			this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedBase() * 3.f / 4.f, GetSpeedBase() * 3.f / 2.f);
		}
		Util::Easing(&m_Speed, this->m_SpeedTarget, 0.95f);

		// 移動ベクトルを加算した仮座標を作成
		Util::VECTOR3D PosBefore = GetTargetPos();
		Util::VECTOR3D PosAfter;
		{
			PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), this->m_Rot);
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

		this->m_AnimPer[static_cast<size_t>(CharaAnim::Roll)] = -std::clamp(m_RollPer, Util::deg2rad(-100.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(100.f * DrawerMngr->GetDeltaTime())) / Util::deg2rad(100.f * DrawerMngr->GetDeltaTime());
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Pitch)] = -std::clamp(m_PtichPer, Util::deg2rad(-50.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(50.f * DrawerMngr->GetDeltaTime())) / Util::deg2rad(50.f * DrawerMngr->GetDeltaTime());
		this->m_AnimPer[static_cast<size_t>(CharaAnim::Yaw)] = std::clamp(m_YawPer, Util::deg2rad(-20.f * DrawerMngr->GetDeltaTime()), Util::deg2rad(20.f * DrawerMngr->GetDeltaTime())) / Util::deg2rad(20.f * DrawerMngr->GetDeltaTime());


		//アニメアップデート
		bool IsUpdateAnim = true;
		switch (pOption->GetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel))->GetSelect()) {
		case 0:
			IsUpdateAnim = false;
			break;
		case 1:
			IsUpdateAnim = true;
			break;
		case 2:
			IsUpdateAnim = true;
			break;
		default:
			break;
		}

		if (IsUpdateAnim) {
			for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
				SetAnim(loop).SetPer(this->m_AnimPer[loop]);
			}
			SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
			SetModel().FlipAnimAll();
		}
		//射撃
		m_ShotSwitch = false;
		if (m_PlaneParam.attack) {
			m_ShootTimer += DrawerMngr->GetDeltaTime();
		}
		else {
			m_ShootTimer = 0.f;
		}
		m_ShotSwitch |= m_Gun.at(0).Update(m_PlaneParam.attack, IsUpdateAnim, GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), GetPlayerID());
		m_ShotSwitch |= m_Gun.at(1).Update(m_PlaneParam.attack && (m_ShootTimer > 0.1f), IsUpdateAnim, GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun2)), GetPlayerID());
	}
}
void PlaneCommon::SetShadowDraw_Sub(void) const noexcept {
	GetModel().DrawModel();
}
void PlaneCommon::CheckDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	this->m_AimPoint = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Center)).pos();
	auto Pos2D = ConvWorldPosToScreenPos(this->m_AimPoint.get());
	if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
		this->m_AimPoint2D.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
		this->m_AimPoint2D.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
	}
}
void PlaneCommon::Draw_Sub(void) const noexcept {
	if (m_IsInCloud) {
		SetFogEnable(true);
		SetFogMode(DX_FOGMODE_LINEAR);
		SetFogStartEnd(3.f * Scale3DRate, 10.f * Scale3DRate);
		SetFogColor(222, 222, 222);
	}
	for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
		if (!GetModel().GetMeshSemiTransState(loop)) {
			GetModel().DrawMesh(loop);
		}
	}
	SetFogEnable(false);
}
void PlaneCommon::DrawFront_Sub(void) const noexcept {
	if (m_IsInCloud) {
		SetFogEnable(true);
		SetFogMode(DX_FOGMODE_LINEAR);
		SetFogStartEnd(3.f * Scale3DRate, 10.f * Scale3DRate);
		SetFogColor(222, 222, 222);
	}
	for (int loop = 0; loop < GetModel().GetMeshNum(); ++loop) {
		if (GetModel().GetMeshSemiTransState(loop)) {
			GetModel().DrawMesh(loop);
		}
	}

	SetFogEnable(false);
}
void PlaneCommon::ShadowDraw_Sub(void) const noexcept {
	GetModel().DrawModel();
}
void PlaneCommon::Dispose_Sub(void) noexcept {
	for (auto& ae : this->m_DamageEffect) {
		ae.reset();
	}
	m_Jobs.Dispose();
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->StopAll();
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->StopAll();

	SetModel().Dispose();

	for (auto& g : m_Gun) {
		g.Dispose();
	}
}

void Plane::Update_Chara(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* pOption = Util::OptionParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	m_IsFreeView = KeyMngr->GetBattleKeyPress(Util::EnumBattle::ChangeWeapon);
	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Aim)) {
		m_IsFPS ^= 1;
	}

	int LookX = 0;
	int LookY = 0;

	int MX = DrawerMngr->GetMousePositionX();
	int MY = DrawerMngr->GetMousePositionY();
	DxLib::GetMousePoint(&MX, &MY);

	LookX = (MX - DrawerMngr->GetWindowDrawWidth() / 2) * pOption->GetParam(pOption->GetOptionType(Util::OptionType::XSensing))->GetSelect() / 100;
	LookY = (MY - DrawerMngr->GetWindowDrawHeight() / 2)* pOption->GetParam(pOption->GetOptionType(Util::OptionType::YSensing))->GetSelect() / 100;
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

	bool IsDeath = false;
	bool IsOut = false;
	auto Matt = GetEyeMatrix().rotation();
	if (IsBorderOut()) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, 0.f, 0.f)).normalized());

		auto YT = Matt.zvec(); YT.z = std::hypotf(YT.x, YT.z); YT.x = 0.f; YT = YT.normalized();
		auto XZ = Matt.zvec(); XZ.y = 0.f; XZ = XZ.normalized();

		this->m_RadR.x = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), YT, Util::VECTOR3D::right());
		this->m_RadR.y = Util::VECTOR3D::SignedAngle(Util::VECTOR3D::forward(), XZ, Util::VECTOR3D::up());

		IsOut = true;
	}
	if (!IsAlive()) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, -100000.f, 0.f)).normalized());
		IsDeath = true;
		IsOut = true;
	}

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
			this->m_Rad.y += Per * Power * Util::deg2rad(720.f) * DrawerMngr->GetDeltaTime();
		}
		else {
			this->m_Rad.y = this->m_RadR.y;
		}
	}
	{
		this->m_RadR.x = std::clamp(this->m_RadR.x, Util::deg2rad(-89), Util::deg2rad(89));
		float Per = std::clamp(Util::AngleRange180(this->m_RadR.x - this->m_Rad.x) / Util::deg2rad(15.f), -1.f, 1.f);
		if (std::fabsf(Per) > 0.01f) {
			float Power = 0.5f;
			this->m_Rad.x += Per * Power * Util::deg2rad(720.f) * DrawerMngr->GetDeltaTime();
		}
		else {
			this->m_Rad.x = this->m_RadR.x;
		}
	}

	SetParam(
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::W) || (m_IsFPS && LookY > 1),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::S) || (m_IsFPS && LookY < -1),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::A) || IsDeath,
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::D),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q) || (m_IsFPS && LookX < -3),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::E) || (m_IsFPS && LookX > 3),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run),
		KeyMngr->GetBattleKeyPress(Util::EnumBattle::Jump),
		IsOut || !m_IsFreeView && !m_IsFPS, Matt);
}

void EnemyPlane::Update_Chara(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	auto& Player = PlayerManager::Instance()->SetPlane().at(0);

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
	if (!GetCanWatchPlane(0)) {
		m_TargetMat = GetMat().rotation();
		m_AutoTimer = 0.f;
	}
	bool IsDeath = false;
	bool IsOut = false;
	auto Matt = m_TargetMat;
	if (IsBorderOut()) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, 0.f, 0.f)).normalized());

		IsOut = true;
	}
	if (!IsAlive()) {
		Matt = Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), (GetMat().pos() - Util::VECTOR3D::vget(0.f, -100000.f, 0.f)).normalized());
		IsDeath = true;
		IsOut = true;
	}

	bool IsShot = false;
	if (!IsOut) {
		float angle = Util::VECTOR3D::Angle((Player->GetMat().pos() - GetMat().pos()).normalized(), GetMat().zvec2());
		IsShot = angle < Util::deg2rad(2.5f);
		if (IsShot) {
			IsShot = (Player->GetMat().pos() - GetMat().pos()).magnitude() < (300.f * Scale3DRate);
		}
	}

	m_AccelTimer += DrawerMngr->GetDeltaTime();
	if (m_AccelTimer > 5.f) {
		m_AccelTimer -= 5.f;
		float Speedkmh = GetSpeed() / GetSpeedBase();
		m_Accel = (Speedkmh < 170.f / 200.f);
		m_Brake = (Speedkmh > 250.f / 200.f);
	}
	m_AutoTimer += DrawerMngr->GetDeltaTime();
	if (m_AutoTimer > 5.f) {
		m_AutoTimer -= 5.f;
	}
	SetParam(
		false, false, IsDeath, false, false, false,
		IsShot,
		m_Accel,
		m_Brake,
		IsOut || (m_AutoTimer < 2.0f), Matt);

}
