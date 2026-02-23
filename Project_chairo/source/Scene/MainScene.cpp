#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();
	PlayerManager::Create();
	BackGround::Create();
	BackGround::Instance()->Load();
	PlayerManager::Instance()->Load();

	m_AimPoint = std::make_unique<AimPoint>();
	m_AimPoint->Load();
}
void MainScene::Init_Sub(void) noexcept {
	BackGround::Instance()->Init();
	PlayerManager::Instance()->Init();

	auto* PostPassParts = Draw::PostPassEffect::Instance();
	auto* LightParts = Draw::LightPool::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();

	auto& Player = PlayerManager::Instance()->SetPlane();
	Player->SetPos(Util::VECTOR3D::vget(0.f, 15.f * Scale3DRate, 0.f*Scale3DRate), Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(0)));

	Player->SetDamage(InvalidID);

	m_StageScript.Init("Stage01");
	//
	this->m_Exit = false;
	this->m_Fade = 2.f;

	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.3f, -0.7f, -0.3f).normalized();

	PostPassParts->SetAmbientLight(LightVec);

	SetLightEnable(false);
	auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
	SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.f, 1.f, 1.f, 1.0f));
	SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	//DoF
	PostPassParts->SetDoFNearFar(
		(Scale3DRate * 0.15f), Scale3DRate * 5.0f,
		(Scale3DRate * 0.05f), Scale3DRate * 30.0f);

	PostPassParts->SetGodRayPer(0.25f);

	m_MainUI = std::make_unique<MainUI>();
	m_MainUI->Init();

	KeyGuideParts->SetGuideFlip();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
}
void MainScene::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(20.f);

	KeyGuideParts->ChangeGuide(
		[this]() {
			auto* Localize = Util::LocalizePool::Instance();
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			if (!this->m_MainUI->IsPauseActive()) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), Localize->Get(334));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::A), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::D), Localize->Get(335));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(337));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(339));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(336));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(338));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Walk), Localize->Get(309));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Squat), Localize->Get(310));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Prone), Localize->Get(311));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::ChangeWeapon), Localize->Get(315));
				//KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload), Localize->Get(316));
			}
			else {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::UP), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::DOWN), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::LEFT), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::RIGHT), Localize->Get(332));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Diside), Localize->Get(330));
			}
		}
	);
	auto& Watch = PlayerManager::Instance()->SetPlane();
	//
	CameraParts->SetCamInfo(
		CameraParts->GetCamera().GetCamFov() * ((Watch->GetSpeed() - Watch->GetSpeedMax()) / Watch->GetSpeedMax() * 0.35f + 1.f),
		CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());

	m_DamagePer = std::max(m_DamagePer - DrawerMngr->GetDeltaTime(), 0.f);
	m_DamageWatch = std::max(m_DamageWatch - DrawerMngr->GetDeltaTime(), 0.f);
	if (m_DamagePer == 0.f) {
		CameraParts->SetCamShake(1.f, std::fabsf(Watch->GetSpeed() - Watch->GetSpeedMax()) / (Watch->GetSpeedMax() * 2.f) * Scale3DRate);
		if (Watch->GetDamageID() != InvalidID) {
			CameraParts->SetCamShake(0.2f, 5.f * Scale3DRate);
			m_DamagePer = 0.2f;
			m_DamageWatch = 2.f;
		}
	}
	Watch->SetDamage(InvalidID);
	// 影をセット
	PostPassParts->SetShadowFarChange();
	//ポーズメニュー
	if(!this->m_Exit && (this->m_Fade <= 1.f)){
		this->m_MainUI->Update();
		if (this->m_MainUI->IsExit()) {
			this->m_Exit = true;
		}
	}
	if (this->m_MainUI->IsPauseActive()) {
		return;
	}
	if (Watch->GetHitPoint() == 0) {
		this->m_Exit = true;
	}
	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DrawerMngr->GetDeltaTime(), 0.f, 2.f);
	if (m_Exit) {
		if (this->m_Fade >= 2.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
	}
	//更新
	if (this->m_Fade <= 1.f) {
		m_StageScript.Update();
		//
		auto& Player = PlayerManager::Instance()->SetPlane();
		for (auto& a : Player->GetAmmoPer()) {
			if (a->IsActive()) {
				for (auto& s : m_StageScript.EnemyPop()) {
					if (s.m_EnemyScript.IsActive()) {

						SEGMENT_SEGMENT_RESULT Result;
						Util::GetSegmenttoSegment(s.m_EnemyScript.EnemyObj()->GetMat().pos(), s.m_EnemyScript.EnemyObj()->GetMat().pos(),
							a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
						if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
							a->SetHit(Result.SegB_MinDist_Pos);
							s.m_EnemyScript.SetDown();
							break;
						}
					}
				}
			}
		}
		//
		ObjectManager::Instance()->UpdateObject();
	}
	if (Watch->GetHitPoint() != 0) {
		Util::Matrix4x4 EyeMat = Watch->GetEyeMatrix();
		CamTarget = EyeMat.pos();
		CamPosition = EyeMat.pos() - EyeMat.zvec() * (-15.f * Scale3DRate);// +Util::VECTOR3D::up() * (10.f * Scale3DRate);
		CamUp = EyeMat.yvec();
	}
	CameraParts->SetCamPos(CamPosition, CamTarget, CamUp);
	BackGround::Instance()->Update();
}
void MainScene::BGDraw_Sub(void) noexcept {
	BackGround::Instance()->BGDraw();
}
void MainScene::SetShadowDrawRigid_Sub(void) noexcept {
	BackGround::Instance()->SetShadowDrawRigid();
}
void MainScene::SetShadowDraw_Sub(void) noexcept {
	BackGround::Instance()->SetShadowDraw();
	ObjectManager::Instance()->Draw_SetShadow();
}
void MainScene::Draw_Sub(void) noexcept {
	m_AimPoint->CalcPoint();
	BackGround::Instance()->Draw();
	ObjectManager::Instance()->Draw();
	//カーソル
	m_AimPoint->Draw();
}
void MainScene::DrawFront_Sub(void) noexcept {
	ObjectManager::Instance()->DrawFront();
}
void MainScene::DepthDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Depth();
}
void MainScene::ShadowDrawFar_Sub(void) noexcept {
	BackGround::Instance()->ShadowDrawFar();
}
void MainScene::ShadowDraw_Sub(void) noexcept {
	BackGround::Instance()->ShadowDraw();
	ObjectManager::Instance()->Draw_Shadow();
}
void MainScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//
	this->m_MainUI->Draw();
	//
	if (this->m_Fade > 0.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_Fade));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//
}
void MainScene::Dispose_Sub(void) noexcept {
	this->m_MainUI->Dispose();
	this->m_MainUI.reset();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->StopAll();
	BackGround::Release();
	PlayerManager::Release();
	ObjectManager::Instance()->DeleteAll();
	ObjectManager::Release();
}
