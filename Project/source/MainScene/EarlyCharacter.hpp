#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BackGround.hpp"
#include "../Util/Enum.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Sound.hpp"
#include "../Draw/MV1.hpp"
#include "../File/FileStream.hpp"

#include "../Util/CharaAnim.hpp"

#include "BaseObject.hpp"

#include "Character.hpp"

enum class EarlyCharaAnim {
	Stand,//立ち
	Walk,//歩き

	Upper,//左に向く

	DownTop,//仰向きに倒れる
	WakeTop,//仰向きから立つ

	DownBottom,//仰向きに倒れる
	WakeBottom,//仰向きから立つ

	ArmlockedStart,
	ArmlockedInjector,
	ArmlockedEnd,

	ArmlockStart,
	ArmlockEnd,

	Punch,

	Max,
};

namespace AIs {
	class PathChecker {
	public:
		// 経路探索処理用の１ポリゴンの情報
		class PATHPLANNING_UNIT {
			int							m_PolyIndex{};		// ポリゴン番号
			float						m_TotalDistance{};	// 経路探索でこのポリゴンに到達するまでに通過したポリゴン間の距離の合計
			PATHPLANNING_UNIT*			m_PrevPolyUnit{};		// 経路探索で確定した経路上の一つ前のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
			PATHPLANNING_UNIT*			m_NextPolyUnit{};		// 経路探索で確定した経路上の一つ先のポリゴン( 当ポリゴンが経路上に無い場合は nullptr )
		public:
			PATHPLANNING_UNIT*			m_ActiveNextUnit{};	// 経路探索処理対象になっている次のポリゴンのメモリアドレスを格納する変数
		public:
			const auto& GetPolyIndex(void) const noexcept { return this->m_PolyIndex; }
			const auto& GetNextPolyUnit(void) const noexcept { return this->m_NextPolyUnit; }
		public:
			bool SetPrevPolyUnit(PATHPLANNING_UNIT* PUnit, int tris) {
				auto* BackGroundParts = BackGround::Instance();
				// 隣接するポリゴンが既に経路探索処理が行われていて、且つより距離の長い経路となっている場合は何もしない
				auto& Unit = BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(PUnit->m_PolyIndex));

				auto trisdistance = PUnit->m_TotalDistance +
					(BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(Unit.GetLinkPolyIndex(tris))).GetPos() + Unit.GetPos()).magnitude();

