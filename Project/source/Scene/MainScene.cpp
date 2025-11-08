#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();
	PlayerManager::Create();
	BackGround::Create();
	BackGround::Instance()->Load(this->m_MapName.c_str());

	PlayerManager::Instance()->Load();

	m_StandGraph = Draw::GraphPool::Instance()->Get("data/Image/Body.png")->Get();
	m_SquatGraph = Draw::GraphPool::Instance()->Get("data/Image/BodyC.png")->Get();
	m_ProneGraph = Draw::GraphPool::Instance()->Get("data/Image/BodyP.png")->Get();
	m_Watch = Draw::GraphPool::Instance()->Get("data/Image/Watch.png")->Get();
	m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	m_Lock = Draw::GraphPool::Instance()->Get("data/Image/Lock.png")->Get();
}
void MainScene::Init_Sub(void) noexcept {
	BackGround::Instance()->Init();

	PlayerManager::Instance()->Init();

	auto& Player = ((std::shared_ptr<Plane>&)PlayerManager::Instance()->SetPlane().at(0));

	Player->SetPos(Util::VECTOR3D::zero());
	//
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	//Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.3f, -0.7f, 0.3f).normalized();
	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(0.02f, -1.f, 0.02f).normalized();

	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(0.5f);
	PostPassParts->SetAmbientLight(LightVec);

	SetLightEnable(false);
	auto* LightParts = Draw::LightPool::Instance();
	auto& FirstLight = LightParts->Put(Draw::LightType::DIRECTIONAL, LightVec);
	SetLightAmbColorHandle(FirstLight.get(), GetColorF(1.f, 1.f, 1.f, 1.0f));
	SetLightDifColorHandle(FirstLight.get(), GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	//DoF
	PostPassParts->SetDoFNearFar(
		(Scale3DRate * 0.15f), Scale3DRate * 5.0f,
		(Scale3DRate * 0.05f), Scale3DRate * 30.0f);

	PostPassParts->SetGodRayPer(0.5f);

	this->m_IsSceneEnd = false;
	this->m_IsPauseActive = false;

	this->m_OptionWindow.Init();
	this->m_PauseUI.Init();

	this->m_PauseUI.SetEvent(0, [this]() {
		this->m_IsSceneEnd = true;
		this->m_IsPauseActive = false;
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->SetGuideFlip();
		});
	this->m_PauseUI.SetEvent(1, [this]() {
		this->m_OptionWindow.SetActive(true);
		});
	this->m_PauseUI.SetEvent(2, [this]() {
		this->m_IsPauseActive = false;
		auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
		KeyGuideParts->SetGuideFlip();
		});

	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	KeyGuideParts->SetGuideFlip();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
}
void MainScene::Update_Sub(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();

	KeyGuideParts->ChangeGuide(
		[this]() {
			auto* Localize = Util::LocalizePool::Instance();
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			if (this->m_IsChangeEquip) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), Localize->Get(332));
			}
			else if (!this->m_IsPauseActive) {
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::W), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::A), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::D), Localize->Get(334));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(335));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), Localize->Get(308));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Walk), Localize->Get(309));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(312));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(336));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(338));
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
	//
	CameraParts->SetCamInfo(Util::Lerp(Util::deg2rad(45),
		CameraParts->GetCamera().GetCamFov() - this->m_ShotFov * Util::deg2rad(5),
		this->m_FPSPer), CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());
	// 影をセット
	PostPassParts->SetShadowFarChange();
	//ポーズメニュー
	{
		if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Tab)) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
			this->m_IsPauseActive ^= 1;
			KeyGuideParts->SetGuideFlip();
		}
		this->m_PauseUI.SetActive(this->m_IsPauseActive && !this->m_OptionWindow.IsActive());
		if (!this->m_IsPauseActive) {
			this->m_OptionWindow.SetActive(false);
		}
		this->m_PauseUI.Update();
		if (this->m_IsSceneEnd && this->m_PauseUI.IsEnd()) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
		this->m_OptionWindow.Update();
	}
	if (this->m_IsPauseActive) {
		DxLib::SetMouseDispFlag(true);
		return;
	}
	auto& Player = ((std::shared_ptr<Plane>&)PlayerManager::Instance()->SetPlane().at(0));


	ObjectManager::Instance()->UpdateObject();
	//更新
	auto* DrawerMngr = Draw::MainDraw::Instance();
	float XPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionX() - DrawerMngr->GetDispWidth() / 2) / static_cast<float>(DrawerMngr->GetDispWidth() / 2), -1.f, 1.f);
	float YPer = std::clamp(static_cast<float>(DrawerMngr->GetMousePositionY() - DrawerMngr->GetDispHeight() / 2) / static_cast<float>(DrawerMngr->GetDispHeight() / 2), -1.f, 1.f);

	Util::VECTOR3D CamPosition;
	Util::VECTOR3D CamTarget;
	Util::VECTOR3D CamUp;

	this->m_FPSPer = std::clamp(this->m_FPSPer + (Player->IsFPSView() ? 1.f : -1.f) * DeltaTime / 0.25f, 0.f, 1.f);

	Util::VECTOR3D CamPosition1;
	Util::VECTOR3D CamTarget1;
	Util::VECTOR3D CamUp1;
	Util::VECTOR3D CamPosition2;
	Util::VECTOR3D CamTarget2;
	Util::VECTOR3D CamUp2;

	if (this->m_FPSPer != 0.f) {
		Util::Matrix4x4 EyeMat = Player->GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Eye));
		CamPosition1 = EyeMat.pos();
		CamTarget1 = CamPosition1 + EyeMat.zvec() * (-10.f * Scale3DRate);
		CamUp1 = EyeMat.yvec();


		CamTarget1 = EyeMat.pos() + EyeMat.yvec() * (1.f * Scale3DRate);
		CamPosition1 = CamTarget1 - EyeMat.zvec() * (-10.f * Scale3DRate);
		CamUp1 = EyeMat.yvec();
	}
	if (this->m_FPSPer != 1.f) {
		Util::Matrix4x4 EyeMat = Player->GetEyeMatrix();
		CamTarget2 = EyeMat.pos() + EyeMat.yvec() * (1.f * Scale3DRate);
		CamPosition2 = CamTarget2 - EyeMat.zvec() * (-10.f * Scale3DRate);
		CamUp2 = EyeMat.yvec();
	}

	CamPosition = Util::Lerp(CamPosition2, CamPosition1, this->m_FPSPer);
	CamTarget = Util::Lerp(CamTarget2, CamTarget1, this->m_FPSPer);
	CamUp = Util::Lerp(CamUp2, CamUp1, this->m_FPSPer);

	CameraParts->SetCamPos(CamPosition, CamTarget, CamUp);

	if (false) {
		this->m_ShotFov = 1.f;
	}
	else {
		Util::Easing(&m_ShotFov, 0.f, 0.9f);
	}

	DxLib::SetMouseDispFlag(!Player->IsFPSView() && !Player->IsFreeView());

	BackGround::Instance()->Update();

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime, 0.f, 1.f);
	if (!m_Exit) {
	}
	else {
		if (this->m_Fade >= 1.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			Util::SceneBase::SetEndScene();
		}
	}

	//
	{
		PostPassParts->SetScopeParam().m_IsActive = this->m_UseLens;
		PostPassParts->SetScopeParam().m_Radius = (this->m_LensSize - this->m_LensPos).magnitude();
		PostPassParts->SetScopeParam().m_Zoom = 4.f;
		PostPassParts->SetScopeParam().m_Xpos = this->m_LensPos.x;
		PostPassParts->SetScopeParam().m_Ypos = static_cast<float>(DrawerMngr->GetDispHeight()) - this->m_LensPos.y;

		PostPassParts->SetScopeParam().m_Radius = PostPassParts->SetScopeParam().m_Radius / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth()));
		PostPassParts->SetScopeParam().m_Xpos = PostPassParts->SetScopeParam().m_Xpos / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth()));
		PostPassParts->SetScopeParam().m_Ypos = PostPassParts->SetScopeParam().m_Ypos / (static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight()));
	}
	this->m_UseLens = false;
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
	SetFogEnable(true);
	SetFogMode(DX_FOGMODE_LINEAR);
	SetFogStartEnd(20.0f * Scale3DRate, 50.0f * Scale3DRate);
	SetFogColor(0, 0, 0);

	SetVerticalFogEnable(true);
	SetVerticalFogMode(DX_FOGMODE_LINEAR);
	SetVerticalFogStartEnd(8.0f * Scale3DRate, 7.0f * Scale3DRate);
	SetVerticalFogColor(0, 0, 0);
	BackGround::Instance()->Draw();
	SetVerticalFogEnable(false);

	SetFogEnable(false);
	ObjectManager::Instance()->Draw();
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
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* Localize = Util::LocalizePool::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();

	auto& Player = ((std::shared_ptr<Plane>&)PlayerManager::Instance()->SetPlane().at(0));
	{
		int xpos = DrawerMngr->GetDispWidth() / 2;
		int ypos = DrawerMngr->GetDispHeight() * 3 / 4;

		if (true) {
			KeyGuideParts->DrawButton(xpos - 24 / 2, ypos - 24 / 2, DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Reload));
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::MIDDLE, Draw::FontYCenter::TOP,
				xpos, ypos + 18,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(Localize->Get(316)));
			ypos += 52;
		}
	}
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}
void MainScene::Dispose_Sub(void) noexcept {
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->StopAll();
	BackGround::Release();
	PlayerManager::Release();
	ObjectManager::Instance()->DeleteAll();
	this->m_PauseUI.Dispose();
	this->m_OptionWindow.Dispose();
	ObjectManager::Release();
}
