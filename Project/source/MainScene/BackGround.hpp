#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"
#include "../Draw/Voxel.hpp"

enum class InfoType : size_t {
	None,
	Entrance1,
	Entrance2,
	Entrance3,
	Exit1,
	Exit2,
	Exit3,
	WayPoint,
	WayPoint2,
	AmmoBox,
	Max,
};
static const char* InfoTypeStr[static_cast<int>(InfoType::Max)] = {
	"None",
	"Entrance1",
	"Entrance2",
	"Entrance3",
	"Exit1",
	"Exit2",
	"Exit3",
	"WayPoint",
	"WayPoint2",
	"AmmoBox",
};

class WayPointClass {
public:
	class Builds {
		struct LinkBuffer {
			bool isActive{ false };
			char		padding[3]{};
			Util::VECTOR3D Pos{};
			int ID;
		};
	private:
		int							m_mesh{ -1 };
		Util::VECTOR3D					m_MinPos;
		Util::VECTOR3D					m_Pos;
		Util::VECTOR3D					m_MaxPos;
		int							MyIndex{ 0 };
	public:
		std::array<LinkBuffer, 8>	m_LinkPosBuffer{};
	public:
		int	GetIndex(void) const noexcept { return MyIndex; }
		int	GetLinkPolyIndex(int ID) const noexcept { return this->m_LinkPosBuffer[static_cast<size_t>(ID)].ID; }
		void		SetLinkBuffer(int tris, const Util::VECTOR3D& pos) noexcept {
			this->m_LinkPosBuffer[static_cast<size_t>(tris)].isActive = true;
			this->m_LinkPosBuffer[static_cast<size_t>(tris)].Pos = pos;
		}
	public:
		const Util::VECTOR3D& GetMinPos(void) const noexcept { return this->m_MinPos; }
		const Util::VECTOR3D& GetPos(void) const noexcept { return this->m_Pos; }
		const Util::VECTOR3D& GetMaxPos(void) const noexcept { return this->m_MaxPos; }
	public:
		void		Set(int index) noexcept {
			this->MyIndex = index;
			for (auto& L : this->m_LinkPosBuffer) {
				L.isActive = false;
				L.ID = -1;
			}
		}
		void		SetPosition(Util::VECTOR3D MinPos, Util::VECTOR3D MaxPos) noexcept {
			this->m_MinPos = MinPos;
			this->m_MaxPos = MaxPos;
			this->m_Pos = (MinPos + MaxPos) / 2;
		}
	};
private:
	std::vector<Builds>				m_WayPoints;
	char		padding[4]{};
	int								m_SeekPoint{ 0 };
public:
	std::vector<Builds>& SetWayPoints(void) noexcept { return this->m_WayPoints; }
	const std::vector<Builds>& GetWayPoints(void) const noexcept { return this->m_WayPoints; }
public:
	//所定の位置からNm未満かNm以上の座標を取得する
	Util::VECTOR3D		GetRandomPoint(const Util::VECTOR3D& Pos, float RangeSmaller = InvalidID, float RangeOver = InvalidID) const noexcept {
		std::vector<int> SelList;
		SelList.clear();
		for (auto& C : this->m_WayPoints) {
			int index = static_cast<int>(&C - &this->m_WayPoints.front());
			Util::VECTOR3D Vec = C.GetPos() - Pos; Vec.y = (0.f);
			if (RangeSmaller != InvalidID && RangeOver != InvalidID) {
				if (Vec.IsRangeSmaller(RangeSmaller) && !Vec.IsRangeSmaller(RangeOver)) {
					SelList.emplace_back(index);
				}
			}
			else {
				if (RangeSmaller != InvalidID && Vec.IsRangeSmaller(RangeSmaller)) {
					SelList.emplace_back(index);
				}
				if (RangeOver != InvalidID && !Vec.IsRangeSmaller(RangeOver)) {
					SelList.emplace_back(index);
				}
			}
		}
		if (SelList.size() == 0) {
			return Pos;
		}
		return this->m_WayPoints.at(static_cast<size_t>(SelList.at(static_cast<size_t>(GetRand((int)SelList.size() - 1))))).GetPos();
	}

