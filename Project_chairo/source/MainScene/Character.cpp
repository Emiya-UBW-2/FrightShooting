#include "Character.hpp"
#include "PlayerManager.hpp"

void Ammo::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (this->DrawTimer == 0.f) { return; }
	this->DrawTimer = std::max(this->DrawTimer - DrawerMngr->GetDeltaTime(), 0.f);
	if (this->Timer == 0.f) { return; }
	this->Timer = std::max(this->Timer - DrawerMngr->GetDeltaTime(), 0.f);
	//this->YVecAdd -= DrawerMngr->GetGravAccel();
	this->Vector.y += this->YVecAdd;
	Util::VECTOR3D Target = GetMat().pos() + this->Vector;
	//if (BackGround::Instance()->CheckLine(GetMat().pos(), &Target)) 
	//todo::当たり判定
	SetMatrix(GetMat().rotation() * Util::Matrix4x4::Mtrans(Target));
}

void Enemy::Init_Sub(void) noexcept {
	this->m_SpeedTarget = GetSpeedMax();
	this->m_Speed = this->m_SpeedTarget;

	for (auto& s : this->m_ShotEffect) {
		s = std::make_shared<ShotEffect>();
		ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
	}
	for (auto& s : this->m_AmmoPer) {
		s = std::make_shared<Ammo>();
		ObjectManager::Instance()->InitObject(s);
	}
	m_PropellerIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	m_EngineIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(0);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(0);
}
void Enemy::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetPosition(m_PropellerIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetPosition(m_EngineIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(128);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(64);
	// 進行方向に前進
	{
		this->m_Speed = GetSpeedMax();
	}
	// 移動ベクトルを加算した仮座標を作成
	{
		Util::VECTOR3D PosBefore = RailMat.pos();
		Util::VECTOR3D PosAfter = RailMat.pos();// +Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), RailMat.rotation());
		//当たり判定

		RailMat = RailMat.rotation() * Util::Matrix4x4::Mtrans(PosAfter);
		SetMatrix(RailMat);
	}
	//アニメアップデート
	{
		for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
			SetAnim(loop).SetPer(0.f);
		}
		SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
		SetModel().FlipAnimAll();
	}

	auto& Player = PlayerManager::Instance()->SetPlane();
	for (auto& a : GetAmmoPer()) {
		if (a->IsActive()) {
			if (true) {
				SEGMENT_SEGMENT_RESULT Result;
				Util::GetSegmenttoSegment(Player->GetMat().pos(), Player->GetMat().pos(),
					a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
				if (Result.SegA_SegB_MinDist_Square < (2.f * Scale3DRate) * (2.f * Scale3DRate)) {
					if (Player->IsRollingActive()) {
						//はじく
						a->SetHit(Result.SegB_MinDist_Pos);
						Player->Shot(Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), a->GetVector()) * Util::Matrix4x4::Mtrans(Player->GetMat().pos()));
					}
					else {
						a->SetHit(Result.SegB_MinDist_Pos);
						Player->SetDamage(0);
					}
					break;
				}
			}
		}
	}
}