				if (this->m_TotalDistance > trisdistance) {
					this->m_TotalDistance = trisdistance;		// 隣接するポリゴンにここに到達するまでの距離を代入する
				}
				else {
					if (this->m_PrevPolyUnit) { return false; }
				}
				this->m_PrevPolyUnit = PUnit;			// 隣接するポリゴンに経路情報となる自分のポリゴンの番号を代入する
				return true;
			}
			bool SearchThisUnit(PATHPLANNING_UNIT* SearchUnit) {
				// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
				PATHPLANNING_UNIT* PUnitSub2 = SearchUnit;
				while (true) {
					if (PUnitSub2 == nullptr) { break; }
					if (PUnitSub2 == this) { return false; }//既に追加されとる
					PUnitSub2 = PUnitSub2->m_ActiveNextUnit;
				}
				return PUnitSub2 != nullptr;
			}
		public:
			// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
			static void SetNextIndex(PATHPLANNING_UNIT* pGoal, PATHPLANNING_UNIT* pStart) {
				PATHPLANNING_UNIT* PUnit = pGoal;
				while (true) {
					auto* PrevPUnitIndex = PUnit;
					PUnit = PUnit->m_PrevPolyUnit;
					PUnit->m_NextPolyUnit = PrevPUnitIndex;
					if (PUnit == pStart) { break; }
				}
			}
		public:
			void Init(int index) {
				this->m_PolyIndex = index;
				this->m_TotalDistance = 0.0f;
				this->m_PrevPolyUnit = nullptr;
				this->m_NextPolyUnit = nullptr;
				this->m_ActiveNextUnit = nullptr;
			}
		};
	private:
		Util::VECTOR3D					m_GoalPosition;					// 目標位置
		char		padding[4]{};
		std::vector<PATHPLANNING_UNIT>	m_UnitArray;					// 経路探索処理で使用する全ポリゴンの情報配列が格納されたメモリ領域の先頭メモリアドレスを格納する変数
		PATHPLANNING_UNIT*				m_StartUnit{ nullptr };			// 経路のスタート地点にあるポリゴン情報へのメモリアドレスを格納する変数
		PATHPLANNING_UNIT*				m_GoalUnit{ nullptr };			// 経路のゴール地点にあるポリゴン情報へのメモリアドレスを格納する変数
	public:
		const auto& GetStartUnit(void) const noexcept { return this->m_StartUnit; }
	public:
		Util::VECTOR3D GetNextPoint(const Util::VECTOR3D& NowPosition, int* TargetPathPlanningIndex) const {
			auto* BackGroundParts = BackGround::Instance();
			auto Goal = this->m_GoalPosition;
			if (!BackGroundParts->CheckLine(NowPosition, &Goal)) {
				// 方向は目標座標
				return this->m_GoalPosition;
			}
			int NowIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds(NowPosition);
			if (!((*TargetPathPlanningIndex != -1) && (this->m_GoalUnit))) {
				auto Ans = BackGroundParts->GetWayPoint()->GetNearestBuilds2(NowPosition);
				if (Ans == -1) {
					return this->m_GoalPosition;
				}
				return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(Ans)).GetPos();
			}
			if (NowIndex != this->m_GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
				if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
					auto NextIndex = this->m_UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()->GetPolyIndex();
					(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
				}
				// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
				return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(*TargetPathPlanningIndex)).GetPos();
			}
			else {
				// 方向は目標座標
				return this->m_GoalPosition;
			}
			/*
			{
				int NowIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds(NowPosition);
				if (!((*TargetPathPlanningIndex != -1) && (this->m_GoalUnit))) {
					return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(BackGroundParts->GetWayPoint()->GetNearestBuilds2(NowPosition))).GetPos();
				}
				if (NowIndex != this->m_GoalUnit->GetPolyIndex()) {																	// 現在乗っているポリゴンがゴール地点にあるポリゴンの場合は処理を分岐
					if (NowIndex == *TargetPathPlanningIndex) {													// 現在乗っているポリゴンが移動中間地点のポリゴンの場合は次の中間地点を決定する処理を行う
						const float COLLWIDTH = 0.001f * Scale3DRate;												// 当たり判定のサイズ
						while (true) {																				// 次の中間地点が決定するまでループし続ける
							if (!this->m_UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()) { break; }
							auto NextIndex = this->m_UnitArray.at(static_cast<size_t>(*TargetPathPlanningIndex)).GetNextPolyUnit()->GetPolyIndex();
							if (!BackGroundParts->GetWayPoint()->CheckPolyMoveWidth(NowPosition, NextIndex, COLLWIDTH)) { break; }		// 経路上の次のポリゴンの中心座標に直線的に移動できない場合はループから抜ける
							(*TargetPathPlanningIndex) = NextIndex;													// チェック対象を経路上の更に一つ先のポリゴンに変更する
							if ((*TargetPathPlanningIndex) == this->m_GoalUnit->GetPolyIndex()) { break; }				// もしゴール地点のポリゴンだったらループを抜ける
						}
					}
					// 移動方向を決定する、移動方向は現在の座標から中間地点のポリゴンの中心座標に向かう方向
					return BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(*TargetPathPlanningIndex)).GetPos();
				}
				else {
					// 方向は目標座標
					return this->m_GoalPosition;
				}
			}
			//*/
		}
	public:
		bool Init(Util::VECTOR3D StartPos, Util::VECTOR3D GoalPos) {
			auto* BackGroundParts = BackGround::Instance();
			// 指定の２点の経路を探索する( 戻り値 true:経路構築成功 false:経路構築失敗( スタート地点とゴール地点を繋ぐ経路が無かった等 ) )
			this->m_GoalPosition = GoalPos;			// ゴール位置を保存

			this->m_UnitArray.resize(BackGroundParts->GetWayPoint()->GetWayPoints().size());			// 経路探索用のポリゴン情報を格納するメモリ領域を確保、初期化
			for (auto& p : this->m_UnitArray) {
				p.Init(static_cast<int>(&p - &this->m_UnitArray.front()));
			}

			int StartIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds2(StartPos);	// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (StartIndex == -1) { return false; }
			this->m_StartUnit = &this->m_UnitArray[static_cast<size_t>(StartIndex)];					// スタート地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存

			int GoalIndex = BackGroundParts->GetWayPoint()->GetNearestBuilds2(GoalPos);		// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (GoalIndex == -1) { return false; }
			this->m_GoalUnit = &this->m_UnitArray[static_cast<size_t>(GoalIndex)];				// ゴール地点にあるポリゴンの番号を取得し、ポリゴンの経路探索処理用の構造体のアドレスを保存
			if (GoalIndex == StartIndex) { return false; }				// ゴール地点にあるポリゴンとスタート地点にあるポリゴンが同じだったら false を返す

			PATHPLANNING_UNIT* ActiveFirstUnit = this->m_StartUnit;		// 経路探索処理対象のポリゴンとしてスタート地点にあるポリゴンを登録する
			// 経路を探索してゴール地点のポリゴンにたどり着くまでループを繰り返す
			while (true) {
				bool Goal = false;
				// 経路探索処理対象になっているポリゴンをすべて処理
				PATHPLANNING_UNIT* PUnit = ActiveFirstUnit;
				ActiveFirstUnit = nullptr;
				while (true) {
					// ポリゴンの辺の数だけ繰り返し
					for (int K = 0; K < 8; K++) {
						int Index = BackGroundParts->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(PUnit->GetPolyIndex())).GetLinkPolyIndex(K);
						if (Index == -1) { continue; }											// 辺に隣接するポリゴンが無い場合は何もしない
						if (Index == this->m_StartUnit->GetPolyIndex()) { continue; }				//スタート地点のポリゴンだった場合は何もしない
						auto& NowUnit = this->m_UnitArray[static_cast<size_t>(Index)];
						if (!NowUnit.SetPrevPolyUnit(PUnit, K)) {
							continue;
						}
						// 次のループで行う経路探索処理対象に追加する、既に追加されていたら追加しない
						if (!NowUnit.SearchThisUnit(ActiveFirstUnit)) {
							NowUnit.m_ActiveNextUnit = ActiveFirstUnit;
							ActiveFirstUnit = &NowUnit;
						}
						// 隣接するポリゴンがゴール地点にあるポリゴンだったらゴールに辿り着いたフラグを立てる
						if (Index == this->m_GoalUnit->GetPolyIndex()) {
							Goal = true;
						}
					}
					PUnit = PUnit->m_ActiveNextUnit;
					if (PUnit == nullptr) { break; }
				}

				if (!ActiveFirstUnit) { return false; }			// スタート地点にあるポリゴンからゴール地点にあるポリゴンに辿り着けないということなので false を返す
				if (Goal) { break; }
			}
			PATHPLANNING_UNIT::SetNextIndex(this->m_GoalUnit, this->m_StartUnit);		// ゴール地点のポリゴンからスタート地点のポリゴンに辿って経路上のポリゴンに次に移動すべきポリゴンの番号を代入する
			return true;										// ここにきたらスタート地点からゴール地点までの経路が探索できたということなので true を返す
		}
		void Draw(void) const noexcept {
			PATHPLANNING_UNIT* Now = GetStartUnit();
			while (true) {
				if (!Now) { break; }
				Util::VECTOR3D Vec1 = BackGround::Instance()->GetWayPoint()->GetWayPoints().at(static_cast<size_t>(Now->GetPolyIndex())).GetPos();

				DrawSphere3D(
					Vec1.get(),
					0.2f * Scale3DRate / 2.f,
					6,
					DxLib::GetColor(255, 255, 128),
					DxLib::GetColor(255, 255, 0),
					true
				);
				//
				if (Now == this->m_GoalUnit) {
					break;
				}
				Now = Now->GetNextPolyUnit();
			}
		}
		void Dispose(void) {
			this->m_UnitArray.clear();
		}
	};
}

