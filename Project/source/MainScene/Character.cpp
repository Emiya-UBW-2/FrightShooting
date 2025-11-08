#include "Character.hpp"

void Plane::CheckDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	this->m_AimPoint = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Center)).pos();
	auto Pos2D = ConvWorldPosToScreenPos(this->m_AimPoint.get());
	this->m_AimPoint2D.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
	this->m_AimPoint2D.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
}

void Plane::Update_Chara(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Aim)) {
		m_IsFPS ^= 1;
	}

	bool LeftKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::A);
	bool RightKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::D);
	bool UpKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::W);
	bool DownKey = KeyMngr->GetBattleKeyPress(Util::EnumBattle::S);
	uint8_t				MoveKey = 0;
	MoveKey |= LeftKey ? (1 << 0) : 0;
	MoveKey |= RightKey ? (1 << 1) : 0;
	MoveKey |= UpKey ? (1 << 2) : 0;
	MoveKey |= DownKey ? (1 << 3) : 0;


	Util::Matrix4x4 Mat = GetEyeMatrix().rotation() * GetMat().rotation().inverse();
	{
		float YawPer = 0.f;
		if (!IsFPSView()) {
			YawPer = -Mat.zvec2().x * Util::deg2rad(100.f * DeltaTime);
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q) && !KeyMngr->GetBattleKeyPress(Util::EnumBattle::E)) {
			YawPer = Util::deg2rad(-100.f * DeltaTime);
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::E) && !KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q)) {
			YawPer = Util::deg2rad(100.f * DeltaTime);
		}
		if (KeyMngr->GetBattleKeyPress(Util::EnumBattle::Q) && KeyMngr->GetBattleKeyPress(Util::EnumBattle::E)) {
			YawPer = Util::deg2rad(0.f * DeltaTime);
		}
		Util::Easing(&m_YawPer, YawPer, 0.9f);
	}
	{
		float PitchPer = 0.f;
		if (!IsFPSView()) {
			PitchPer = Mat.zvec2().y * Util::deg2rad(100.f * DeltaTime);
		}
		if (UpKey && !DownKey) {
			PitchPer = Util::deg2rad(-100.f * DeltaTime);
		}
		if (DownKey && !UpKey) {
			PitchPer = Util::deg2rad(100.f * DeltaTime);
		}
		if (DownKey && UpKey) {
			PitchPer = Util::deg2rad(0.f * DeltaTime);
		}
		Util::Easing(&m_PtichPer, PitchPer, 0.9f);
	}
	{
		float RollPer = 0.f;
		if (!IsFPSView()) {
			if (m_PtichPer <= Util::deg2rad(-5.f * DeltaTime)) {
				if (m_YawPer > 0.f) {
					RollPer = Util::deg2rad(100.f * DeltaTime);
				}
				else {
					RollPer = Util::deg2rad(-100.f * DeltaTime);
				}
			}
			if (std::fabsf(m_YawPer) >= Util::deg2rad(2.f * DeltaTime)) {
				if (m_YawPer > 0.f) {
					RollPer = Util::deg2rad(100.f * DeltaTime);
				}
				else {
					RollPer = Util::deg2rad(-100.f * DeltaTime);
				}
			}
			if (std::fabsf(m_PtichPer) <= Util::deg2rad(5.f * DeltaTime) && std::fabsf(m_YawPer) <= Util::deg2rad(2.f * DeltaTime)) {
				if (Mat.yvec().y < 0.9f) {
					if (Mat.yvec().x > 0.f) {
						RollPer = Util::deg2rad(-100.f * DeltaTime);
					}
					else {
						RollPer = Util::deg2rad(100.f * DeltaTime);
					}
				}
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
		Util::Easing(&m_RollPer, RollPer, 0.9f);
	}
	// 左右回転
	{
		//ヨー
		{
			float Per = std::clamp(m_YawPer, Util::deg2rad(-20.f*DeltaTime), Util::deg2rad(20.f * DeltaTime));
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
	LookX = MX - DrawerMngr->GetWindowDrawWidth() / 2;
	LookY = MY - DrawerMngr->GetWindowDrawHeight() / 2;
	DxLib::SetMousePoint(DrawerMngr->GetWindowDrawWidth() / 2, DrawerMngr->GetWindowDrawHeight() / 2);



	bool IsMove = (MoveKey != 0);
	{
		this->m_RadAdd.y = Util::deg2rad(static_cast<float>(LookX) / 30.f);
		this->m_RadAdd.x = Util::deg2rad(static_cast<float>(-LookY) / 30.f);

		{
			this->m_RadR.y = Util::AngleRange360(this->m_RadR.y + this->m_RadAdd.y);
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
				float Power = 1.f;
				this->m_Rad.y += Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			else {
				this->m_Rad.y = this->m_RadR.y;
			}
		}
		{
			this->m_RadR.x = Util::AngleRange360(this->m_RadR.x + this->m_RadAdd.x);
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
				float Power = 1.f;
				this->m_Rad.x += Per * Power * Util::deg2rad(720.f) * DeltaTime;
			}
			else {
				this->m_Rad.x = this->m_RadR.x;
			}
		}
	}
	// 進行方向に前進
	Util::Easing(&m_Speed, (IsMove) ? GetSpeedMax() : 0.f, 0.9f);

	// 移動ベクトルを加算した仮座標を作成
	Util::VECTOR3D PosBefore = GetTargetPos();
	Util::VECTOR3D PosAfter;

	Util::VECTOR2D InputVec = Util::VECTOR2D::zero();
	{
		Util::VECTOR3D Vec;
		Vec.x = InputVec.x;
		Vec.z = InputVec.y;
		PosAfter = PosBefore + Util::Matrix3x3::Vtrans(Vec * -GetSpeed(), this->m_Rot);
	}

	//他キャラとのヒット判定
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

	//移動割合
	Util::Easing(&m_MovePer, GetSpeed() / GetSpeedMax(), 0.9f);
	//
	this->m_AnimPer[static_cast<size_t>(CharaAnim::Stand)] = 1.f;
	//アニメアップデート
	for (size_t loop = 0; loop < static_cast<size_t>(CharaAnim::Max); ++loop) {
		SetAnim(loop).SetPer(this->m_AnimPer[loop]);
	}
	SetAnim(static_cast<int>(CharaAnim::Stand)).Update(true, 1.f);
	SetModel().FlipAnimAll();
}