void MyPlane::Init_Sub(void) noexcept {
	this->m_SpeedTarget = GetSpeedMax();
	this->m_Speed = this->m_SpeedTarget;

	for (auto& s : this->m_ShotEffect) {
		s = std::make_shared<ShotEffect>();
		ObjectManager::Instance()->InitObject(s, s, "data/model/FireEffect/");
	}
	for (auto& s : this->m_AmmoPer) {
		s = std::make_shared<Ammo>();
		ObjectManager::Instance()->InitObject(s);
	}
	m_PropellerIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	m_EngineIndex = Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->Play3D(GetMat().pos(), 500.f * Scale3DRate, DX_PLAYTYPE_LOOP);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(0);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(0);
}
void MyPlane::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	//
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetPosition(m_PropellerIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetPosition(m_EngineIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(128);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(64);
	//移動
	{
		Util::VECTOR3D MoveVec = Util::VECTOR3D::forward();
		//上下
		{
			bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
			bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
			float prev = m_MovePointAdd.y;
			if (UpKey && !DownKey) {
				m_MovePointAdd.y -= 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.y = -0.3f;
			}
			if (DownKey && !UpKey) {
				m_MovePointAdd.y += 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.y = 0.3f;
			}
			m_MovePointAdd.y = std::clamp(m_MovePointAdd.y, -4.f * Scale3DRate, 4.f * Scale3DRate);
			if (prev == m_MovePointAdd.y) {
				MoveVec.y = 0.0f;
			}
		}
		//ロール
		{
			bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
			bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);

			bool Left2Key = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Q);
			bool Right2Key = KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::E);

			float prev = m_MovePointAdd.x;
			if (LeftKey && !RightKey) {
				m_MovePointAdd.x -= 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = -0.3f;
			}
			if (RightKey && !LeftKey) {
				m_MovePointAdd.x += 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = 0.3f;
			}
			/*
			if (Left2Key && !Right2Key) {
				m_MovePointAdd.x -= 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = -0.6f;
			}
			if (Right2Key && !Left2Key) {
				m_MovePointAdd.x += 20.f * Scale3DRate * DrawerMngr->GetDeltaTime();
				MoveVec.x = 0.6f;
			}
			//*/
			m_MovePointAdd.x = std::clamp(m_MovePointAdd.x, -6.f * Scale3DRate, 6.f * Scale3DRate);
			if (prev == m_MovePointAdd.x) {
				MoveVec.x = 0.0f;
			}

			float RollPer = 0.f;
			RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
			auto YVec = (GetMat() * RailMat.inverse()).yvec();
			if (YVec.y > 0.f) {
				RollPer *= YVec.x;
			}
			else {
				RollPer *= (YVec.x > 0.f) ? 1.f : -1.f;
			}
			if (prev != m_MovePointAdd.x) {
				if (LeftKey && !RightKey) {
					RollPer = Util::deg2rad(-200.f * DrawerMngr->GetDeltaTime());
				}
				if (RightKey && !LeftKey) {
					RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
				}
			}

			m_RollingTimer = std::max(m_RollingTimer - DrawerMngr->GetDeltaTime(), 0.f);
			if (Left2Key && !Right2Key) {
				RollPer = Util::deg2rad(-20000.f * DrawerMngr->GetDeltaTime());
				m_RollingTimer = 0.2f;
			}
			if (Right2Key && !Left2Key) {
				RollPer = Util::deg2rad(20000.f * DrawerMngr->GetDeltaTime());
				m_RollingTimer = 0.2f;
			}

			Util::Easing(&m_RollPer, RollPer, 0.9f);
			this->m_Roll *= Util::Matrix3x3::RotAxis(this->m_Roll.zvec(), m_RollPer);
		}
		Util::Easing(&m_MoveVec, MoveVec, 0.95f);
		Util::Easing(&m_MovePoint, m_MovePointAdd, 0.9f);
	}
	// 進行方向に前進
	{
		bool AccelKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run);
		bool BrakeKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Jump);
		if (!AccelKey && !BrakeKey) {
			this->m_SpeedTarget = GetSpeedMax();
		}
		if (AccelKey && !BrakeKey) {
			this->m_SpeedTarget += 10.f * DrawerMngr->GetDeltaTime();
		}
		if (!AccelKey && BrakeKey) {
			this->m_SpeedTarget -= 10.f * DrawerMngr->GetDeltaTime();
		}
		this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedMax() * 3.f / 4.f, GetSpeedMax() * 3.f / 2.f);
		Util::Easing(&this->m_Speed, this->m_SpeedTarget, 0.95f);
	}
	// 移動ベクトルを加算した仮座標を作成
	{
		Util::VECTOR3D PosBefore = RailMat.pos();
		Util::VECTOR3D PosAfter = RailMat.pos() + Util::Matrix4x4::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), RailMat.rotation());
		//当たり判定

		RailMat = RailMat.rotation() * Util::Matrix4x4::Mtrans(PosAfter);
		SetMatrix(
			(this->m_Roll * Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), m_MoveVec) * Util::Matrix3x3::Get33DX(RailMat.rotation())).Get44DX() *
			Util::Matrix4x4::Mtrans(RailMat.pos() - Util::Matrix4x4::Vtrans(m_MovePoint, RailMat.rotation())));
	}
	//アニメアップデート
	{
		for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
			SetAnim(loop).SetPer(0.f);
		}
		SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
		SetModel().FlipAnimAll();
	}
	//射撃
	{
		if (!KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
			m_ShootTimer = 0.f;
		}
		else {
			if (m_ShootTimer == 0.f) {
				Shot(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);
				m_ShootTimer = 0.1f;
			}
			m_ShootTimer = std::max(m_ShootTimer - DrawerMngr->GetDeltaTime(), 0.f);
		}
	}
}
