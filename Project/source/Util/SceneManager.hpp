#pragma once

#pragma warning(disable:4464)
#include "Util.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/KeyGuide.hpp"
#include "../Draw/FontDraw.hpp"

#include "../MainScene/BaseObject.hpp"

#pragma warning(disable:4710)
#pragma warning( push, 3 )
#include <vector>
#pragma warning( pop )

namespace Util {
	class SceneBase {
		const SceneBase*	m_pNextScene{ nullptr };
		int					m_SceneID{ 0 };
		bool				m_IsEndGame{ false };
		bool				m_IsEndScene{ false };
		char		padding[2]{};
	public:
		SceneBase(void) noexcept {}
		virtual ~SceneBase(void) noexcept {}
	public:
		auto GetID(void) const noexcept { return this->m_SceneID; }
		const SceneBase* GetNextScene(void) const noexcept { return this->m_pNextScene; }
		bool IsEndGame(void) const noexcept { return this->m_IsEndGame; }
		bool IsEndScene(void) const noexcept { return this->m_IsEndScene; }
	protected:
		virtual void SetID(int SceneID) noexcept { this->m_SceneID = SceneID; }
	public:
		void SetNextScene(const SceneBase* Ptr) noexcept { this->m_pNextScene = Ptr; }
		void SetEndGame(void) noexcept { this->m_IsEndGame = true; }
		void SetEndScene(void) noexcept { this->m_IsEndScene = true; }
	public:
		void Load(void) noexcept { Load_Sub(); }
		void Init(void) noexcept {
			this->m_IsEndGame = false;
			this->m_IsEndScene = false;
			Init_Sub();
		}
		void Update(void) noexcept { Update_Sub(); }
		void BGDraw(void) noexcept { BGDraw_Sub(); }
		void SetShadowDrawRigid(void) noexcept { SetShadowDrawRigid_Sub(); }
		void SetShadowDraw(void) noexcept { SetShadowDraw_Sub(); }
		void Draw(void) noexcept { Draw_Sub(); }
		void DepthDraw(void) noexcept { DepthDraw_Sub(); }
		void ShadowDrawFar(void) noexcept { ShadowDrawFar_Sub(); }
		void ShadowDraw(void) noexcept { ShadowDraw_Sub(); }
		void UIDraw(void) noexcept { UIDraw_Sub(); }
		void Dispose(void) noexcept { Dispose_Sub(); }
	public:
		virtual void Load_Sub(void) noexcept = 0;
		virtual void Init_Sub(void) noexcept = 0;
		virtual void Update_Sub(void) noexcept = 0;
		virtual void BGDraw_Sub(void) noexcept = 0;
		virtual void SetShadowDrawRigid_Sub(void) noexcept = 0;
		virtual void SetShadowDraw_Sub(void) noexcept = 0;
		virtual void Draw_Sub(void) noexcept = 0;
		virtual void DepthDraw_Sub(void) noexcept = 0;
		virtual void ShadowDrawFar_Sub(void) noexcept = 0;
		virtual void ShadowDraw_Sub(void) noexcept = 0;
		virtual void UIDraw_Sub(void) noexcept = 0;
		virtual void Dispose_Sub(void) noexcept = 0;
	};

