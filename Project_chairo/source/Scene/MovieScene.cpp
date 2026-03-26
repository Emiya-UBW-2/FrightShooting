#include "MovieScene.hpp"

#include "../Util/Localize.hpp"

void MovieScene::Load_Sub(void) noexcept {
	ObjectManager::Create();

	m_StoryScript.Load("01");
}
void MovieScene::Init_Sub(void) noexcept {
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	auto* LightParts = Draw::LightPool::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();

	//
	this->m_Exit = false;
	this->m_Fade = 1.f;

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

	KeyGuideParts->SetGuideFlip();
}
void MovieScene::Update_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(7.5f);

	KeyGuideParts->ChangeGuide(
		[]() {
			auto* Localize = Util::LocalizePool::Instance();
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Tab), Localize->Get(333));

			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::UP), "");
			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::DOWN), "");
			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::LEFT), "");
			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::RIGHT), Localize->Get(332));
			KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumMenu::Diside), Localize->Get(330));
		}
	);
	CameraParts->SetCamInfo(CameraParts->GetCamera().GetCamFov(), 1.f * Scale3DRate, 200.f * Scale3DRate);
	// 影をセット
	PostPassParts->SetShadowFarChange();
	//ポーズメニュー
	if (m_StoryScript.GetIsEnd()) {
		this->m_Exit = true;
	}
	this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DrawerMngr->GetDeltaTime(), 0.f, 2.f);
	if (m_Exit) {
		if (this->m_Fade >= 2.f) {
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
	}
	//更新
	if (this->m_Fade <= 1.f) {
		m_StoryScript.Update();
		//
		ObjectManager::Instance()->UpdateObject();
	}
}
void MovieScene::BGDraw_Sub(void) noexcept {
}
void MovieScene::SetShadowDrawRigid_Sub(void) noexcept {
}
void MovieScene::SetShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_SetShadow();
}
void MovieScene::Draw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw();
}
void MovieScene::DrawFront_Sub(void) noexcept {
	ObjectManager::Instance()->DrawFront();
}
void MovieScene::DepthDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Depth();
}
void MovieScene::ShadowDrawFar_Sub(void) noexcept {
}
void MovieScene::ShadowDraw_Sub(void) noexcept {
	ObjectManager::Instance()->Draw_Shadow();
}
void MovieScene::UIDraw_Sub(void) noexcept {
	auto* DrawerMngr = Draw::MainDraw::Instance();
	//
	if (this->m_Fade > 0.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_Fade));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//
}
void MovieScene::Dispose_Sub(void) noexcept {
	ObjectManager::Instance()->DeleteAll();
	ObjectManager::Release();
}
