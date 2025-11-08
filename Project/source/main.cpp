#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "Util/SceneManager.hpp"
#include "Util/Enum.hpp"
#include "Util/Key.hpp"
#include "Util/Sound.hpp"
#include "Util/CharaAnim.hpp"
#include "Scene/TitleScene.hpp"
#include "Scene/MainScene.hpp"
#include "Draw/MainDraw.hpp"
#include "Draw/Light.hpp"
#include "Draw/KeyGuide.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Util::OptionParam::Create();

	Draw::MainDraw::Create();

	Util::SceneManager::Create();
	Util::KeyParam::Create();
	Draw::FontPool::Create();
	Draw::GraphPool::Create();
	Util::LocalizePool::Create();
	Camera::Camera3D::Create();
	Draw::PostPassEffect::Create();
	Draw::LightPool::Create();
	DXLibRef::KeyGuide::Create();
	Sound::SoundPool::Create();
	Util::HandAnimPool::Create();

	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* pOption = Util::OptionParam::Instance();
	auto* SceneMngr = Util::SceneManager::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();

	TitleScene Title{};
	MainScene Main{};
	//シーン設定
	SceneMngr->AddScenePtrList(&Title);
	SceneMngr->AddScenePtrList(&Main);
	SceneMngr->SetFirstScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Title)));
	//メインループ開始
	DrawerMngr->LoopStart();
	while (DrawerMngr->CanContinueLoop()) {
		//更新
		DrawerMngr->Update();
		for (int loop = 0; loop < DrawerMngr->GetUpdateTickCount(); ++loop) {
			Util::KeyParam::Instance()->Update();
			SceneMngr->Update();
			CameraParts->Update();
			// 影をセット
			PostPassParts->UpdateShadowActive();
			if (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0) {
				if (PostPassParts->PopShadowFarChange()) {
					Util::VECTOR3D Pos = CameraParts->GetCameraForDraw().GetCamPos();
					Pos.x = 0.f;
					Pos.z = 0.f;
					PostPassParts->Update_Shadow([]() {
						auto* SceneMngr = Util::SceneManager::Instance();
						SceneMngr->ShadowFarDraw3D();
						}, Pos, true);
				}
				PostPassParts->Update_Shadow([]() {
					auto* SceneMngr = Util::SceneManager::Instance();
					SceneMngr->ShadowDraw3D();
					}, CameraParts->GetCameraForDraw().GetCamPos(), false);
				PostPassParts->SetDrawShadow([]() {
					auto* SceneMngr = Util::SceneManager::Instance();
					SceneMngr->SetShadowDrawRigid();
					}, []() {
						auto* SceneMngr = Util::SceneManager::Instance();
						SceneMngr->SetShadowDraw();
						});
			}
			//Depth描画
			{
				PostPassParts->SetDepthDraw([]() {
					auto* SceneMngr = Util::SceneManager::Instance();
					SceneMngr->DepthDraw3D();
					});
			}
		}
		//描画
		{
			// 全ての画面を初期化
			PostPassParts->StartDraw();
			// 空
			PostPassParts->DrawGBuffer(1000.0f, 50000.0f, []() {
				auto* SceneMngr = Util::SceneManager::Instance();
				SceneMngr->BGDraw();
				});
			// 3距離
			{
				float Far = 1000000.f;
				float Near = CameraParts->GetCameraForDraw().GetCamFar();
				for (int loop = 0; loop < 3; ++loop) {
					PostPassParts->DrawGBuffer(Near, Far, []() {
						auto* SceneMngr = Util::SceneManager::Instance();
						SceneMngr->Draw3D();
						});
					Far = Near;
					if (loop == 0) {
						Near = CameraParts->GetCameraForDraw().GetCamNear();
					}
					else if (loop == 1) {
						Near = 0.01f;
					}
				}
			}
			// ポストプロセス
			PostPassParts->EndDraw();
		}
		DrawerMngr->StartDraw();
		{
			PostPassParts->GetBufferScreen().DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), false);
			SceneMngr->UIDraw();
		}
		DrawerMngr->EndDraw();
		//終了判定
		if (SceneMngr->IsEndScene()) {
			break;
		}
	}

	Util::HandAnimPool::Release();
	Sound::SoundPool::Release();
	DXLibRef::KeyGuide::Release();
	Draw::LightPool::Release();
	Draw::PostPassEffect::Release();
	Camera::Camera3D::Release();
	Util::LocalizePool::Release();
	Draw::GraphPool::Release();
	Draw::FontPool::Release();
	Util::KeyParam::Release();
	Util::SceneManager::Release();

	Draw::MainDraw::Release();

	Util::OptionParam::Release();
	return 0;// ソフトの終了 
}