	int		GetNearestBuilds(const Util::VECTOR3D& NowPosition) const noexcept {
		for (auto& bu : this->m_WayPoints) {
			if (
				(bu.GetMaxPos().x >= NowPosition.x && NowPosition.x >= bu.GetMinPos().x) &&
				(bu.GetMaxPos().y >= NowPosition.y && NowPosition.y >= bu.GetMinPos().y) &&
				(bu.GetMaxPos().z >= NowPosition.z && NowPosition.z >= bu.GetMinPos().z)
				) {
				return static_cast<int>(&bu - &this->m_WayPoints.front());
			}
		}
		return -1;
	}
	int		GetNearestBuilds2(const Util::VECTOR3D& NowPosition) const noexcept {
		float Len = 10.f * Scale3DRate * 10.f * Scale3DRate;
		int Answer = -1;
		for (auto& bu : this->m_WayPoints) {
			Util::VECTOR3D Length = bu.GetPos() - NowPosition;
			//Length.y = 0.f;
			if (Len >= Length.sqrMagnitude()) {
				Len = Length.sqrMagnitude();
				Answer = static_cast<int>(&bu - &this->m_WayPoints.front());
			}
		}
		return Answer;
	}
	// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )
	bool			CheckPolyMove(Util::VECTOR3D StartPos, Util::VECTOR3D TargetPos) const {
		int Rate = 6;

		int CheckPoly[4]{};
		int CheckPolyPrev[4]{};
		int NextCheckPoly[4]{};
		int NextCheckPolyPrev[4]{};

		// 開始座標と目標座標の y座標値を 0.0f にして、平面上の判定にする
		StartPos.y = (0.0f);
		TargetPos.y = (0.0f);

		// 開始座標と目標座標の直上、若しくは直下に存在するポリゴンを検索する
		int StartPoly = this->GetNearestBuilds(StartPos);
		int TargetPoly = this->GetNearestBuilds(TargetPos);

		// ポリゴンが存在しなかったら移動できないので false を返す
		if (StartPoly == -1 || TargetPoly == -1) { return false; }

		// 指定線分上にあるかどうかをチェックするポリゴンとして開始座標の直上、若しくは直下に存在するポリゴンを登録
		int CheckPolyNum = 1;
		CheckPoly[0] = StartPoly;
		int CheckPolyPrevNum = 0;
		CheckPolyPrev[0] = -1;

		// 結果が出るまで無条件で繰り返し
		while (true) {
			int NextCheckPolyNum = 0;			// 次のループでチェック対象になるポリゴンの数をリセットしておく
			int NextCheckPolyPrevNum = 0;			// 次のループでチェック対象から外すポリゴンの数をリセットしておく
			// チェック対象のポリゴンの数だけ繰り返し
			for (int i = 0; i < CheckPolyNum; i++) {
				int Index = CheckPoly[i];
				// チェック対象のポリゴンの３座標を取得 y座標を0.0にして、平面的な判定を行うようにする
				Util::VECTOR3D Pos = this->m_WayPoints.at(static_cast<size_t>(Index)).GetPos(); Pos.y = (0.f);

				for (int K = 0; K < 8; K++) {
					int LinkIndex = this->m_WayPoints.at(static_cast<size_t>(Index)).GetLinkPolyIndex(K);

					;

					Util::VECTOR3D PolyPos = Pos;
					PolyPos.x += ((static_cast<float>(Rate) / 2.f) * ((K == 0 || K == 1) ? 1.f : -1.f));
					PolyPos.z += ((static_cast<float>(Rate) / 2.f) * ((K == 1 || K == 2) ? 1.f : -1.f));
					int K2 = (K + 1) % 4;
					Util::VECTOR3D PolyPos2 = Pos;
					PolyPos2.x += ((static_cast<float>(Rate) / 2.f) * ((K2 == 0 || K2 == 1) ? 1.f : -1.f));
					PolyPos2.z += ((static_cast<float>(Rate) / 2.f) * ((K2 == 1 || K2 == 2) ? 1.f : -1.f));
					// ポリゴンの頂点番号0と1の辺に隣接するポリゴンが存在する場合で、
					// 且つ辺の線分と移動開始点、終了点で形成する線分が接していたら if 文が真になる
					if (LinkIndex != -1 && Util::GetMinLenSegmentToSegment(StartPos, TargetPos, PolyPos, PolyPos2) < 0.01f) {
						// もし辺と接しているポリゴンが目標座標上に存在するポリゴンだったら 開始座標から目標座標上まで途切れなくポリゴンが存在するということなので true を返す
						if (LinkIndex == TargetPoly) { return true; }

						// 辺と接しているポリゴンを次のチェック対象のポリゴンに加える

						// 既に登録されているポリゴンの場合は加えない
						int j = 0;
						for (j = 0; j < NextCheckPolyNum; j++) {
							if (NextCheckPoly[j] == LinkIndex) { break; }
						}
						if (j == NextCheckPolyNum) {
							// 次のループで除外するポリゴンの対象に加える

							// 既に登録されている除外ポリゴンの場合は加えない
							int j2 = 0;
							for (j2 = 0; j2 < NextCheckPolyPrevNum; j2++) {
								if (NextCheckPolyPrev[j2] == Index) { break; }
							}
							if (j2 == NextCheckPolyPrevNum) {
								NextCheckPolyPrev[NextCheckPolyPrevNum] = Index;
								NextCheckPolyPrevNum++;
							}

							// 一つ前のループでチェック対象になったポリゴンの場合も加えない
							int j3 = 0;
							for (j3 = 0; j3 < CheckPolyPrevNum; j3++) {
								if (CheckPolyPrev[j3] == LinkIndex) { break; }
							}
							if (j3 == CheckPolyPrevNum) {
								// ここまで来たら漸く次のチェック対象のポリゴンに加える
								NextCheckPoly[NextCheckPolyNum] = LinkIndex;
								NextCheckPolyNum++;
							}
						}
					}
				}
			}

			// 次のループでチェック対象になるポリゴンが一つもなかったということは
			// 移動開始点、終了点で形成する線分と接するチェック対象のポリゴンに隣接する
			// ポリゴンが一つもなかったということなので、直線的な移動はできないということで false を返す
			if (NextCheckPolyNum == 0) { return false; }

			// 次にチェック対象となるポリゴンの情報をコピーする
			for (int i = 0; i < NextCheckPolyNum; i++) {
				CheckPoly[i] = NextCheckPoly[i];
			}
			CheckPolyNum = NextCheckPolyNum;

			// 次にチェック対象外となるポリゴンの情報をコピーする
			for (int i = 0; i < NextCheckPolyPrevNum; i++) {
				CheckPolyPrev[i] = NextCheckPolyPrev[i];
			}
			CheckPolyPrevNum = NextCheckPolyPrevNum;
		}
	}
	bool			CheckPolyMoveWidth(Util::VECTOR3D StartPos, int TargetIndex, float Width) const {
		// ポリゴン同士の連結情報を使用して指定の二つの座標間を直線的に移動できるかどうかをチェックする( 戻り値 true:直線的に移動できる false:直線的に移動できない )( 幅指定版 )
		Util::VECTOR3D TargetPos = this->m_WayPoints.at(static_cast<size_t>(TargetIndex)).GetPos();
		// 最初に開始座標から目標座標に直線的に移動できるかどうかをチェック
		if (CheckPolyMove(StartPos, TargetPos) == false) { return false; }

		Util::VECTOR3D Direction = TargetPos - StartPos;		// 開始座標から目標座標に向かうベクトルを算出
		Direction.y = (0.0f);		// y座標を 0.0f にして平面的なベクトルにする

		// 開始座標から目標座標に向かうベクトルに直角な正規化ベクトルを算出
		Util::VECTOR3D SideDirection = Util::VECTOR3D::Cross(Direction, Util::VECTOR3D::up()).normalized();
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			Util::VECTOR3D TempVec = SideDirection * (Width / 2.0f);
			if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
				return false;
			}
		}
		{
			// 開始座標と目標座標を Width / 2.0f 分だけ一つ前とは逆方向の垂直方向にずらして、再度直線的に移動できるかどうかをチェック
			Util::VECTOR3D TempVec = SideDirection * (-Width / 2.0f);
			if (CheckPolyMove(StartPos + TempVec, TargetPos + TempVec) == false) {
				return false;
			}
		}
		return true;		// ここまできたら指定の幅があっても直線的に移動できるということなので true を返す
	}