class MarkDraw {
	const Draw::GraphHandle*	m_Graph{};
	float						m_Timer{};
	float						m_Per{};
public:
	MarkDraw(void) noexcept {}
	MarkDraw(const MarkDraw&) = delete;
	MarkDraw(MarkDraw&&) = delete;
	MarkDraw& operator=(const MarkDraw&) = delete;
	MarkDraw& operator=(MarkDraw&&) = delete;
	virtual ~MarkDraw(void) noexcept {}
public:
	bool IsActive() const noexcept { return this->m_Timer != 0.f; }
	void SetActive() noexcept { this->m_Timer = 3.f; }
	void SetDisActive() noexcept { this->m_Timer = 0.f; }
public:
	void Init(std::string_view FilePath)noexcept {
		this->m_Graph = Draw::GraphPool::Instance()->Get(FilePath)->Get();
		this->m_Timer = 0.f;
		this->m_Per = 0.f;
	}
	void Update() noexcept {
		this->m_Timer = std::max(this->m_Timer - DeltaTime, 0.f);
		Util::Easing(&this->m_Per, this->m_Timer, 0.9f);
	}
	void Draw(const  Util::VECTOR3D& Pos, int ColorR, int ColorG, int ColorB) const noexcept {
		if (this->m_Per > 0.f) {
			SetUseLighting(false);
			DxLib::SetDrawBright(ColorR, ColorG, ColorB);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(this->m_Per * 5.f * 255.f), 0, 255));
			DxLib::DrawBillboard3D(
				(Pos + Util::VECTOR3D::up() * (0.25f * Scale3DRate * std::clamp(this->m_Per * 5.f, 0.f, 1.f))).get(),
				0.5f,
				0.5f,
				0.25f * Scale3DRate,
				0.f,
				this->m_Graph->get(),
				true
			);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			SetUseLighting(true);
			DxLib::SetDrawBright(255, 255, 255);
		}
	}
};