	class SceneManager : public SingletonBase<SceneManager> {
	private:
		friend class SingletonBase<SceneManager>;
	private:
		enum class EnumScenePhase : size_t {
			Load,
			Update,
			GoNext,
			GoEnd,
		};
	private:
		SceneBase*						m_FirstScene{ nullptr };
		std::vector<const SceneBase*>	m_pScene{};
		SceneBase*						m_NowScene{ nullptr };
		EnumScenePhase					m_Phase{ EnumScenePhase::GoNext };
		int								m_ASyncLoadNum{ 0 };
		bool							m_LoadEndSwitch{ false };
		bool							m_IsUpdateActive{ false };
		char		padding[2]{};
	private:
		SceneManager(void) noexcept {
			this->m_NowScene = nullptr;
			this->m_Phase = EnumScenePhase::GoNext;
		}
		SceneManager(const SceneManager&) = delete;
		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;
		virtual ~SceneManager(void) noexcept {}
	public:
		void BGDraw(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->BGDraw();
			}
		}
		void ShadowFarDraw3D(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->ShadowDrawFar();
			}
		}
		void ShadowDraw3D(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->ShadowDraw();
			}
		}
		void SetShadowDrawRigid(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->SetShadowDrawRigid();
			}
		}
		void SetShadowDraw(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->SetShadowDraw();
			}
		}
		void Draw3D(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->Draw();
			}
		}
		void DepthDraw3D(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->DepthDraw();
			}
		}
		void UIDraw(void) noexcept {
			if (this->m_Phase == EnumScenePhase::Load || this->m_LoadEndSwitch) {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				// 背景
				DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, TRUE);
				// 
				auto* Font = Draw::FontPool::Instance();
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
					Draw::FontXCenter::RIGHT, Draw::FontYCenter::BOTTOM,
					(DrawerMngr->GetDispWidth() - 32), (DrawerMngr->GetDispHeight() - 32), ColorPalette::Green, ColorPalette::Black,
					"Loading...%d / %d", this->m_ASyncLoadNum - GetASyncLoadNum(), this->m_ASyncLoadNum);
				return;
			}
			if (this->m_NowScene) {
				this->m_NowScene->UIDraw();
			}
			auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
			KeyGuideParts->Draw();
		}
	public:
		auto IsEndScene(void) const noexcept { return (this->m_Phase == EnumScenePhase::GoEnd); }
		const SceneBase* GetScene(int SceneID) const noexcept {
			for (auto& pS : this->m_pScene) {
				if (pS->GetID() == SceneID) {
					return pS;
				}
			}
			return nullptr;
		}
	public:
		void AddScenePtrList(const SceneBase* Ptr) noexcept { this->m_pScene.emplace_back(Ptr); }
		void SetFirstScene(const SceneBase* Ptr) noexcept { this->m_FirstScene = const_cast<SceneBase*>(Ptr); }
	public:
		void Update(void) noexcept {
#if _DEBUG
			if (!this->m_FirstScene) {
				MessageBox(NULL, "FirstScene doesnt Setting Yet", "", MB_OK);
				exit(-1);
			}
#endif
			auto* CameraParts = Camera::Camera3D::Instance();
			auto* pOption = Util::OptionParam::Instance();
			Set3DSoundListenerPosAndFrontPosAndUpVec(CameraParts->GetCameraForDraw().GetCamPos().get(), CameraParts->GetCameraForDraw().GetCamVec().get(), CameraParts->GetCameraForDraw().GetCamUp().get());		// 音位置指定
			CameraParts->SetCamInfo(Util::deg2rad(pOption->GetParam(pOption->GetOptionType(Util::OptionType::Fov))->GetSelect() / 2), 0.5f, Scale3DRate * 60.0f);

			switch (this->m_Phase) {
			case EnumScenePhase::Load:
				if (this->m_NowScene) {
					if (GetASyncLoadNum() == 0) {
						this->m_Phase = EnumScenePhase::Update;
						//
						auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
						KeyGuideParts->SetGuideFlip();
						this->m_NowScene->Init();
						this->m_LoadEndSwitch = true;
					}
				}
				break;
			case EnumScenePhase::Update:
				this->m_LoadEndSwitch = false;
				if (this->m_NowScene) {
					this->m_NowScene->Update();
					if (this->m_NowScene->IsEndScene()) {
						this->m_Phase = EnumScenePhase::GoNext;
					}
					if (this->m_NowScene->IsEndGame()) {
						this->m_Phase = EnumScenePhase::GoEnd;
					}
				}
				break;
			case EnumScenePhase::GoNext:
				if (this->m_NowScene) {
					this->m_NowScene->Dispose();
				}
				if (!this->m_NowScene) {
					this->m_NowScene = this->m_FirstScene;
				}
				else {
					this->m_NowScene = const_cast<SceneBase*>(this->m_NowScene->GetNextScene());
				}
				if (this->m_NowScene) {
					auto* PostPassParts = Draw::PostPassEffect::Instance();
					PostPassParts->Reset();
					auto* LightParts = Draw::LightPool::Instance();
					LightParts->Dispose();
					auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
					KeyGuideParts->Dispose();
					SetUseASyncLoadFlag(true);
					this->m_NowScene->Load();
					SetUseASyncLoadFlag(false);
					this->m_Phase = EnumScenePhase::Load;
					this->m_ASyncLoadNum = GetASyncLoadNum();
				}
				break;
			case EnumScenePhase::GoEnd:
				if (this->m_NowScene) {
					this->m_NowScene->Dispose();
					auto* PostPassParts = Draw::PostPassEffect::Instance();
					PostPassParts->Reset();
					auto* LightParts = Draw::LightPool::Instance();
					LightParts->Dispose();
					auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
					KeyGuideParts->Dispose();
				}
				break;
			default:
				break;
			}
		}
	};
}
