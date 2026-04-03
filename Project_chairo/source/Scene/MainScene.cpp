#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

#include "../MainScene/Character.hpp"

void MainScene::Load_Sub(void) noexcept {
	BackGround::Create();

	PlayerManager::Create();
	PlayerManager::Instance()->Load();

	m_AimPoint = std::make_unique<AimPoint>();
	m_AimPoint->Load();

	AmmoPool::Create();
	EffectPool::Create();

	AmmoPool::Instance()->Load();
	EffectPool::Instance()->Load();

	m_NowStage = GameRule::Instance()->GetNextStage();
	m_StageScript.Load(m_NowStage);
	BackGround::Instance()->Load();

	m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	m_Damage = Draw::GraphPool::Instance()->Get("data/Image/damage.png")->Get();

	HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/fall.wav", true);
}
void MainScene::Init_Sub(void) noexcept {
	m_NextEvent = false;
	if (m_StageScript.GetStartEvent() != "") {
		if (!GameRule::Instance()->GetIsStartEvent()) {
			GameRule::Instance()->SetIsStartEvent(true);
			GameRule::Instance()->SetNextEvent(m_StageScript.GetStartEvent());
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Movie)));
			SceneBase::SetEndScene();
			m_NextEvent = true;
		}
	}
	if (!m_NextEvent) {
		Util::SaveData::Instance()->SetParam("Stage", m_NowStage);
	}

	AmmoPool::Instance()->Init();
	EffectPool::Instance()->Init();
	BackGround::Instance()->Init();
	PlayerManager::Instance()->Init();

	auto* PostPassParts = Draw::PostPassEffect::Instance();
	auto* LightParts = Draw::LightPool::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();

	auto& Player = PlayerManager::Instance()->SetPlane();
	Player->SetPlanePosition(Util::VECTOR3D::vget(0.f, 15.f * Scale3DRate, 0.f*Scale3DRate), Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(0)));
	Player->SetDamageOff();
	//
	this->m_Exit = false;
	this->m_Fade = 1.f;

	this->m_NextStage = false;
	this->m_FadeStage = 0.f;

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

	//
	if (!m_NextEvent) {
		Player->SetHitPoint(GameRule::Instance()->GetHP() + 30);
	}

	m_MainUI = std::make_unique<MainUI>();
	m_MainUI->Init();

	KeyGuideParts->SetGuideFlip();

	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);

}
void MainScene::Update_Sub(void) noexcept {
	if (m_NextEvent) {
		return;
	}
	auto* DrawerMngr = Draw::MainDraw::Instance();
	auto* CameraParts = Camera::Camera3D::Instance();
	auto* KeyGuideParts = DXLibRef::KeyGuide::Instance();
	auto* PostPassParts = Draw::PostPassEffect::Instance();
	PostPassParts->SetShadowScale(7.5f);

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
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Q), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::E), Localize->Get(336));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Throttle), "");
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Brake), Localize->Get(337));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Gun), Localize->Get(338));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::Missile), Localize->Get(339));
				KeyGuideParts->AddGuide(DXLibRef::KeyGuide::GetPADStoOffset(Util::EnumBattle::MultiMissile), Localize->Get(340));
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
	auto& Player = PlayerManager::Instance()->SetPlane();
	auto& Watch = PlayerManager::Instance()->SetPlane();
	//
	CameraParts->SetCamInfo(
		CameraParts->GetCamera().GetCamFov() * ((Watch->GetSpeed() - Watch->GetSpeedMax()) / Watch->GetSpeedMax() * 0.35f + 1.f),
		0.1f * Scale3DRate, 300.f * Scale3DRate);

	m_DamagePer = std::max(m_DamagePer - DrawerMngr->GetDeltaTime(), 0.f);
	m_DamageWatch = std::max(m_DamageWatch - DrawerMngr->GetDeltaTime(), 0.f);
	if (m_DamagePer == 0.f) {
		CameraParts->SetCamShake(1.f, std::fabsf(Watch->GetSpeed() - Watch->GetSpeedMax()) / (Watch->GetSpeedMax() * 2.f) * Scale3DRate);
		if (Player->IsDamageOn()) {
			CameraParts->SetCamShake(0.2f, 25.f * Scale3DRate);
			m_DamagePer = 0.2f;
			m_DamageWatch = 2.f;
		}
	}
	Player->SetDamageOff();
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

	//次ステージ処理
	switch (GameRule::Instance()->GetGameType()) {
	case GameType::Normal:
		if (this->m_Fade <= 0.f) {
			auto& s = m_StageScript.EnemyPop().back();
			//最後の敵が死んだら
			if (s.m_EnemyScript.IsActive() && !s.m_EnemyScript.IsAlive()) {
				this->m_NextStage = true;
			}
		}
		break;
	case GameType::AllRange:
		if (this->m_Fade <= 0.f) {
			bool IsClear = true;
			for (auto& s : m_StageScript.EnemyPop()) {
				//敵が生きている
				if (s.m_EnemyScript.IsAlive()) {
					IsClear = false;
				}
			}
			if (IsClear) {
				this->m_NextStage = true;
			}
		}
		break;
	case GameType::Max:
	default:
		break;
	}
	this->m_FadeStage = std::clamp(this->m_FadeStage + (this->m_NextStage ? 3.f : -3.f) * DrawerMngr->GetDeltaTime(), 0.f, 2.f);
	if (this->m_NextStage) {
		if (this->m_FadeStage >= 2.f) {
			GameRule::Instance()->SetNextStage(m_StageScript.GetNextStage());
			GameRule::Instance()->SetIsStartEvent(false);
			if (m_StageScript.GetEndEvent() != "") {
				GameRule::Instance()->SetNextEvent(m_StageScript.GetEndEvent());
				SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Movie)));
				SceneBase::SetEndScene();
			}
			else {
				SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
				SceneBase::SetEndScene();
			}
		}
	}
	//更新
	if (this->m_Fade <= 1.f) {
		m_StageScript.Update();
		//ホーミング対象を探す
		for (auto& s : m_StageScript.EnemyPop()) {
			if (s.m_EnemyScript.IsAlive()) {
			}
		}
		//
		for (auto& a : AmmoPool::Instance()->GetAmmoPer()) {
			if (a->IsActive()) {
				if (a->GetShooterID() == Player->GetObjectID()) {
					for (auto& s : m_StageScript.EnemyPop()) {
						//敵被弾
						if (s.m_EnemyScript.IsActive()) {

							SEGMENT_SEGMENT_RESULT Result;
							Util::GetSegmenttoSegment(s.m_EnemyScript.EnemyObj()->GetMat().pos(), s.m_EnemyScript.EnemyObj()->GetMat().pos(),
								a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
							if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
								Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Result.SegB_MinDist_Pos, 500.f * Scale3DRate);
								a->SetHit(Result.SegB_MinDist_Pos);
								s.m_EnemyScript.EnemyObj()->SetDamage(1);
								break;
							}
						}
					}
				}
				else {
					//自機被弾
					SEGMENT_SEGMENT_RESULT Result;
					Util::GetSegmenttoSegment(Player->GetMat().pos(), Player->GetMat().pos(),
						a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
					if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
						if (Player->IsRollingActive()) {
							AmmoPool::Instance()->ShotAmmo(
								Util::Matrix4x4::RotVec2(Util::VECTOR3D::forward(), a->GetVector().normalized()) *
								Util::Matrix4x4::Mtrans(Player->GetMat().pos()), 25.f + 200.f,
								Player->GetObjectID());
							a->SetHit(Result.SegB_MinDist_Pos);
						}
						else {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Result.SegB_MinDist_Pos, 500.f * Scale3DRate);
							a->SetHit(Result.SegB_MinDist_Pos);
							Player->SetDamageOn(10);
							m_DamageWatch = 2.f;
						}
						break;
					}
				}
			}
		}
		for (auto& a : AmmoPool::Instance()->GetBombPer()) {
			if (a->IsActive()) {
				if (a->GetShooterID() == Player->GetObjectID()) {
					//ホーミング用処理
					if (a->IsSeeker()) {
						//一番近い敵を探す
						float Mag = (1000.f * Scale3DRate) * (1000.f * Scale3DRate);
						int ID = InvalidID;
						for (auto& s : m_StageScript.EnemyPop()) {
							if (s.m_EnemyScript.IsAlive()) {
								auto Vec = a->GetMat().pos() - s.m_EnemyScript.EnemyObj()->GetMat().pos();
								auto sID = s.m_EnemyScript.EnemyObj()->GetObjectID();
								if (Mag > Vec.sqrMagnitude()) {
									bool IsHitID = false;
									//他のボムと同じロックオンIDを取らないようにする
									for (auto& a2 : AmmoPool::Instance()->GetBombPer()) {
										if (a2->IsActive() && (a != a2)) {
											if (sID == a2->GetHomingID()) {
												IsHitID = true;
												break;
											}
										}
									}
									if (!IsHitID) {
										Mag = Vec.sqrMagnitude();
										ID = sID;
									}
								}
							}
						}
						a->SetHomingTarget(ID != InvalidID, ID);
					}
					//ヒット判定
					for (auto& s : m_StageScript.EnemyPop()) {
						if (s.m_EnemyScript.IsActive()) {
							SEGMENT_SEGMENT_RESULT Result;
							Util::GetSegmenttoSegment(s.m_EnemyScript.EnemyObj()->GetMat().pos(), s.m_EnemyScript.EnemyObj()->GetMat().pos(),
								a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
							if (Result.SegA_SegB_MinDist_Square < (2.f * Scale3DRate) * (2.f * Scale3DRate)) {
								Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Result.SegB_MinDist_Pos, 500.f * Scale3DRate);
								a->SetHit(Result.SegB_MinDist_Pos);
								s.m_EnemyScript.EnemyObj()->SetDamage(1);
								break;
							}
						}
					}
				}
			}
		}
		//
		ObjectManager::Instance()->UpdateObject();
		//
		m_DamageWatch = std::max(m_DamageWatch - DrawerMngr->GetDeltaTime(), 1.f - Watch->GetHitPointPer());
	}
	if (Watch->GetHitPoint() != 0) {
		Util::Matrix3x3 EyeMat = Util::Matrix3x3::Get33DX(Watch->GetEyeMatrix());
		CamUp = Watch->GetEyeMatrix().yvec();

		//敵を向く
		for (auto& s : m_StageScript.EnemyPop()) {
			if (s.m_EnemyScript.IsActive() && s.m_EnemyScript.IsAlive()) {
				if (s.m_EnemyScript.GetEnemyType()== EnemyType::AI) {
					//EyeMat = Util::Matrix3x3::RotVec2(Util::VECTOR3D::back(), (s.m_EnemyScript.EnemyObj()->GetMat().pos() - Watch->GetEyeMatrix().pos()).normalized());
					//CamUp = Util::VECTOR3D::up();
					break;
				}
			}
		}
		Util::Easing(&EyeMatR, EyeMat, 0.9f);

		CamTarget = Watch->GetEyeMatrix().pos();
		CamPosition = Watch->GetEyeMatrix().pos() - EyeMatR.zvec() * (-15.f * Scale3DRate);// +Util::VECTOR3D::up() * (10.f * Scale3DRate);
		Util::Easing(&CamUpR, CamUp, 0.9f);
	}
	CameraParts->SetCamPos(CamPosition, CamTarget, CamUpR);
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
	BackGround::Instance()->Draw();
	ObjectManager::Instance()->Draw();
	//カーソル
	m_AimPoint->CalcPoint();
	m_AimPoint->Draw();
}
void MainScene::DrawFront_Sub(void) noexcept {
	ObjectManager::Instance()->DrawFront();

	auto* DrawerMngr = Draw::MainDraw::Instance();
	for (auto& s : m_StageScript.EnemyPop()) {
		//敵が生きている
		if (s.m_EnemyScript.IsAlive()) {
			if (s.m_EnemyScript.EnemyObj()->IsDrawAimPoint()) {
				auto sID = s.m_EnemyScript.EnemyObj()->GetObjectID();
				bool IsHitID = false;
				for (auto& a : AmmoPool::Instance()->GetBombPer()) {
					if (a->IsActive()) {
						if (sID == a->GetHomingID()) {
							IsHitID = true;
							break;
						}
					}
				}
				if (IsHitID) {
					SetDrawBright(255, 0, 0);
				}
				else {
					SetDrawBright(0, 255, 0);
				}
				m_Cursor->DrawRotaGraph(
					static_cast<int>(s.m_EnemyScript.EnemyObj()->GetAimPoint2D().x),
					static_cast<int>(s.m_EnemyScript.EnemyObj()->GetAimPoint2D().y),
					50.f*Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth()))
					/(s.m_EnemyScript.EnemyObj()->GetAimPoint2D().z)
					
					,
					0.f, true);
				SetDrawBright(255, 255, 255);
			}
		}
	}
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
	if (std::clamp(static_cast<int>(255.f * m_DamageWatch * 0.5f), 0, 255) > 10) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_DamageWatch * 0.5f), 0, 255));
		m_Damage->DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//
	this->m_MainUI->Draw();
	//
	if (this->m_Fade > 0.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_Fade));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//
	if (this->m_FadeStage > 0.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.f * this->m_FadeStage));
		DxLib::DrawBox(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), ColorPalette::Black, true);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//
}
void MainScene::Dispose_Sub(void) noexcept {
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->StopAll();

	auto& Player = PlayerManager::Instance()->SetPlane();
	GameRule::Instance()->SetHP(Player->GetHitPoint());

	//Util::SaveData::Instance()->Save();

	this->m_MainUI->Dispose();
	this->m_MainUI.reset();

	m_StageScript.Dispose();

	ObjectManager::Instance()->DeleteAll();

	PlayerManager::Release();
	BackGround::Release();
	AmmoPool::Release();
	EffectPool::Release();
}
