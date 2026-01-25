#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

void MainScene::Load_Sub(void) noexcept {
	ObjectManager::Create();
	PlayerManager::Create();
	BackGround::Create();
	BackGround::Instance()->Load();

	PlayerManager::Instance()->Load();

	m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	m_Lock = Draw::GraphPool::Instance()->Get("data/Image/Lock.png")->Get();
	m_Speed = Draw::GraphPool::Instance()->Get("data/Image/speed.png")->Get();
	m_Meter = Draw::GraphPool::Instance()->Get("data/Image/meter.png")->Get();
	m_Damage = Draw::GraphPool::Instance()->Get("data/Image/damage.png")->Get();
}
void MainScene::Init_Sub(void) noexcept {
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	auto* LightParts = Draw::LightPool::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();

	BackGround::Instance()->Init();

	PlayerManager::Instance()->Init();

	auto& Player = ((std::shared_ptr<PlaneCommon>&)PlayerManager::Instance()->SetPlane().at(0));

	Player->SetPos(Util::VECTOR3D::vget(0.f, 300.f * Scale3DRate, 500.f*Scale3DRate), Util::deg2rad(0));
	for (auto& c : PlayerManager::Instance()->SetPlane()) {
		c->SetDamage(InvalidID);
	}
	//
	this->m_Exit = false;
	this->m_Fade = 2.f;

	this->m_cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
	this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);
	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);

	Util::VECTOR3D LightVec = Util::VECTOR3D::vget(-0.3f, -0.7f, 0.3f).normalized();

	PostPassParts->SetAmbientLight(LightVec);

	SetLightEnable(false);
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

	KeyGuideParts->SetGuideFlip();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
}
void MainScene::Update_Sub(void) noexcept {
	auto* KeyMngr = Util::KeyParam::Instance();
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(40.f);

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
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::S), Localize->Get(334));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::A), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::D), Localize->Get(335));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(336));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Run), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Jump), Localize->Get(337));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Attack), Localize->Get(339));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Aim), Localize->Get(338));
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
	auto& Watch = ((std::shared_ptr<PlaneCommon>&)PlayerManager::Instance()->SetPlane().at(0));
	//
	CameraParts->SetCamInfo(
		CameraParts->GetCamera().GetCamFov() * ((Watch->GetSpeed() - Watch->GetSpeedMax()) / Watch->GetSpeedMax() * 0.35f + 1.f),
		CameraParts->GetCamera().GetCamNear(), CameraParts->GetCamera().GetCamFar());

	CameraParts->SetCamShake(1.f, std::fabsf(Watch->GetSpeed() - Watch->GetSpeedMax()) / (Watch->GetSpeedMax() * 2.f) * Scale3DRate);
	// 影をセット
	PostPassParts->SetShadowFarChange();
	//ポーズメニュー
	if(!this->m_Exit && (this->m_Fade <= 1.f)){
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
			this->m_Exit = true;
		}
		this->m_OptionWindow.Update();
	}
	if (this->m_IsPauseActive) {
		return;
	}
	for (auto& c : PlayerManager::Instance()->SetPlane()) {
		c->SetDamage(InvalidID);
	}
	if (this->m_Fade <= 1.f) {
		ObjectManager::Instance()->UpdateObject();
	}
	//更新
	if (Watch->GetHitPoint() != 0) {
		Util::Matrix4x4 EyeMat = Watch->GetEyeMatrix();
		CamTarget = EyeMat.pos();
		CamPosition = CamTarget - EyeMat.zvec() * (-10.f * Scale3DRate);
		CamUp = EyeMat.yvec();
	}
	else {
		this->m_Exit = true;
	}

	CameraParts->SetCamPos(CamPosition, CamTarget, CamUp);

	BackGround::Instance()->Update();

	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DrawerMngr->GetDeltaTime(), 0.f, 2.f);
	if (!m_Exit) {
	}
	else {
		if (this->m_Fade >= 2.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Title)));
			SceneBase::SetEndScene();
		}
	}

	//
	{
		PostPassParts->SetScopeParam().m_IsActive = false;
	}
	this->m_AimPointDraw = false;
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
	auto& Watch = ((std::shared_ptr<PlaneCommon>&)PlayerManager::Instance()->SetPlane().at(0));

	auto* DrawerMngr = Draw::MainDraw::Instance();
	{
		auto Pos2D = ConvWorldPosToScreenPos((Watch->GetMat().pos() + Watch->GetMat().zvec2() * (25.f * Scale3DRate)).get());
		if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
			this->m_AimPointDraw = true;
			this->m_AimPoint2D_Near.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
			this->m_AimPoint2D_Near.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
		}
	}
	{
		auto Pos2D = ConvWorldPosToScreenPos((Watch->GetMat().pos() + Watch->GetMat().zvec2() * (50.f * Scale3DRate)).get());
		if (0.f <= Pos2D.z && Pos2D.z <= 1.f) {
			this->m_AimPointDraw = true;
			this->m_AimPoint2D_Far.x = Pos2D.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
			this->m_AimPoint2D_Far.y = Pos2D.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
		}
	}

	BackGround::Instance()->Draw();
	ObjectManager::Instance()->Draw();
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
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* Localize = Util::LocalizePool::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();

	auto& Watch = ((std::shared_ptr<PlaneCommon>&)PlayerManager::Instance()->SetPlane().at(0));

	if(this->m_AimPointDraw) {
		SetDrawBright(0, 128, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Far.x), static_cast<int>(this->m_AimPoint2D_Far.y), 0.5f, 0.f, true);
		SetDrawBright(0, 255, 0);
		m_Cursor->DrawRotaGraph(static_cast<int>(this->m_AimPoint2D_Near.x), static_cast<int>(this->m_AimPoint2D_Near.y), 1.f, 0.f, true);
		SetDrawBright(255, 255, 255);
	}
	if (std::clamp(static_cast<int>(255.f * m_DamageWatch * 0.5f), 0, 255) > 10) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_DamageWatch*0.5f), 0, 255));
		m_Damage->DrawExtendGraph(0, 0, 1920, 1080, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	{
		float speed = Watch->GetSpeed() / (1.f / 60.f / 60.f * 1000.f * Scale3DRate / 60.f);

		Util::Easing(&this->m_SpeedPer, 90.f + speed * 3.f + GetRandf(3.f), 0.9f);

		int X = 1920 / 2 + 765 + static_cast<int>(CameraParts->GetShake().x * 10.f), Y = 1080 - 128 - 64 + static_cast<int>(CameraParts->GetShake().y * 10.f);
		m_Speed->DrawRotaGraph(X, Y, 1.0f, 0.f, true);
		m_Meter->DrawRotaGraph(X, Y, 1.0f, Util::deg2rad(this->m_SpeedPer), true);
	}
	if (this->m_Fade > 0.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_Fade));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	this->m_PauseUI.Draw();
	this->m_OptionWindow.Draw();
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