public:
	void			Init(int Count) noexcept {
		this->m_WayPoints.resize(static_cast<size_t>(Count));
		this->m_SeekPoint = 0;
	}
	auto& AddWayPoint(Util::VECTOR3D MinPos, Util::VECTOR3D MaxPos) noexcept {
		auto& w = this->m_WayPoints.at(static_cast<size_t>(this->m_SeekPoint));
		w.Set(this->m_SeekPoint);
		w.SetPosition(MinPos, MaxPos);
		++this->m_SeekPoint;
		return w;
	}
	void			Setup(void) noexcept {
		for (auto& w : this->m_WayPoints) {
			for (auto& L : w.m_LinkPosBuffer) {
				if (L.isActive) {
					L.ID = GetNearestBuilds(L.Pos);
				}
			}
		}
	}
};

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
	class mapGraph {
		const Draw::GraphHandle*	m_Map{};
		int					m_ID{};
	public:
		float				m_Per{};
	public:
		const auto& GetMap(void) const noexcept { return this->m_Map; }
		auto GetID(void) const noexcept { return this->m_ID; }
		auto GetPer(void) const noexcept { return this->m_Per; }
	public:
		mapGraph(void) noexcept {}
		mapGraph(const mapGraph&) = delete;
		mapGraph(mapGraph&& o) noexcept {
			this->m_Map = o.m_Map;
			this->m_ID = o.m_ID;
			this->m_Per = o.m_Per;
		}
		mapGraph& operator=(const mapGraph&) = delete;
		mapGraph& operator=(mapGraph&& o) noexcept {
			this->m_Map = o.m_Map;
			this->m_ID = o.m_ID;
			this->m_Per = o.m_Per;
			return *this;
		}
		virtual ~mapGraph(void) noexcept {}
	public:
		void Init(std::basic_string_view<TCHAR> FileName, int ID) noexcept {
			this->m_Map = Draw::GraphPool::Instance()->Get(FileName)->Get();
			this->m_ID = ID;
			this->m_Per = 0.f;
		}
	};
	struct MapInfo {
		InfoType					m_InfoType{ InfoType::None };
		BG::Algorithm::Vector3Int	m_pos{};
		char		padding[4]{};
	};