class EarlyCharacter :public CharacterCommon {
	std::array<float, static_cast<int>(EarlyCharaAnim::Max)>		m_AnimPer{};

	Util::VECTOR3D		m_MyTarget = Util::VECTOR3D::zero();
	AIs::PathChecker	m_PathChecker{};
	int					m_TargetPathPlanningIndex{ 0 };		// 次の中間地点となる経路上のポリゴンの経路探索情報が格納されているメモリアドレスを格納する変数
	float				m_PathUpdateTimer{ 0.f };

	SpecialAction		m_DownTop{};
	SpecialAction		m_DownBottom{};

	float				m_KeepPower{ 0.f };
	float				m_KeepTimer{ 0.f };

	Util::VECTOR3D		m_HitVec{};
	float				m_HitPower{ 0.f };
	float				m_HitBack{ 0.f };

	Util::VECTOR3D		m_DownVec{};
	float				m_DownPower{ 0.f };
	char		padding[4]{};

	Sound::SoundUniqueID	m_PunchID{ InvalidID };
	Sound::SoundUniqueID	m_HitHumanID{ InvalidID };
	Sound::SoundUniqueID	m_DownHumanID{ InvalidID };
	Sound::SoundUniqueID	m_ArmlockStartID{ InvalidID };
	Sound::SoundUniqueID	m_ArmlockID{ InvalidID };

	MarkDraw			m_Alert{};
	MarkDraw			m_Caution{};

	SpecialAction		m_Armlocked{};
	float				m_ArmlockedInjectorTimer{};
	int					m_ArmlockedPos{};
	bool				m_ArmlockedInjector{ false };
	char		padding2[7]{};

	SpecialAction		m_Punch{};
	SpecialAction		m_Armlock{};

	float				m_AttackCoolDown{ 0.f };
	bool				m_PunchAttack{ false };
	char		padding3[3]{};

