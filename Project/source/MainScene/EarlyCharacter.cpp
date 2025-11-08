#include "EarlyCharacter.hpp"

#include "PlayerManager.hpp"

void EarlyCharacter::Update_Chara(void) noexcept {
	auto* BackGroundParts = BackGround::Instance();

	auto& Player = ((std::shared_ptr<Character>&)PlayerManager::Instance()->SetCharacter().at(0));
	{
		auto MyPos = GetEyeMatrix().pos();
		auto MyVec = GetEyeMatrix().zvec() * -1.f; MyVec.y = 0.f; MyVec = MyVec.normalized();
		auto TargetPos = Player->GetEyeMatrix().pos();
		auto TargetVec = TargetPos - MyPos; 
		auto Height = std::fabsf(TargetVec.y); TargetVec.y = 0.f;
		auto Len = TargetVec.magnitude();
		auto rad = std::atan2f(Util::VECTOR3D::Cross(MyVec, TargetVec.normalized()).y, Util::VECTOR3D::Dot(MyVec, TargetVec.normalized()));
		{
			bool IsAlert = !IsDown();
			{
				if (IsAlert) {
					IsAlert = !this->m_Armlocked.IsActive();
				}
				if (IsAlert) {
					IsAlert = Height < 1.5f * Scale3DRate;
				}
				if (IsAlert) {
					IsAlert = std::fabsf(rad) < Util::deg2rad(45) || (Len < 0.75f * Scale3DRate);
				}
				if (IsAlert) {
					IsAlert = Len < 4.f * Scale3DRate;
				}
				if (IsAlert) {
					IsAlert = !BackGroundParts->CheckLine(MyPos, &TargetPos);
				}
			}

			//誰かが見てた状態がになってないか5秒間隔で確認
			bool WatchAny = false;
			for (auto& c : PlayerManager::Instance()->GetCharacter()) {
				if (c->IsPlayer()) { continue; }
				if (c->GetObjectID() == GetObjectID()) { continue; }
				auto& ec = ((std::shared_ptr<EarlyCharacter>&)c);
				if (ec->IsDown()) { continue; }
				if (ec->m_AlertTimer <= 7.f) { continue; }
				WatchAny = true;
				break;
			}
			if (WatchAny) {
				this->m_AlertAnyTimer += DeltaTime;
			}
			else {
				this->m_AlertAnyTimer = 0.f;
			}
			if (this->m_AlertAnyTimer >= 5.f) {
				this->m_AlertAnyTimer -= 5.f;
				IsAlert = true;
			}


			if (IsAlert) {
				if (!IsWatching()) {
					if (!this->m_Alert.IsActive()) {
						this->m_Alert.SetActive();
						this->m_Caution.SetDisActive();
					}
				}
				this->m_AlertTimer = 10.f;
			}
			else {
				this->m_AlertTimer = std::max(this->m_AlertTimer - DeltaTime, 0.f);
			}
		}
		{
			bool IsCaution = !IsDown();
			{
				if (IsCaution) {
					IsCaution = !this->m_Armlocked.IsActive();
				}
				if (IsCaution) {
					IsCaution = Height < 1.5f * Scale3DRate;
				}
				if (IsCaution) {
					IsCaution = std::fabsf(rad) < Util::deg2rad(45) || (Len < 0.75f * Scale3DRate);
				}
				if (IsCaution) {
					IsCaution = Len < 7.f * Scale3DRate;
				}
				if (IsCaution) {
					IsCaution = !BackGroundParts->CheckLine(MyPos, &TargetPos);
				}
			}
			if (!(IsDown() || this->m_Armlocked.IsActive()) && this->m_FlipBack > 0.f) {
				IsCaution = true;
			}
			if (IsCaution) {
				if (!IsWatching()) {
					if (!this->m_Alert.IsActive()) {
						if (!this->m_Caution.IsActive()) {
							this->m_Caution.SetActive();
						}
					}
				}
				this->m_CautionTimer = std::min(this->m_CautionTimer + DeltaTime, 3.f);
			}
			else {
				this->m_CautionTimer = std::max(this->m_CautionTimer - DeltaTime, 0.f);
			}
			if (this->m_CautionTimer == 3.f) {
				if (!IsWatching()) {
					if (!this->m_Alert.IsActive()) {
						this->m_Alert.SetActive();
						this->m_Caution.SetDisActive();
					}
				}
				this->m_AlertTimer = 10.f;
			}
		}
		if (!(IsDown() || this->m_Armlocked.IsActive()) && this->m_FlipBack>0.f) {
			this->m_FlipBack = std::max(this->m_FlipBack - DeltaTime, 0.f);
			this->m_IsMove = false;
			this->m_IsTurn = true;
		}
		else if (IsWatching()) {
			//視認したらその方向を追う
			SetTarget(TargetPos);
			this->m_IsMove = true;
			this->m_IsTurn = true;
		}
		else {
			//見失った
			//TODO:一番近いウェイポイントに移ってから哨戒
			SetTarget(MyPos + MyVec * 2.f);
			this->m_IsMove = false;
			this->m_IsTurn = false;
		}
	}

	{
		this->m_Alert.Update();
		this->m_Caution.Update();

		bool				CanPunch{ false };
		bool				CanArmlock{ false };
		if (!this->m_Armlocked.IsActive() &&
			!(this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End)
			) {
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Base2 = Base;
			Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

			Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
			Util::VECTOR3D Target2 = Target;
			Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
			if (Player->CheckHit(Base1, &Target1) || Player->CheckHit(Base2, &Target2) || Player->CheckHit(Base3, &Target3)) {
				Util::VECTOR3D A = Player->GetMat().zvec(); A.y = 0.f;
				Util::VECTOR3D B = Target - Base; B.y = 0.f;
				if (Util::VECTOR3D::Dot(A, B) > 0.f) {
					CanPunch = true;
				}
				else {
					CanArmlock = true;
				}
			}
		}


		if (CanPunch && Player->CanDamage() && (this->m_AttackCoolDown == 0.f)) {
			//パンチ
			if (!this->m_Punch.IsActive()) {
				this->m_Punch.SetActive();
				SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).SetTime(0.f);
				this->m_AttackCoolDown = 3.f;
			}
		}
		if (CanArmlock && Player->CanDamage() && (this->m_AttackCoolDown == 0.f)) {
			if (!this->m_Armlock.IsActive()) {
				this->m_Armlock.SetActive();
				this->m_Armlock.m_Time = 2.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).SetTime(0.f);
				//IDの相手に羽交い絞めを開始させる
				Player->SetArmlocked(this->GetObjectID());

				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ArmlockID)->Play3D(GetMat().pos(), 10.f * Scale3DRate, DX_PLAYTYPE_LOOP);
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				this->m_AttackCoolDown = 3.f;
			}
		}
		if (this->m_Armlock.IsActive() && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).GetTimePer() >= 1.f) {
			if (this->m_Armlock.m_Time == 0.f) {
				if (!this->m_Armlock.m_End) {
					this->m_Armlock.m_End = true;
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ArmlockID)->StopAll();
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_ArmlockStartID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).SetTime(0.f);
					Player->SetArmlockedEnd();
				}
			}
		}

		this->m_Armlock.m_Time = std::max(this->m_Armlock.m_Time - DeltaTime, 0.f);
		this->m_AttackCoolDown = std::max(this->m_AttackCoolDown - DeltaTime, 0.f);

		//前準備
		this->m_PathUpdateTimer = std::max(this->m_PathUpdateTimer - DeltaTime, 0.f);
		if (this->m_PathUpdateTimer <= 0.f) {
			this->m_PathUpdateTimer += 1.f;
			//ChangePoint
			Util::VECTOR3D MyPos = GetTargetPos();
			this->m_TargetPathPlanningIndex = -1;
			for (int i = 0; i < 10; i++) {
				this->m_PathChecker.Dispose();
				if (this->m_PathChecker.Init(MyPos, this->m_MyTarget)) {	// 指定の２点の経路情報を探索する
					this->m_TargetPathPlanningIndex = this->m_PathChecker.GetStartUnit()->GetPolyIndex();	// 移動開始時点の移動中間地点の経路探索情報もスタート地点にあるポリゴンの情報
					break;
				}
				else {
					MyPos = BackGroundParts->GetWayPoint()->GetRandomPoint(MyPos, 10.f * Scale3DRate);//選定できない場合10m以内で再選定
				}
			}
		}
		bool IsMove = false;
		//
		{
			bool IsMoving = false;
			if (SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).GetPer() > 0.5f) {
				IsMoving = true;
				float Time = SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).GetTime();

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
			if (!IsMoving) {
				this->m_FootSoundID = -1;
			}
		}
		// 左右回転
		{
			{
				Util::VECTOR3D Vec3D = this->m_MyTarget - GetTargetPos(); Vec3D.y = 0.f;
				IsMove = Vec3D.magnitude() >= 0.5f * Scale3DRate;
			}

			Util::VECTOR2D InputVec = Util::VECTOR2D::zero();
			{
				auto Pos = (GetTargetPos() + Util::VECTOR3D::up() * (1.f * Scale3DRate));
				Util::VECTOR3D Vec3D = this->m_PathChecker.GetNextPoint(Pos, &this->m_TargetPathPlanningIndex) - Pos;
				InputVec.x = Vec3D.x;
				InputVec.y = Vec3D.z;

				if (!(IsDown() || this->m_Armlocked.IsActive()) && this->m_FlipBack > 0.f) {
					InputVec.x = m_FlipBackVec.x;
					InputVec.y = m_FlipBackVec.z;
				}


				if (InputVec.magnitude() >= 1.f * Scale3DRate) {
					InputVec = InputVec.normalized();
				}
				if (this->m_Punch.IsActive() || this->m_Armlock.IsActive()) {
					InputVec = Util::VECTOR2D::zero();
				}
			}
			//歩くより早く移動する場合
			if (GetSpeed() > (3.f * Scale3DRate * DeltaTime)) {
				Util::Easing(&m_InputVec, InputVec, 0.975f);
			}
			else {
				Util::Easing(&m_InputVec, InputVec, 0.9f);
			}

			if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
				this->m_InputVec.x = this->m_DownVec.x;
				this->m_InputVec.y = this->m_DownVec.z;
			}
			if (std::fabsf(this->m_HitPower) > 0.5f) {
				this->m_InputVec.x = -m_HitVec.x;
				this->m_InputVec.y = -m_HitVec.z;
			}

			if (this->m_InputVec.sqrMagnitude() > 0.f) {
				float Per = CalcYradDiff(std::atan2f(-m_InputVec.x, -m_InputVec.y));

				IsMove = std::fabsf(Per) < 0.3f;

				this->m_RadAdd.y = 0.f;
				if (std::fabsf(Per) > 0.01f) {
					float Power = 1.f;
					this->m_RadAdd.y = Per * Power * Util::deg2rad(720.f) * DeltaTime;
				}

				if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
					IsMove = false;
				}
				else if (std::fabsf(this->m_HitPower) > 0.5f) {
					IsMove = false;
				}
				else if (this->m_Armlocked.IsActive() || this->m_Punch.IsActive() || this->m_Armlock.IsActive()) {
					IsMove = false;
					this->m_RadAdd.y = 0.f;
				}
				if (!m_IsMove) {
					//停止
					IsMove = false;
				}
				if (!m_IsTurn) {
					this->m_RadAdd.y = 0.f;
				}


				this->m_Rad.y += this->m_RadAdd.y;
				{
					float Power = 1.f;
					Util::Easing(&m_Rad.z, this->m_RadAdd.y * Power, 0.9f);
				}
				this->m_Rad.y = Util::AngleRange360(this->m_Rad.y);
			}

			Util::Easing(&m_Rad.x, 0.f, 0.9f);
		}

		// 進行方向に前進
		Util::Easing(&m_Speed, IsMove ? GetSpeedMax() : 0.f, 0.9f);

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
			Util::VECTOR3D Vec = Util::VECTOR3D::forward();
			if (this->m_Armlocked.IsActive() || this->m_Punch.IsActive() || this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
				Vec = Util::VECTOR3D::zero();
			}

			PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Vec * -GetSpeed(), this->m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(5.f * Scale3DRate * DeltaTime) * this->m_DownPower, this->m_Rot);

			PosAfter = PosAfter + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * (5.f * Scale3DRate * DeltaTime) * this->m_HitPower,
				Util::Matrix3x3::RotVec2(Util::VECTOR3D::forward(), this->m_HitVec));

			Util::Easing(&this->m_HitBack, 0.f, 0.95f);
			Util::Easing(&this->m_HitPower, 0.f, 0.9f);
			Util::Easing(&this->m_DownPower, 0.f, 0.9f);

			this->m_KeepTimer = std::clamp(this->m_KeepTimer - DeltaTime, 0.f, 1.f);
			if (this->m_KeepTimer == 0.f) {
				this->m_KeepPower = std::clamp(this->m_KeepPower - DeltaTime / 2.f, 0.f, 1.f);
			}
		}
		//他キャラとのヒット判定
		if (!this->m_Armlocked.IsActive() && !this->m_Armlock.IsActive() &&
			!(this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End)
			) {
			float Radius = 2.0f * 0.3f * Scale3DRate;
			for (auto& c : PlayerManager::Instance()->SetCharacter()) {
				if (c->GetObjectID() == this->GetObjectID()) { continue; }
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
		CheckWall(PosBefore, &PosAfter, Util::VECTOR3D::zero(), Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
		if (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End) {
			Util::VECTOR3D PosAdd = GetEyeMatrix().pos() - GetMat().pos(); PosAdd.y = 0.f; PosAdd = PosAdd.normalized() * (0.5f * Scale3DRate);
			CheckWall(PosBefore, &PosAfter, PosAdd, Util::VECTOR3D::up() * (0.7f * Scale3DRate), Util::VECTOR3D::up() * (1.6f * Scale3DRate), 0.35f * Scale3DRate);
		}
		// 地面判定
		if (CheckGround(&PosAfter, Util::VECTOR3D::zero(), 1.f, 0.1f)) {
			this->m_Vector.y = 0.f;
		}
		else {
			// ヒットしていない際は落下させる
			this->m_Vector.y -= GravAccel;
			PosAfter.y += this->m_Vector.y;
		}
		// 仮座標を反映
		this->m_Speed = std::clamp((PosAfter - PosBefore).magnitude(), 0.f, this->m_Speed);

		this->m_MyPosTarget = PosAfter;
		Util::VECTOR3D MyPos = GetMat().pos();
		Util::Easing(&MyPos, PosAfter, 0.9f);

		this->m_Rot = Util::Matrix3x3::Get33DX(GetRotMat());
		SetMatrix(
			GetRotMat() * Util::Matrix4x4::Mtrans(MyPos)
		);

		//移動割合
		Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);

		{
			auto Prev = this->m_DownTop.m_Time;
			if (this->m_DrugPer < GetDrugPerMax()) {
				this->m_DownTop.m_Time = std::max(this->m_DownTop.m_Time - DeltaTime, 0.f);
			}
			if (Prev != 0.f && this->m_DownTop.m_Time == 0.f) {
				this->m_DownTop.m_Active = false;
				//起き上がる
				if (!this->m_DownTop.m_End) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).SetTime(0.f);
				}
				this->m_DownTop.m_End = true;
			}
		}
		{
			auto Prev = this->m_DownBottom.m_Time;
			if (this->m_DrugPer < GetDrugPerMax()) {
				this->m_DownBottom.m_Time = std::max(this->m_DownBottom.m_Time - DeltaTime, 0.f);
			}
			if (Prev != 0.f && this->m_DownBottom.m_Time == 0.f) {
				this->m_DownBottom.m_Active = false;
				//起き上がる
				if (!this->m_DownBottom.m_End) {
					SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).SetTime(0.f);
				}
				this->m_DownBottom.m_End = true;
			}
		}

		if (this->m_PunchAttack) {
			this->m_PunchAttack = false;
			Util::VECTOR3D Base = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Upper2)).pos();
			Util::VECTOR3D Target = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::forward() * -(1.5f * Scale3DRate), this->m_Rot);
			for (auto& c : PlayerManager::Instance()->SetCharacter()) {
				if (!c->IsPlayer()) { continue; }
				Util::VECTOR3D Base1 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
				Util::VECTOR3D Base2 = Base;
				Util::VECTOR3D Base3 = Base + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);

				Util::VECTOR3D Target1 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * (0.3f * Scale3DRate), this->m_Rot);
				Util::VECTOR3D Target2 = Target;
				Util::VECTOR3D Target3 = Target + Util::Matrix3x3::Vtrans(Util::VECTOR3D::right() * -(0.3f * Scale3DRate), this->m_Rot);
				if (c->CheckHit(Base1, &Target1) || c->CheckHit(Base2, &Target2) || c->CheckHit(Base3, &Target3)) {
					((std::shared_ptr<Character>&)c)->SetHit(Target - Base);
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_HitHumanID)->Play3D(Target, 10.f * Scale3DRate);
				}
			}
		}

		if (this->m_Punch.IsActive() && SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTimePer() >= 1.f) {
			this->m_Punch.m_Active = false;
		}
		if (this->m_ArmlockedInjector && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).GetTimePer() >= 1.f) {
			this->m_ArmlockedInjector = false;
		}
		if (this->m_Armlocked.m_End && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).GetTimePer() >= 1.f) {
			this->m_Armlocked.m_End = false;
			this->m_Armlocked.m_Active = false;

			this->m_DownBottom.SetActive();
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).SetTime(SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).GetTotalTime());

			this->m_DownBottom.m_End = false;
		}
		if (this->m_Armlock.m_End && SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).GetTimePer() >= 1.f) {
			this->m_Armlock.m_End = false;
			this->m_Armlock.m_Active = false;
			this->m_AttackCoolDown = 3.f;
		}
		//
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockStart)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockEnd)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedEnd)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 0.f;

		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = 0.f;
		this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = 0.f;

		if (this->m_Armlock.IsActive()) {
			if (!this->m_Armlock.m_End) {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockStart)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockStart)).Update(false, 1.f);
			}
			else {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockEnd)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockEnd)).Update(false, 1.f);
			}
		}
		else if (this->m_Armlocked.IsActive()) {
			if (!this->m_Armlocked.m_End) {
				if (!this->m_ArmlockedInjector) {
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 1.f;
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 0.f;
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedStart)).Update(false, 1.f);
				}
				else {
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedStart)] = 0.f;
					this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedInjector)] = 1.f;
					SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).Update(false, 1.f);

					float Now = SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).GetTime();
					if (static_cast<int>(Now) == 30 && static_cast<int>(Now) != static_cast<int>(this->m_ArmlockedInjectorTimer)) {
						SetDrug(GetDrugPerMax() * 100.f / 100.f);
					}
					this->m_ArmlockedInjectorTimer = Now;
				}
			}
			else {
				this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::ArmlockedEnd)] = 1.f;
				SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).Update(false, 1.f);


				float Now = SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).GetTime();
				if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(this->m_Armlocked.m_AnimTimer)) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_DownHumanID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
				}
				this->m_Armlocked.m_AnimTimer = Now;
			}
		}
		else if (this->m_DownTop.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownTop)).Update(false, 1.f);
		}
		else if (this->m_DownTop.m_End) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeTop)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeTop)).GetTimePer() >= 1.f) {
				this->m_DownTop.m_End = false;
			}
		}
		else if (this->m_DownBottom.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::DownBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).Update(false, 1.f);
		}
		else if (this->m_DownBottom.m_End) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::WakeBottom)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).Update(false, 1.f);
			if (SetAnim(static_cast<int>(EarlyCharaAnim::WakeBottom)).GetTimePer() >= 1.f) {
				this->m_DownBottom.m_End = false;
			}
		}
		else if (this->m_Punch.IsActive()) {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Punch)] = 1.f;
			SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).Update(false, 1.f);

			float Now = SetAnim(static_cast<int>(EarlyCharaAnim::Punch)).GetTime();
			if (static_cast<int>(Now) == 1 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_Time)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_PunchID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
			}
			if (static_cast<int>(Now) == 10 && static_cast<int>(Now) != static_cast<int>(this->m_Punch.m_Time)) {
				Camera::Camera3D::Instance()->SetCamShake(0.1f, 0.1f * Scale3DRate);
				this->m_PunchAttack = true;
			}
			this->m_Punch.m_Time = Now;
		}
		else {
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Upper)] = 1.f;
			//停止
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Stand)] = (1.f - GetMovePer01());
			//移動
			this->m_AnimPer[static_cast<size_t>(EarlyCharaAnim::Walk)] = GetMovePer01();
		}

		//アニメアップデート
		for (size_t loop = 0; loop < static_cast<size_t>(EarlyCharaAnim::Max); ++loop) {
			SetAnim(loop).SetPer(this->m_AnimPer[loop]);
		}
		SetAnim(static_cast<int>(EarlyCharaAnim::Upper)).Update(true, 1.f);
		SetAnim(static_cast<int>(EarlyCharaAnim::Stand)).Update(true, 1.f);
		SetAnim(static_cast<int>(EarlyCharaAnim::Walk)).Update(true, GetSpeed() * 2.75f);
		SetModel().FlipAnimAll();



		{
			ResetFrameUserLocalMatrix(static_cast<int>(CharaFrame::Upper));
			SetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper),
				Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), this->m_HitBack * Util::deg2rad(90.f)) *
				GetFrameLocalMatrix(static_cast<int>(CharaFrame::Upper))
			);
		}

		Util::Easing(&m_CanSeePer, this->m_CanSeeUI ? 1.f : 0.f, 0.9f);

		Util::Easing(&m_DrugPerR, this->m_DrugPer, 0.9f);

		this->m_CanSeeUI = false;
	}
}