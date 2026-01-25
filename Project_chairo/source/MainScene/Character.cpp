#include "Character.hpp"
#include "PlayerManager.hpp"

void PlaneCommon::Init_Sub(void) noexcept {
	this->m_SpeedTarget = GetSpeedMax();
	this->m_Speed = this->m_SpeedTarget;
	this->m_TotalAmmo = this->m_CanHaveAmmo;

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
void PlaneCommon::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* KeyMngr = Util::KeyParam::Instance();
	//
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetPosition(m_PropellerIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetPosition(m_EngineIndex, GetMat().pos());
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PropellerID)->SetLocalVolume(128);
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EngineID)->SetLocalVolume(64);
	//射撃
	{
		if (!KeyMngr->GetBattleKeyPress(Util::EnumBattle::Attack)) {
			m_ShootTimer = 0.f;
		}
		else {
			if (m_ShootTimer == 0.f) {
				this->m_ShotEffect.at(static_cast<size_t>(this->m_ShotEffectID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)));
				++m_ShotEffectID %= static_cast<int>(this->m_ShotEffect.size());

				this->m_AmmoPer.at(static_cast<size_t>(this->m_AmmoID))->Set(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Gun1)), PlayerID);
				++m_AmmoID %= static_cast<int>(this->m_AmmoPer.size());

				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ShotID)->Play3D(GetMat().pos(), 500.f * Scale3DRate);

				m_ShootTimer = 0.1f;
			}
			m_ShootTimer = std::max(m_ShootTimer - DrawerMngr->GetDeltaTime(), 0.f);
		}
	}
	//移動
	{
		bool AccelKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Run);
		bool BrakeKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::Jump);
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

				float prev = m_MovePointAdd.x;
				if (LeftKey && !RightKey) {
					m_MovePointAdd.x -= 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
					MoveVec.x = -0.3f;
				}
				if (RightKey && !LeftKey) {
					m_MovePointAdd.x += 10.f * Scale3DRate * DrawerMngr->GetDeltaTime();
					MoveVec.x = 0.3f;
				}
				m_MovePointAdd.x = std::clamp(m_MovePointAdd.x, -6.f * Scale3DRate, 6.f * Scale3DRate);
				if (prev == m_MovePointAdd.x) {
					MoveVec.x = 0.0f;
				}

				float RollPer = 0.f;
				RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
				if (MyMat.yvec().y > 0.f) {
					RollPer *= MyMat.yvec().x;
				}
				else {
					RollPer *= (MyMat.yvec().x > 0.f) ? 1.f : -1.f;
				}
				if (prev != m_MovePointAdd.x) {
					if (LeftKey && !RightKey) {
						RollPer = Util::deg2rad(-200.f * DrawerMngr->GetDeltaTime());
					}
					if (RightKey && !LeftKey) {
						RollPer = Util::deg2rad(200.f * DrawerMngr->GetDeltaTime());
					}
				}
				Util::Easing(&m_RollPer, RollPer, 0.9f);
				this->m_Rot *= Util::Matrix3x3::RotAxis(this->m_Rot.zvec(), m_RollPer);
			}

			Util::Easing(&m_MoveVec, MoveVec, 0.95f);
			Util::Easing(&m_MovePoint, m_MovePointAdd, 0.9f);
			// 進行方向に前進
			{
				if (AccelKey) {
					this->m_SpeedTarget += DrawerMngr->GetDeltaTime();
				}
				if (BrakeKey) {
					this->m_SpeedTarget -= DrawerMngr->GetDeltaTime();
				}
				this->m_SpeedTarget = std::clamp(this->m_SpeedTarget, GetSpeedMax() * 3.f / 4.f, GetSpeedMax() * 3.f / 2.f);
				Util::Easing(&this->m_Speed, this->m_SpeedTarget, 0.95f);
			}
			// 移動ベクトルを加算した仮座標を作成
			Util::VECTOR3D PosBefore = GetTargetPos();
			this->m_MyPosTarget = PosBefore + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (-this->m_Speed * (60.f * DrawerMngr->GetDeltaTime())), this->m_Rot);
			//当たり判定
			this->m_Speed = std::clamp((this->m_MyPosTarget - PosBefore).magnitude(), 0.f, this->m_Speed);
			BaseMat = Util::Matrix4x4::Mtrans(GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye)).pos() + m_MovePoint);
			Util::VECTOR3D MyPos = GetMat().pos();
			Util::Easing(&MyPos, this->m_MyPosTarget, 0.9f);
			//this->m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
			SetMatrix(this->m_Rot.Get44DX() * Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), m_MoveVec)* Util::Matrix4x4::Mtrans(MyPos));
			//アニメアップデート
			{
				for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
					SetAnim(loop).SetPer(0.f);
				}
				SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
				SetModel().FlipAnimAll();
			}
		}
	}
}
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
		Util::Matrix4x4::RotAxis(Util::VECTOR3D::Cross(this->Vector, GetMat().zvec()).normalized(), Util::deg2rad(1800.f) * DrawerMngr->GetDeltaTime()) *
		Util::Matrix4x4::Mtrans(Target));
}