private:
	std::vector<MapInfo>	m_MapInfo;
	BG::VoxelControl		Voxel;
	Draw::MV1				SkyBoxID{};
	std::vector<mapGraph>	m_map;
	std::string				m_MapName;
	std::unique_ptr<WayPointClass>		m_WayPoint;
private:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
	auto			GetVoxelPoint(const Util::VECTOR3D& StartPos) const noexcept {
		BG::Algorithm::Vector3Int Pos = Voxel.GetReferenceCells().GetVoxelPoint(StartPos);
		Pos.x -= Voxel.GetReferenceCells().All / 2;
		Pos.z -= Voxel.GetReferenceCells().All / 2;
		return Pos;
	}
	auto			GetWorldPos(const BG::Algorithm::Vector3Int& StartPos) const noexcept {
		BG::Algorithm::Vector3Int Pos = StartPos;
		//Pos.x += Voxel.GetReferenceCells().All / 2;
		//Pos.z += Voxel.GetReferenceCells().All / 2;
		return Voxel.GetReferenceCells().GetWorldPos(Pos);
	}
	int				CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal = nullptr) const noexcept {
		return Voxel.CheckLine(StartPos, EndPos, Normal);
	}
	bool			CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius, const std::vector<const Draw::MV1*>& addonColObj) const noexcept {
		return Voxel.CheckWall(StartPos, EndPos, AddCapsuleMin, AddCapsuleMax, Radius, addonColObj);
	}
	auto&			GetMapGraph(void) noexcept { return this->m_map; }
	const auto&		GetMapInfo(void) const noexcept { return this->m_MapInfo; }
	void			SettingChange(int DrawLOD, int ShadowLOD) noexcept { Voxel.SettingChange(DrawLOD, ShadowLOD); }
	const auto&		GetWayPoint(void) const noexcept { return this->m_WayPoint; }