	float				m_CanSeePer{};
	Util::VECTOR2D		m_UIPos{};
	bool				m_CanSeeUI{ false };
	char		padding4[3]{};

	float				m_DrugPer{};
	float				m_DrugPerR{};
	const float			m_DrugPerMax{ 100.f };

	float				m_AlertTimer{};
	float				m_CautionTimer{};
	float				m_AlertAnyTimer{};
	float				m_FlipBack{};
	Util::VECTOR3D		m_FlipBackVec{};
	bool				m_IsMove{ false };
	bool				m_IsTurn{ false };
	char		padding5[2]{};
public:
	EarlyCharacter(void) noexcept {}
	EarlyCharacter(const EarlyCharacter&) = delete;
	EarlyCharacter(EarlyCharacter&&) = delete;
	EarlyCharacter& operator=(const EarlyCharacter&) = delete;
	EarlyCharacter& operator=(EarlyCharacter&&) = delete;
	virtual ~EarlyCharacter(void) noexcept {}
public:
	float GetSpeedMax(void) const noexcept { return 2.f * Scale3DRate * DeltaTime; }

	auto GetCanSeeUI(void) const noexcept { return this->m_CanSeeUI; }
	auto GetUIPos(void) const noexcept { return this->m_UIPos; }
	auto GetCanSeePer(void) const noexcept { return this->m_CanSeePer; }

	auto GetDrugPer(void) const noexcept { return this->m_DrugPerR; }
	auto GetDrugPerMax(void) const noexcept { return this->m_DrugPerMax; }

	auto IsDown(void) const noexcept { return (this->m_DownTop.IsActive() || this->m_DownTop.m_End || this->m_DownBottom.IsActive() || this->m_DownBottom.m_End); }

	auto IsWatching(void) const noexcept { return this->m_AlertTimer != 0.f; }
	
	auto GetFindPer(void) const noexcept { return this->m_CautionTimer / 3.f; }
public:
	void		SetTarget(const Util::VECTOR3D& pos) noexcept { this->m_MyTarget = pos; }
	void		SetHit(const Util::VECTOR3D& Vec, float Power) noexcept {
		this->m_KeepPower = std::clamp(this->m_KeepPower + Power, 0.f, 1.f);
		this->m_KeepTimer = 1.f;
		if (this->m_KeepPower >= 1.f) {
			//倒れる
			Util::VECTOR3D A = GetMat().zvec(); A.y = 0.f;
			Util::VECTOR3D B = Vec; B.y = 0.f;
			if (Util::VECTOR3D::Dot(A, B) > 0.f) {
				SetDownTop(B.normalized());
			}
			else {
				SetDownBottom(B.normalized());
			}
			Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_DownHumanID)->Play3D(GetMat().pos(), 10.f * Scale3DRate);
		}
		else {
			//のけぞり
			Util::VECTOR3D A = GetMat().zvec(); A.y = 0.f;
			Util::VECTOR3D B = Vec; B.y = 0.f;
			if (Util::VECTOR3D::Dot(A, B) > 0.f) {
				this->m_HitVec = (B.normalized());
				this->m_HitPower = 0.5f;
				this->m_HitBack = 1.f;
			}
			else {
				this->m_HitVec = (B.normalized()) * -1.f;
				this->m_HitPower = -0.5f;
				this->m_HitBack = -1.f;
			}
		}
	}
	//倒れる
	void		SetDownTop(const Util::VECTOR3D& Vec) noexcept {
		this->m_DownVec = Vec * -1.f;
		if (!this->m_DownTop.IsActive()) {
			SetAnim(static_cast<int>(EarlyCharaAnim::DownTop)).SetTime(0.f);
			this->m_DownTop.m_Time = 3.f;
			this->m_DownPower = -1.f;
		}
		this->m_DownTop.SetActive();
	}
	//倒れる
	void		SetDownBottom(const Util::VECTOR3D& Vec) noexcept {
		this->m_DownVec = Vec;
		if (!this->m_DownBottom.IsActive()) {
			SetAnim(static_cast<int>(EarlyCharaAnim::DownBottom)).SetTime(0.f);
			this->m_DownBottom.m_Time = 3.f;
			this->m_DownPower = 1.f;
		}
		this->m_DownBottom.SetActive();
	}
	//
	void		SetArmlocked(int UniqueID) noexcept {
		this->m_ArmlockedPos = UniqueID;
		this->m_Armlocked.SetActive();
		SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedStart)).SetTime(0.f);
	}
	void		SetArmlockedInjector() noexcept {
		if (!this->m_ArmlockedInjector) {
			this->m_ArmlockedInjector = true;
			SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedInjector)).SetTime(0.f);
		}
	}
	void		SetArmlockedEnd() noexcept {
		if (!this->m_Armlocked.m_End) {
			this->m_Armlocked.m_End = true;
			m_FlipBackVec = GetEyeMatrix().zvec(); m_FlipBackVec.y = 0.f; m_FlipBackVec = m_FlipBackVec.normalized();
			this->m_FlipBack = 1.5f;
			SetAnim(static_cast<int>(EarlyCharaAnim::ArmlockedEnd)).SetTime(0.f);
			this->m_DownBottom.m_Time = 3.f;
			this->m_DownPower = 1.f;
		}
	}

	void		SetDrug(float value) noexcept {
		auto prev = this->m_DrugPer;
		this->m_DrugPer = std::clamp(this->m_DrugPer + value, 0.f, GetDrugPerMax() * 2.f);
		if (this->m_DrugPer != prev && this->m_DrugPer == GetDrugPerMax() * 2.f) {
			SetDownTop(GetMat().zvec());
		}
	}
