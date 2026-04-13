#include "MainScene.hpp"

#include "../MainScene/BackGround.hpp"

#include "../MainScene/Character.hpp"

void MainScene::Load_Sub(void) noexcept {
	this->m_NowStage = GameRule::Instance()->GetNextStage();
	this->m_StageScript.Load(this->m_NowStage);
	this->m_NextEvent = false;
	if (this->m_StageScript.GetStartEvent() != "") {
		if (!GameRule::Instance()->GetIsStartEvent()) {
			GameRule::Instance()->SetIsStartEvent(true);
			this->m_NextEvent = true;
		}
	}

	PlayerManager::Create();
	PlayerManager::Instance()->Load();

	this->m_AimPoint = std::make_unique<AimPoint>();
	this->m_AimPoint->Load();

	AmmoPool::Create();
	AmmoPool::Instance()->Load();

	EffectPool::Create();
	EffectPool::Instance()->Load();

	BackGround::Create();
	BackGround::Instance()->Load();

	this->m_Cursor = Draw::GraphPool::Instance()->Get("data/Image/Cursor.png")->Get();
	this->m_Damage = Draw::GraphPool::Instance()->Get("data/Image/damage.png")->Get();

	this->HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/fall.wav", true);
	this->m_EnviID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/Envi.wav", false);
}
void MainScene::Init_Sub(void) noexcept {
	if (m_NextEvent) {
		GameRule::Instance()->SetNextEvent(m_StageScript.GetStartEvent());
		SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Movie)));
		SceneBase::SetEndScene();
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
	auto Pos = this->m_StageScript.GetStartPos(); Pos.y = 15.f * Scale3DRate;
	auto MovePoint = this->m_StageScript.GetStartPos(); MovePoint.y = this->m_StageScript.GetStartPos().y - 15.f * Scale3DRate; MovePoint.z = 0.f;
	Player->SetPlanePosition(Pos, Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(0)), MovePoint);
	Player->SetDamageOff();
	//
	this->m_Exit = false;
	this->m_Fade = 1.f;
	this->m_NextStage = false;
	this->m_FadeStage = 0.f;
	//Light
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
	//GodRay
	PostPassParts->SetGodRayPer(0.25f);
	//
	if (!m_NextEvent) {
		Player->SetHitPoint(GameRule::Instance()->GetHP() + 30);
	}
	m_MainUI = std::make_unique<MainUI>();
	m_MainUI->Init();

	KeyGuideParts->SetGuideFlip();
	Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_EnviID)->Play(DX_PLAYTYPE_LOOP, TRUE);
	EyeMatR = Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(0));
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
	Util::Easing(&m_ManeuverFovPer, Watch->GetIsManeuver() ? 0.75f : 1.f, 0.95f);
	CameraParts->SetCamInfo(
		CameraParts->GetCamera().GetCamFov() * ((Watch->GetSpeed() - Watch->GetSpeedMax()) / Watch->GetSpeedMax() * 0.35f + 1.f) * m_ManeuverFovPer,
		0.1f * Scale3DRate, 300.f * Scale3DRate);

	m_DamagePer = std::max(m_DamagePer - DrawerMngr->GetDeltaTime(), 0.f);
	m_DamageWatch = std::max(m_DamageWatch - DrawerMngr->GetDeltaTime(), 0.f);
	if (m_DamagePer == 0.f) {
		if (m_ManeuverFovPer > 1.01f) {
			CameraParts->SetCamShake(1.f, 2.f * Scale3DRate);
		}
		else {
			CameraParts->SetCamShake(1.f, std::fabsf(Watch->GetSpeed() - Watch->GetSpeedMax()) / (Watch->GetSpeedMax() * 2.f) * Scale3DRate);
		}
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
	if (!this->m_Exit && (this->m_Fade <= 1.f) && !this->m_NextStage) {
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
		//マニューバターゲット選定
		{
			auto vec1 = Player->GetMat().zvec2();
			float Dot = -1.f;
			auto Prev = Player->GetManeuverID();
			Player->SetManeuverTargetID(InvalidID);

			switch (GameRule::Instance()->GetGameType()) {
			case GameType::Normal:
				m_ManeuverActive = 0.f;
				break;
			case GameType::AllRange:
			{
				for (auto& s : m_StageScript.EnemyPop()) {
					if (!s.m_EnemyScript.IsAlive()) { continue; }
					if (s.m_EnemyScript.GetEnemyType() != EnemyType::AI) { continue; }
					auto vec2 = s.m_EnemyScript.EnemyObj()->GetMat().pos() - Player->GetMat().pos();
					if (vec2.magnitude() > 100.f * Scale3DRate) { continue; }//彼我の距離が100m以内
					auto vec3 = s.m_EnemyScript.EnemyObj()->GetMat().zvec2();
					float dot = Util::VECTOR3D::Dot(vec1, vec2.normalized());
					float dot2 = Util::VECTOR3D::Dot(vec1, vec3.normalized());
					if (dot < cos(Util::deg2rad(75))) { continue; }//敵の位置が自分の前方左右75度以内
					if (dot2 < cos(Util::deg2rad(75))) { continue; }//彼我の向きが左右75度以内
					if (Dot < dot) {
						Dot = dot;
						Player->SetManeuverTargetID(s.m_EnemyScript.EnemyObj()->GetObjectID());
						m_ManeuverPos2D = s.m_EnemyScript.EnemyObj()->GetDamagePoint().at(0).Pos2D;
					}
				}
				if (Prev != Player->GetManeuverID()) {
					m_ManeuverActive = 0.5f;
				}
				if (Player->GetManeuverID() != InvalidID) {
					m_ManeuverActive = std::clamp(m_ManeuverActive + DrawerMngr->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
				else {
					m_ManeuverActive = std::clamp(m_ManeuverActive - DrawerMngr->GetDeltaTime() / 0.25f, 0.f, 1.f);
				}
			}
				break;
			case GameType::Max:
			default:
				break;
			}
		}
		//警報
		{
			bool IsAlert = false;
			switch (GameRule::Instance()->GetGameType()) {
			case GameType::Normal:
				break;
			case GameType::AllRange:
				for (auto& s : m_StageScript.EnemyPop()) {
					if (!s.m_EnemyScript.IsAlive()) { continue; }
					if (s.m_EnemyScript.GetEnemyType() != EnemyType::AI) { continue; }
					bool IsInsight = true;
					auto vec1 = s.m_EnemyScript.EnemyObj()->GetMat().zvec2();
					auto vec2 = Player->GetMat().pos() - s.m_EnemyScript.EnemyObj()->GetMat().pos();
					auto vec3 = Player->GetMat().zvec2();
					float dot = Util::VECTOR3D::Dot(vec1, vec2.normalized());
					float dot2 = Util::VECTOR3D::Dot(vec1, vec3.normalized());
					if (dot < cos(Util::deg2rad(75))) { IsInsight = false; }//敵の位置が自分の前方左右75度以内
					if (dot2 < cos(Util::deg2rad(75))) { IsInsight = false; }//彼我の向きが左右75度以内
					if (IsInsight) {
						IsAlert = true;
						break;
					}
				}
				break;
			case GameType::Max:
			default:
				break;
			}

			this->m_MainUI->SetIsAlert(IsAlert);
		}
		//地面や敵機との激突判定
		{
			//ヒット判定
			auto Ret = BackGround::Instance()->GetCol().CollCheck_Line(Player->GetRePos(), Player->GetMat().pos());
			if (Ret.HitFlag == TRUE) {
				Player->SetDamageOn(10);
			}
			for (auto& s : m_StageScript.EnemyPop()) {
				if (s.m_EnemyScript.IsAlive()) {
					bool IsHit = false;
					Util::VECTOR3D Pos;
					switch (s.m_EnemyScript.GetEnemyType()) {
					case EnemyType::BOSS:
					{
						auto Res = s.m_EnemyScript.EnemyObj()->SetColModel().CollCheck_Line(Player->GetRePos(), Player->GetMat().pos());
						if (Res.HitFlag == TRUE) {
							IsHit = true;
							Pos = Res.HitPosition;
						}
					}
					break;
					case EnemyType::Normal:
					case EnemyType::AI:
					case EnemyType::Max:
					default:
					{
						SEGMENT_SEGMENT_RESULT Result;
						Util::GetSegmenttoSegment(s.m_EnemyScript.EnemyObj()->GetMat().pos(), s.m_EnemyScript.EnemyObj()->GetMat().pos(),
							Player->GetRePos(), Player->GetMat().pos(), &Result);
						if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
							IsHit = true;
							Pos = Result.SegB_MinDist_Pos;
						}
					}
					break;
					}

					if (IsHit) {
						Player->SetDamageOn(10);
						break;
					}
				}
			}
		}
		//弾
		for (auto& a : AmmoPool::Instance()->GetAmmoPer()) {
			if (a->IsActive()) {
				if (a->GetShooterID() == Player->GetObjectID()) {
					for (auto& s : m_StageScript.EnemyPop()) {
						//敵被弾
						if (s.m_EnemyScript.IsActive()) {
							bool IsHit = false;
							Util::VECTOR3D Pos;
							switch (s.m_EnemyScript.GetEnemyType()) {
							case EnemyType::BOSS:
							{
								auto Res = s.m_EnemyScript.EnemyObj()->SetColModel().CollCheck_Line(a->GetMat().pos() - a->GetVector(), a->GetMat().pos());
								if (Res.HitFlag == TRUE) {
									IsHit = true;
									Pos = Res.HitPosition;
									Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
									a->SetHit(Pos);
								}
							}
							break;
							case EnemyType::Normal:
							case EnemyType::AI:
							case EnemyType::Max:
							default:
							{
								{
									auto& dp = s.m_EnemyScript.EnemyObj()->SetDamagePoint().at(0);
									if (!s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
										auto Post = s.m_EnemyScript.EnemyObj()->GetMat().pos();
										SEGMENT_SEGMENT_RESULT Result;
										Util::GetSegmenttoSegment(Post, Post,
											a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
										if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
											IsHit = true;
											Pos = Result.SegB_MinDist_Pos;
											Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
											a->SetHit(Pos);
											dp.SetDamage(1);
											break;
										}
									}
								}
								for (auto& dp : s.m_EnemyScript.EnemyObj()->SetDamagePoint()) {
									if (s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
										auto Post = s.m_EnemyScript.EnemyObj()->GetFrameLocalWorldMatrix(dp.frame).pos();
										SEGMENT_SEGMENT_RESULT Result;
										Util::GetSegmenttoSegment(Post, Post,
											a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
										if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
											IsHit = true;
											Pos = Result.SegB_MinDist_Pos;
											Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
											a->SetHit(Pos);
											dp.SetDamage(1);
											break;
										}
									}
								}
							}
							break;
							}

							if (IsHit) {
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
								Player->GetObjectID(), a->GetScale());
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
		//ミサイル
		for (auto& a : AmmoPool::Instance()->GetBombPer()) {
			if (a->IsActive()) {
				if (a->GetShooterID() == Player->GetObjectID()) {
					//ホーミング用処理
					if (a->IsSeeker()) {
						//一番近い敵を探す
						float Mag = (1000.f * Scale3DRate) * (1000.f * Scale3DRate);
						std::pair<int, int> ID = std::make_pair(InvalidID, InvalidID);
						for (auto& s : m_StageScript.EnemyPop()) {
							if (!s.m_EnemyScript.IsAlive()) { continue; }
							{
								auto& dp = s.m_EnemyScript.EnemyObj()->SetDamagePoint().at(0);
								if (!s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
									if (s.m_EnemyScript.IsActive() && dp.GetHitPoint() > 0) {
										auto Pos = s.m_EnemyScript.EnemyObj()->GetMat().pos();
										auto Vec = a->GetMat().pos() - Pos;
										auto sID = std::make_pair(s.m_EnemyScript.EnemyObj()->GetObjectID(), InvalidID);
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
							}
							for (auto& dp : s.m_EnemyScript.EnemyObj()->SetDamagePoint()) {
								if (s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
									if (s.m_EnemyScript.IsActive() && dp.GetHitPoint() > 0) {
										auto Pos = s.m_EnemyScript.EnemyObj()->GetFrameLocalWorldMatrix(dp.frame).pos();
										auto Vec = a->GetMat().pos() - Pos;
										auto sID = std::make_pair(s.m_EnemyScript.EnemyObj()->GetObjectID(), dp.frame);
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
							}
						}
						a->SetHomingTarget(ID.first != InvalidID, ID.first, ID.second);
					}
					//ヒット判定
					for (auto& s : m_StageScript.EnemyPop()) {
						if (s.m_EnemyScript.IsActive()) {
							bool IsHit = false;
							Util::VECTOR3D Pos;
							switch (s.m_EnemyScript.GetEnemyType()) {
							case EnemyType::BOSS:
							{
								{
									auto& dp = s.m_EnemyScript.EnemyObj()->SetDamagePoint().at(0);
									if (!s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
										auto Post = s.m_EnemyScript.EnemyObj()->GetMat().pos();
										SEGMENT_SEGMENT_RESULT Result;
										Util::GetSegmenttoSegment(Post, Post,
											a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
										if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
											IsHit = true;
											Pos = Result.SegB_MinDist_Pos;
											Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
											a->SetHit(Pos);
											dp.SetDamage(1);
										}
									}
								}
								for (auto& dp : s.m_EnemyScript.EnemyObj()->SetDamagePoint()) {
									if (s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
										auto Post = s.m_EnemyScript.EnemyObj()->GetFrameLocalWorldMatrix(dp.frame).pos();
										SEGMENT_SEGMENT_RESULT Result;
										Util::GetSegmenttoSegment(Post, Post,
											a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
										if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
											IsHit = true;
											Pos = Result.SegB_MinDist_Pos;
											Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
											a->SetHit(Pos);
											dp.SetDamage(1);
										}
									}
								}
								auto Res = s.m_EnemyScript.EnemyObj()->SetColModel().CollCheck_Line(a->GetMat().pos() - a->GetVector(), a->GetMat().pos());
								if (Res.HitFlag == TRUE) {
									IsHit = true;
									Pos = Res.HitPosition;
									Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
									a->SetHit(Pos);
								}
							}
							break;
							case EnemyType::Normal:
							case EnemyType::AI:
							case EnemyType::Max:
							default:
							{
								auto& dp = s.m_EnemyScript.EnemyObj()->SetDamagePoint().at(0);
								if (!s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
									auto Post = s.m_EnemyScript.EnemyObj()->GetMat().pos();
									SEGMENT_SEGMENT_RESULT Result;
									Util::GetSegmenttoSegment(Post, Post,
										a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
									if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
										IsHit = true;
										Pos = Result.SegB_MinDist_Pos;
										Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
										a->SetHit(Pos);
										dp.SetDamage(1);
									}
								}
							}
								for (auto& dp : s.m_EnemyScript.EnemyObj()->SetDamagePoint()) {
									if (s.m_EnemyScript.EnemyObj()->HaveFrame(dp.frame)) {
										auto Post = s.m_EnemyScript.EnemyObj()->GetFrameLocalWorldMatrix(dp.frame).pos();
										SEGMENT_SEGMENT_RESULT Result;
										Util::GetSegmenttoSegment(Post, Post,
											a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
										if (Result.SegA_SegB_MinDist_Square < dp.m_Radius * dp.m_Radius) {
											IsHit = true;
											Pos = Result.SegB_MinDist_Pos;
											Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Pos, 500.f * Scale3DRate);
											a->SetHit(Pos);
											dp.SetDamage(1);
										}
									}
								}
								break;
							}

							if (IsHit) {
								break;
							}
						}
					}
				}
				else {
					//ホーミング用処理
					if (a->IsSeeker()) {
						std::pair<int, int> ID = std::make_pair(InvalidID, InvalidID);
						{
							if (Player->GetHitPoint() > 0) {
								ID = std::make_pair(Player->GetObjectID(), InvalidID);
							}
						}
						a->SetHomingTarget(ID.first != InvalidID, ID.first, ID.second);
					}
					//自機被弾
					SEGMENT_SEGMENT_RESULT Result;
					Util::GetSegmenttoSegment(Player->GetMat().pos(), Player->GetMat().pos(),
						a->GetMat().pos(), a->GetMat().pos() - a->GetVector(), &Result);
					if (Result.SegA_SegB_MinDist_Square < (5.f * Scale3DRate) * (5.f * Scale3DRate)) {
						if (Player->IsRollingActive()) {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, HitHumanID)->Play3D(Result.SegB_MinDist_Pos, 500.f * Scale3DRate);
							a->SetHit(Result.SegB_MinDist_Pos);
							Player->SetDamageOn(1);
							m_DamageWatch = 2.f;
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
			if (s.m_EnemyScript.IsAlive()) {
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

	auto& Player = PlayerManager::Instance()->SetPlane();
	auto* DrawerMngr = Draw::MainDraw::Instance();

	if (0.f < m_ManeuverActive && m_ManeuverActive < 1.f) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * m_ManeuverActive), 0, 255));
		DxLib::SetDrawBright(255, 255, 0);
		m_Cursor->DrawRotaGraph(
			static_cast<int>(m_ManeuverPos2D.x),
			static_cast<int>(m_ManeuverPos2D.y),
			50.f * Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())) / (m_ManeuverPos2D.z) *
			Util::Lerp(10.f, 1.f, m_ManeuverActive),
			0.f, true);
		DxLib::SetDrawBright(255, 255, 255);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	for (auto& s : m_StageScript.EnemyPop()) {
		//敵が生きている
		if (s.m_EnemyScript.IsAlive()) {
			for (auto& dp : s.m_EnemyScript.EnemyObj()->GetDamagePoint()) {
				if (dp.IsDraw && dp.GetHitPoint() > 0) {
					auto sID = std::make_pair(s.m_EnemyScript.EnemyObj()->GetObjectID(), dp.frame);
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
						DxLib::SetDrawBright(255, 0, 0);
					}
					else {
						if (sID.first == Player->GetManeuverID()) {
							DxLib::SetDrawBright(255, 255, 0);
						}
						else {
							DxLib::SetDrawBright(0, 255, 0);
						}
					}
					m_Cursor->DrawRotaGraph(
						static_cast<int>(dp.Pos2D.x),
						static_cast<int>(dp.Pos2D.y),
						50.f * Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())) / (dp.Pos2D.z),
						0.f, true);
					DxLib::SetDrawBright(255, 255, 255);

					{
						int XS = static_cast<int>(400.f * 10.f * Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())) / (dp.Pos2D.z)),
							YS = static_cast<int>(32.f * 10.f * Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())) / (dp.Pos2D.z));
						int XP = static_cast<int>(dp.Pos2D.x) - XS / 2,
							YP = static_cast<int>(dp.Pos2D.y)
							+ static_cast<int>(200.f * 10.f * Scale3DRate / (static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth())) / (dp.Pos2D.z));
						int R = std::clamp(static_cast<int>(Util::Lerp(512.f, 0.f, dp.GetHitPointPer())), 0, 255);
						int G = std::clamp(static_cast<int>(Util::Lerp(0.f, 512.f, dp.GetHitPointPer())), 0, 255);
						DrawBox(XP, YP, XP + static_cast<int>(static_cast<float>(XS) * dp.GetHitPointPer()), YP + YS, GetColor(R, G, 0), true);
						DrawBox(XP, YP, XP + XS, YP + YS, ColorPalette::Green, false);
					}
				}
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