public:
	void Load(const char* MapName) noexcept {
		this->m_MapName = MapName;
		Voxel.Load(("data/" + this->m_MapName + "/tex.png").c_str());							// 事前読み込み
		Draw::MV1::Load("data/model/SkyBox/model.mqoz", &SkyBoxID);
		for (int loop = 0; loop < Voxel.GetReferenceCells().All; ++loop) {
			std::string Path = "data/" + this->m_MapName + "/map";
			Path += std::to_string(loop);
			Path += ".png";
			if (Util::IsFileExist(Path.c_str())) {
				this->m_map.emplace_back();
				this->m_map.back().Init(Path, loop);
			}
		}
	}
	void Init(void) noexcept {
		Voxel.InitStart();											// 初期化開始時処理
		Voxel.LoadCellsFile(("data/" + this->m_MapName + "/Map.txt").c_str());					// ボクセルデータの読み込み
		Voxel.InitEnd();											// 初期化終了時処理
		this->m_WayPoint = std::make_unique<WayPointClass>();
		this->m_MapInfo.clear();
		if (std::filesystem::is_regular_file("data/" + this->m_MapName + "/Event.txt")) {
			std::ifstream ifs("data/" + this->m_MapName + "/Event.txt");
			while (true) {
				std::string Buffer;
				std::getline(ifs, Buffer);
				std::string LEFT = Buffer.substr(0, Buffer.find("="));
				std::string RIGHT = Buffer.substr(Buffer.find("=") + 1);
				if (LEFT == "Type") {
					this->m_MapInfo.emplace_back();
					for (int loop = 0; loop < static_cast<int>(InfoType::Max); ++loop) {
						if (RIGHT == InfoTypeStr[static_cast<size_t>(loop)]) {
							this->m_MapInfo.back().m_InfoType = static_cast<InfoType>(loop);
							break;
						}
					}
				}
				else if (LEFT == "X") {
					this->m_MapInfo.back().m_pos.x = std::stoi(RIGHT);
				}
				else if (LEFT == "Y") {
					this->m_MapInfo.back().m_pos.y = std::stoi(RIGHT);
				}
				else if (LEFT == "Z") {
					this->m_MapInfo.back().m_pos.z = std::stoi(RIGHT);
				}
				if (ifs.eof()) { break; }
			}
		}
		//経路探索
		{
			for (size_t p1 = 0; p1 < this->m_MapInfo.size(); ++p1) {
				auto& m = this->m_MapInfo.at(p1);
				if (m.m_InfoType != InfoType::WayPoint) { continue; }
				auto Pos1 = Voxel.GetReferenceCells().GetWorldPos(m.m_pos) + Util::VECTOR3D::up() * (1.f * Scale3DRate);
				int ID = 0;
				for (size_t p2 = 0; p2 < this->m_MapInfo.size(); ++p2) {
					auto& m2 = this->m_MapInfo.at(p2);
					if (p1 == p2) { continue; }
					if (ID >= 8) { continue; }
					if (m2.m_InfoType != InfoType::WayPoint) { continue; }
					auto Pos2 = Voxel.GetReferenceCells().GetWorldPos(m2.m_pos) + Util::VECTOR3D::up() * (1.f * Scale3DRate);
					if (!CheckLine(Pos1, &Pos2)) {
						//線分の間を埋める
						auto Vec = Pos2 - Pos1;
						int Len = static_cast<int>(Vec.magnitude()) + 1;
						for (int loop = 0; loop <= Len; ++loop) {
							float Per = static_cast<float>(loop) / static_cast<float>(Len);
							this->m_MapInfo.emplace_back();
							this->m_MapInfo.back().m_InfoType = InfoType::WayPoint2;

							auto Pos = Util::Lerp(Pos1, Pos2, Per) - Util::VECTOR3D::up() * (1.f * Scale3DRate);
							auto Pos3 = Pos - Util::VECTOR3D::up() * (2.f * Scale3DRate);
							if (CheckLine(Pos, &Pos3)) {
								Pos = Pos3;
							}

							this->m_MapInfo.back().m_pos = Voxel.GetReferenceCells().GetVoxelPoint(Pos);
						}
						++ID;
					}
				}
			}
			for (size_t p1 = 0; p1 < this->m_MapInfo.size(); ++p1) {
				auto& m = this->m_MapInfo.at(p1);
				if (m.m_InfoType != InfoType::WayPoint2) { continue; }
				for (size_t p2 = 0; p2 < this->m_MapInfo.size(); ++p2) {
					auto& m2 = this->m_MapInfo.at(p2);
					if (p1 == p2) { continue; }
					if (!(m2.m_InfoType == InfoType::WayPoint2)) { continue; }
					if ((m.m_pos == m2.m_pos) ||
						(m.m_pos.x == m2.m_pos.x && m.m_pos.y == m2.m_pos.y + 1 && m.m_pos.z == m2.m_pos.z) ||
						(m.m_pos.x == m2.m_pos.x && m.m_pos.y == m2.m_pos.y + 2 && m.m_pos.z == m2.m_pos.z)
						) {
						this->m_MapInfo.erase(this->m_MapInfo.begin() + static_cast<int64_t>(p1));
						--p1;
						break;
					}
				}
			}
			int count = 0;
			for (auto& m : this->m_MapInfo) {
				if ((m.m_InfoType == InfoType::WayPoint2)) {
					++count;
				}
			}
			this->m_WayPoint->Init(count);
			for (auto& m : this->m_MapInfo) {
				if (!((m.m_InfoType == InfoType::WayPoint2))) { continue; }
				auto Pos1 = Voxel.GetReferenceCells().GetWorldPos(m.m_pos) + Util::VECTOR3D::up() * (1.f * Scale3DRate);
				auto& bu = this->m_WayPoint->AddWayPoint(
					Pos1 + Util::VECTOR3D::vget(1.f, 1.f, 1.f) * (-0.125f * Scale3DRate),
					Pos1 + Util::VECTOR3D::vget(1.f, 9.f, 1.f) * (0.125f * Scale3DRate)
				);
				int ID = 0;
				for (auto& m2 : this->m_MapInfo) {
					if (ID >= 8) { continue; }
					if (&m == &m2) { continue; }
					if (!((m2.m_InfoType == InfoType::WayPoint2))) { continue; }
					auto Pos2 = Voxel.GetReferenceCells().GetWorldPos(m2.m_pos) + Util::VECTOR3D::up() * (1.f * Scale3DRate);
					auto Vec = Pos2 - Pos1;
					if (Vec.magnitude() >= 0.4f * Scale3DRate) { continue; }
					if (!CheckLine(Pos1, &Pos2)) {
						bu.SetLinkBuffer(ID, Pos2);
						++ID;
					}
				}
			}
			//リンクを確立
			this->m_WayPoint->Setup();
		}
	}
	void Update(void) noexcept {
		auto* CameraParts = Camera::Camera3D::Instance();
		auto* PostPassParts = Draw::PostPassEffect::Instance();
		// ボクセル処理
		Voxel.SetDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(),
			(CameraParts->GetCameraForDraw().GetCamVec() - CameraParts->GetCameraForDraw().GetCamPos()).normalized());// 描画する際の描画中心座標と描画する向きを指定
		Voxel.SetShadowDrawInfo(CameraParts->GetCameraForDraw().GetCamPos(), PostPassParts->GetAmbientLightVec());// シャドウマップに描画する際の描画中心座標と描画する向きを指定
		Voxel.Update();
	}
	void Dispose(void) noexcept {
		Voxel.Dispose();
		Voxel.Dispose_Load();
		this->m_WayPoint.reset();
		SkyBoxID.Dispose();
		this->m_map.clear();
		this->m_MapInfo.clear();
	}

	void BGDraw(void) const noexcept {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {
		Voxel.DrawByShader();
	}
	void SetShadowDraw(void) const noexcept {}
	void Draw(void) const noexcept {
		Voxel.Draw();
		/*
		{
			for (auto& m : this->m_MapInfo) {
				unsigned int Color = 0;
				switch (m.m_InfoType) {
				case InfoType::Entrance1:
					Color = GetColor(255, 0, 0);
					break;
				case InfoType::Entrance2:
					Color = GetColor(255, 64, 0);
					break;
				case InfoType::Entrance3:
					Color = GetColor(255, 128, 0);
					break;
				case InfoType::Exit1:
					Color = GetColor(255, 255, 0);
					break;
				case InfoType::Exit2:
					Color = GetColor(255, 255, 64);
					break;
				case InfoType::Exit3:
					Color = GetColor(255, 255, 128);
					break;
				case InfoType::None:
				case InfoType::Max:
				default:
					break;
				}
				DrawSphere3D(Voxel.GetReferenceCells().GetWorldPos(m.m_pos).get(), 0.125f * Scale3DRate, 8, Color, GetColor(0, 0, 0), TRUE);
			}
		}
		//*/
	}
	void ShadowDrawFar(void) const noexcept {
		Voxel.DrawShadow();
	}
	void ShadowDraw(void) const noexcept {}
};