public:
	void CheckDraw_Sub(void) noexcept override {
		auto* DrawerMngr = Draw::MainDraw::Instance();

		Util::VECTOR3D Pos1 = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Head)).pos();
		Util::VECTOR3D Pos2 = GetCameraPosition();

		//Pos2 = Pos1 + (Pos2 - Pos1) * 0.5f;

		if (BackGround::Instance()->CheckLine(Pos1, &Pos2) == 0) {
			auto Pos = ConvWorldPosToScreenPos(Pos1.get());
			if (0.0f < Pos.z && Pos.z < 1.0f) {
				this->m_UIPos.x = Pos.x * static_cast<float>(DrawerMngr->GetDispWidth()) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
				this->m_UIPos.y = Pos.y * static_cast<float>(DrawerMngr->GetDispHeight()) / static_cast<float>(DrawerMngr->GetRenderDispHeight());
				this->m_CanSeeUI |= true;
			}
		}
	}
public:
	bool IsPlayer(void) noexcept override { return false; }

	void Load_Chara(void) noexcept override {
		this->m_DownHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/DownHuman.wav", true);
		this->m_ArmlockStartID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/ArmlockStart.wav", true);
		this->m_ArmlockID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Armlock.wav", true);
		this->m_HitHumanID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/HitHuman.wav", true);
		this->m_PunchID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/SE/move/Punch.wav", true);

		this->m_Alert.Init("data/Image/Alert.png");
		this->m_Caution.Init("data/Image/Caution.png");
	}
	void Init_Chara(void) noexcept override {
		this->m_PathUpdateTimer = 1.f;

		this->m_DownTop.Init();
		this->m_DownBottom.Init();
		this->m_Armlocked.Init();
		this->m_Punch.Init();
		this->m_Armlock.Init();

		this->m_DownPower = 0.f;

		this->m_DrugPer = 0.f;
		this->m_DrugPerR = 0.f;
	}
	void Update_Chara(void) noexcept override;
	void Draw_Chara(void) const noexcept override {
		auto Pos = GetFrameLocalWorldMatrix(static_cast<int>(CharaFrame::Head)).pos();
		this->m_Alert.Draw(Pos, 255, 0, 0);
		this->m_Caution.Draw(Pos, 0, 255, 0);
	}
	void Dispose_Chara(void) noexcept override {
	}
};
