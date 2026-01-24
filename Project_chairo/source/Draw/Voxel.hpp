#pragma once
#pragma warning(disable:4464)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#define NOMINMAX
#pragma warning( push, 3 )
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <thread>
#include "DxLib.h"
#pragma warning( pop )

#include "MV1.hpp"
#include "ImageDraw.hpp"
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"

namespace BG {
	// 算術補助
	namespace Algorithm {
		// 三次元整数座標系
		class Vector3Int {
		public:
			int x{};
			int y{};
			int z{};
		public:
			Vector3Int()noexcept {
				this->x = 0;
				this->y = 0;
				this->z = 0;
			}
			Vector3Int(int X, int Y, int Z)noexcept {
				this->x = X;
				this->y = Y;
				this->z = Z;
			}
		public:
			void operator+=(const Vector3Int& o) noexcept {
				this->x += o.x;
				this->y += o.y;
				this->z += o.z;
			}
			const Vector3Int operator+(const Vector3Int& o) const noexcept {
				return Vector3Int(this->x + o.x, this->y + o.y, this->z + o.z);
			}

			bool operator==(const Vector3Int& o) const noexcept {
				return this->x == o.x && this->y == o.y && this->z == o.z;
			}
		};

		// ブレゼンハムアルゴリズムで線分上にボクセルを走査
		inline static void		Bresenham3D(int x1, int y1, int z1, int x2, int y2, int z2, const std::function<bool(const Algorithm::Vector3Int&)>& OutPutLine) noexcept {
			int err_1{}, err_2{};
			int point[3]{};

			point[0] = x1;
			point[1] = y1;
			point[2] = z1;
			const int dx = x2 - x1;
			const int dy = y2 - y1;
			const int dz = z2 - z1;
			const int x_inc = (dx < 0) ? -1 : 1;
			const int Length = std::abs(dx);
			const int y_inc = (dy < 0) ? -1 : 1;
			const int dyAbs = std::abs(dy);
			const int z_inc = (dz < 0) ? -1 : 1;
			const int dzAbs = std::abs(dz);
			const int dx2 = Length << 1;
			const int dy2 = dyAbs << 1;
			const int dz2 = dzAbs << 1;

			if ((Length >= dyAbs) && (Length >= dzAbs)) {
				err_1 = dy2 - Length;
				err_2 = dz2 - Length;
				for (int loop = 0; loop < Length; ++loop) {
					if (OutPutLine(Algorithm::Vector3Int(point[0], point[1], point[2]))) { return; }
					if (err_1 > 0) {
						point[1] += y_inc;
						err_1 -= dx2;
					}
					if (err_2 > 0) {
						point[2] += z_inc;
						err_2 -= dx2;
					}
					err_1 += dy2;
					err_2 += dz2;
					point[0] += x_inc;
				}
			}
			else if ((dyAbs >= Length) && (dyAbs >= dzAbs)) {
				err_1 = dx2 - dyAbs;
				err_2 = dz2 - dyAbs;
				for (int loop = 0; loop < dyAbs; ++loop) {
					if (OutPutLine(Algorithm::Vector3Int(point[0], point[1], point[2]))) { return; }
					if (err_1 > 0) {
						point[0] += x_inc;
						err_1 -= dy2;
					}
					if (err_2 > 0) {
						point[2] += z_inc;
						err_2 -= dy2;
					}
					err_1 += dx2;
					err_2 += dz2;
					point[1] += y_inc;
				}
			}
			else {
				err_1 = dy2 - dzAbs;
				err_2 = dx2 - dzAbs;
				for (int loop = 0; loop < dzAbs; ++loop) {
					if (OutPutLine(Algorithm::Vector3Int(point[0], point[1], point[2]))) { return; }
					if (err_1 > 0) {
						point[1] += y_inc;
						err_1 -= dz2;
					}
					if (err_2 > 0) {
						point[0] += x_inc;
						err_2 -= dz2;
					}
					err_1 += dy2;
					err_2 += dx2;
					point[2] += z_inc;
				}
			}
			OutPutLine(Algorithm::Vector3Int(point[0], point[1], point[2]));
		}
		// AABBと線分の当たり判定
		inline static bool		ColRayBox(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AABBMinPos, const Util::VECTOR3D& AABBMaxPos, Util::VECTOR3D* Normal = nullptr) noexcept {
			Util::VECTOR3D Vec = *EndPos - StartPos;
			// 交差判定
			float start[3]{}, dir[3]{}, min[3]{}, max[3]{};
			start[0] = StartPos.x;
			start[1] = StartPos.y;
			start[2] = StartPos.z;
			dir[0] = Vec.x;
			dir[1] = Vec.y;
			dir[2] = Vec.z;

			min[0] = AABBMinPos.x;
			min[1] = AABBMinPos.y;
			min[2] = AABBMinPos.z;

			max[0] = AABBMaxPos.x;
			max[1] = AABBMaxPos.y;
			max[2] = AABBMaxPos.z;

			float t_min = -FLT_MAX;
			float t_max = FLT_MAX;

			for (int loop = 0; loop < 3; ++loop) {
				/*
				if (abs(dir[loop]) < FLT_EPSILON) {
					if (start[loop] < min[loop] || start[loop] > max[loop])
						return false; // 交差していない
				}
				else
				// */
				{
					// スラブとの距離を算出
					// t1が近スラブ、t2が遠スラブとの距離
					float odd = 1.0f / dir[loop];
					float t1 = (min[loop] - start[loop]) * odd;
					float t2 = (max[loop] - start[loop]) * odd;
					if (t1 > t2) {
						float tmp = t1;
						t1 = t2;
						t2 = tmp;
					}

					if (t1 > t_min) {
						t_min = t1;
					}
					if (t2 < t_max) {
						t_max = t2;
					}

					// スラブ交差チェック
					if (t_min >= t_max) {
						return false;
					}
				}
			}

			// 交差している
			if (EndPos) {
				*EndPos = StartPos + Vec * t_min;
			}
			float ret[3] = { EndPos->x, EndPos->y, EndPos->z };
			// どの向き？
			if (Normal) {
				for (int loop = 0; loop < 3; ++loop) {
					if (std::abs(ret[loop] - min[loop]) < 0.00001f) {
						switch (loop) {
						case 0:
							*Normal = Util::VECTOR3D::vget(-1.f, 0.f, 0.f);
							break;
						case 1:
							*Normal = Util::VECTOR3D::vget(0.f, -1.f, 0.f);
							break;
						case 2:
							*Normal = Util::VECTOR3D::vget(0.f, 0.f, -1.f);
							break;
						default:
							break;
						}
					}
					if (std::abs(ret[loop] - max[loop]) < 0.00001f) {
						switch (loop) {
						case 0:
							*Normal = Util::VECTOR3D::vget(1.f, 0.f, 0.f);
							break;
						case 1:
							*Normal = Util::VECTOR3D::vget(0.f, 1.f, 0.f);
							break;
						case 2:
							*Normal = Util::VECTOR3D::vget(0.f, 0.f, 1.f);
							break;
						default:
							break;
						}
					}
				}
			}

			return true;
		}
		// 線分をDotaDotbの結果から縮める
		inline static bool		CullingLine(int* MaxminT, int* MaxmaxT, float dTa, float dTb) noexcept {
			bool OnFront = (dTa >= 0.0f && dTb >= 0.0f);
			bool Onbehind = (dTa < 0.0f && dTb < 0.0f);
			if (Onbehind && !OnFront) {
				return false;
			}
			if (!OnFront && !Onbehind) {
				if (dTa < 0.0f) {
					*MaxminT = std::max(*MaxminT + static_cast<int>(static_cast<float>(*MaxmaxT - *MaxminT) * (dTa / (dTa - dTb))) - 1, *MaxminT);
				}
				else {
					*MaxmaxT = std::min(*MaxminT + static_cast<int>(static_cast<float>(*MaxmaxT - *MaxminT) * (dTa / (dTa - dTb))) + 1, *MaxmaxT);
				}
			}
			return true;
		};
	}
	// 設定パラメーター
	static constexpr int TotalCellLayer = 4;// LOD段階
	static constexpr int MulPer = 2;// LODに伴う粒度指定
	static constexpr float CellScale = 0.25f * Scale3DRate;// 1個のブロックの実際のサイズ設定

	// ブロックの最大描画範囲
	static constexpr int DrawMaxXPlus = 100;
	static constexpr int DrawMaxXMinus = -100;
	static constexpr int DrawMaxZPlus = 100;
	static constexpr int DrawMaxZMinus = -100;
	static constexpr int DrawMaxYPlus = 30;
	static constexpr int DrawMaxYMinus = -70;

	// 内部計算用
	static constexpr int8_t s_EmptyBlick = 0;// ブロックIDがない=空であることをしめす
	static constexpr int ReferenceCell = 0;// 基準になるボクセルタイルリストID
	// テクスチャタイリングする際のUV分割
	static constexpr float TexTileU = 1.0f / 2.0f;
	static float TexTileV = 1.0f / 16.0f;

	// ボクセル描画、コリジョン生成クラス
	class VoxelControl {
		// 描画ポリゴンデータ
		class CellsData {
			// セル一つ一つに含まれる情報
			class CellBuffer {
				int8_t					m_ID{};// ID
				int8_t					m_FillInfo{};// 周りの遮蔽データのbitフラグ 各ビットが立っていると遮蔽されている
			public:
				// ブロックIDを1～で指定/取得
				void SetID(int8_t id) noexcept { this->m_ID = id; }
				const int8_t& GetID(void) const noexcept { return this->m_ID; }
				int8_t GetCellTexID(void) const noexcept { return ((GetID() - 1) * 3); }
				// ブロック間の遮蔽を設定/取得
				void ResetOcclusion(void) noexcept { this->m_FillInfo = 0; }
				void SetOcclusion(int id) noexcept { this->m_FillInfo |= (1 << id); }//特定の辺を遮蔽する
			public:
				bool IsEmpty(void) const noexcept { return GetID() == s_EmptyBlick; }//セルが置かれていない
				bool IsOcclusion(int id) const noexcept { return (this->m_FillInfo & (1 << id)) != 0; }//特定の辺の遮蔽を取得
				bool CanDraw(void) const noexcept { return !IsEmpty() && (this->m_FillInfo != 0b111111); }// なにかセルが置かれていて、尚且つ6辺の何れかが見える状態
			};
		private:
			std::vector<CellBuffer>	m_CellBuffer;
			int						m_LODRate = 0;
		public:
			// 算術補助系
			int						ScaleRate = static_cast<int>(std::pow(MulPer, GetLODRate()));
			int						All = 256 / this->ScaleRate;// XYZに描画するセルの1軸の数
			int						Half = this->All / 2;// Allの半分
			int						AllPow2 = this->All * this->All;// Allの2乗
			float					Scale = (CellScale * static_cast<float>(this->ScaleRate));// 描画座標に変換する際にかける数
		private:
			// 特定軸のループするインデックスを取得
			int			GetLoopIndex(int axis) const noexcept {
				return std::clamp(axis, 0, this->All - 1);
				//return (axis % this->All + this->All) % this->All;
			}
		public:
			const CellBuffer& GetCellBuf(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept { return this->m_CellBuffer[GetCellIndex(Xvoxel, Yvoxel, Zvoxel)]; }
			const CellBuffer& GetCellBuf(const Algorithm::Vector3Int& VoxelPoint) const noexcept { return GetCellBuf(VoxelPoint.x, VoxelPoint.y, VoxelPoint.z); }
			CellBuffer& SetCellBuf(int Xvoxel, int Yvoxel, int Zvoxel) noexcept { return this->m_CellBuffer[GetCellIndex(Xvoxel, Yvoxel, Zvoxel)]; }
			const int& GetLODRate(void) const noexcept { return this->m_LODRate; }
		public:
			bool			isReferenceCell(void) const noexcept { return GetLODRate() == ReferenceCell; }// 自身が基準ボクセルタイルリストかどうか
			size_t		GetCellIndex(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept { return static_cast<size_t>(GetLoopIndex(Xvoxel) * this->AllPow2 + Yvoxel * this->All + GetLoopIndex(Zvoxel)); }
			int8_t		isFill(int Xvoxel, int Yvoxel, int Zvoxel, int mul) const noexcept {
				mul /= this->ScaleRate;
				int FillCount = 0;
				int FillAll = 0;

				int xMaxmin = Xvoxel * mul + mul - 1;
				int yMaxmin = Yvoxel * mul + mul - 1;
				int zMaxmin = Zvoxel * mul + mul - 1;
				std::vector<int> IDCount;
				for (int xt = xMaxmin; xt < xMaxmin + mul; ++xt) {
					for (int yt = yMaxmin; yt < std::min(yMaxmin + mul, this->All); ++yt) {
						for (int zt = zMaxmin; zt < zMaxmin + mul; ++zt) {
							++FillAll;
							if (GetCellBuf(xt, yt, zt).IsEmpty()) { continue; }
							++FillCount;
							size_t cell = static_cast<size_t>(GetCellBuf(xt, yt, zt).GetID() + 1);
							if (cell > IDCount.size()) {
								IDCount.resize(cell);
							}
							++IDCount[cell - 1];
						}
					}
				}
				if ((FillAll != 0) && (static_cast<float>(FillCount) / static_cast<float>(FillAll) >= (1.0f / 4.0f))) {
					int max = -1;
					int8_t id = 1;
					for (auto& idc : IDCount) {
						if (max < idc) {
							max = idc;
							id = static_cast<int8_t>(&idc - &IDCount.front());
						}
					}
					return id;
				}
				else {
					return s_EmptyBlick;
				}
			}
			bool			isInside(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept {
				return 
					((5 <= Xvoxel) && (Xvoxel < this->All - 5)) &&
					((5 <= Yvoxel) && (Yvoxel < this->All - 5)) &&
					((5 <= Zvoxel) && (Zvoxel < this->All - 5))
					;
			}
			// ボクセル座標から描画座標の最小範囲に変換
			Util::VECTOR3D		GetWorldPos(int Xvoxel, int Yvoxel, int Zvoxel) const noexcept {
				return Util::VECTOR3D::vget(
					static_cast<float>(Xvoxel - this->Half) * this->Scale,
					static_cast<float>(Yvoxel - this->Half) * this->Scale,
					static_cast<float>(Zvoxel - this->Half) * this->Scale
				);
			}
			const Util::VECTOR3D		GetWorldPos(const Algorithm::Vector3Int& VoxelPoint) const noexcept {
				return GetWorldPos(VoxelPoint.x, VoxelPoint.y, VoxelPoint.z);
			}
			// ボクセル座標から描画座標のXYZ各範囲に変換
			const Util::VECTOR3D		GetWorldPosOffset(const Algorithm::Vector3Int& VoxelPoint, int Xofs, int Yofs, int Zofs) const noexcept {
				return GetWorldPos(VoxelPoint.x + Xofs, VoxelPoint.y + Yofs, VoxelPoint.z + Zofs);
			}
			// 指定の描画座標が入っている範囲のボクセル座標に変換
			const Algorithm::Vector3Int	GetVoxelPoint(const Util::VECTOR3D& pos) const noexcept {
				return Algorithm::Vector3Int(
					static_cast<int>(pos.x / this->Scale) + this->Half,
					static_cast<int>(pos.y / this->Scale) + this->Half,
					static_cast<int>(pos.z / this->Scale) + this->Half
				);
			}
			void				CalcOcclusion(int Xvoxel, int Yvoxel, int Zvoxel) noexcept {
				if (!isInside(Xvoxel, Yvoxel, Zvoxel)) { return; }
				if (GetCellBuf(Xvoxel, Yvoxel, Zvoxel).IsEmpty()) { return; }

				SetCellBuf(Xvoxel, Yvoxel, Zvoxel).ResetOcclusion();
				if ((Xvoxel == this->All - 1) ? true : !GetCellBuf(Xvoxel + 1, Yvoxel, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(0); }

				if ((Xvoxel == 0) ? true : !GetCellBuf(Xvoxel - 1, Yvoxel, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(1); }

				if (Yvoxel == this->All - 1) {
					SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(2);
				}
				else {
					if (GetCellBuf(Xvoxel, Yvoxel + 1, Zvoxel).GetCellTexID() >= 7) {
					}
					else if (!GetCellBuf(Xvoxel, Yvoxel + 1, Zvoxel).IsEmpty()) {
						SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(2);
					}
				}
				if ((Yvoxel == 0) ? true : !GetCellBuf(Xvoxel, Yvoxel - 1, Zvoxel).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(3); }
				if ((Zvoxel == this->All - 1) ? true : !GetCellBuf(Xvoxel, Yvoxel, Zvoxel + 1).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(4); }
				if ((Zvoxel == 0) ? true : !GetCellBuf(Xvoxel, Yvoxel, Zvoxel - 1).IsEmpty()) { SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetOcclusion(5); }
			}
		public:
			void				Init(int scale) noexcept {
				this->m_LODRate = scale;
				// 算術補助系の更新
				this->ScaleRate = static_cast<int>(std::pow(MulPer, GetLODRate()));
				this->All = 256 / this->ScaleRate;
				this->Half = this->All / 2;
				this->AllPow2 = this->All * this->All;
				this->Scale = (CellScale * static_cast<float>(this->ScaleRate));
				this->m_CellBuffer.resize(static_cast<size_t>(this->All * this->All * this->All));
			}
			void				Dispose(void) noexcept {
				this->m_CellBuffer.clear();
			}
		};
		// スレッド実行処理
		class ThreadJobs {
			std::thread						m_Job;// 作業スレッド
			bool							m_JobEnd{};// ジョブ終了通知
			bool							m_IsDoOnce{};// 1回だけ実行するフラグ
			bool							m_isDoing{ false };// 
			bool							m_IsDoEnd{};// ジョブ終了後作業終了通知
			char		padding[4]{};
			std::function<void()>			m_Doing{ nullptr };// ジョブ
			std::function<void()>			m_EndDoing{ nullptr };// ジョブ終了後作業
		public:
			ThreadJobs(void) noexcept {}
			ThreadJobs(const ThreadJobs&) = delete;
			ThreadJobs(ThreadJobs&&) = delete;
			ThreadJobs& operator=(const ThreadJobs&) = delete;
			ThreadJobs& operator=(ThreadJobs&&) = delete;
			virtual ~ThreadJobs(void) noexcept {
				Dispose();
			}
		public:
			// ジョブを強制開始させる
			void JobStart(void) noexcept {
				if (!this->m_isDoing) {
					this->m_isDoing = true;
					this->m_JobEnd = true;
					this->m_IsDoEnd = false;
				}
			}
		public:
			// ジョブの開始 行うジョブとジョブ完了時に行う内容を指定
			void Init(std::function<void()> Doing, std::function<void()> EndDoing, bool IsDoOnce) noexcept {
				this->m_Doing = Doing;
				this->m_EndDoing = EndDoing;
				this->m_IsDoOnce = IsDoOnce;
				JobStart();
			}
			// 毎フレーム更新する内容
			void Update(bool isActive) noexcept {
				if (isActive) {
					if (this->m_JobEnd) {
						this->m_JobEnd = false;
						if (this->m_IsDoEnd) {
							this->m_IsDoEnd = false;
							if (this->m_EndDoing) {
								this->m_EndDoing();
							}
						}
						// 1回だけの実行をしない場合
						if (!this->m_IsDoOnce) {
							this->m_isDoing = true;
						}
						if (this->m_isDoing) {
							if (this->m_Job.joinable()) {
								this->m_Job.detach();
							}
							std::thread tmp([&]() {
								if (this->m_Doing) {
									this->m_Doing();
								}
								this->m_JobEnd = true;
								this->m_isDoing = false;
								this->m_IsDoEnd = true;
								});
							this->m_Job.swap(tmp);
							// 強制待機
							// this->m_Job.join();
						}
					}
				}
				else {
					// 1回だけ実行する場合はオンにしといて次回実行に備える
					if (this->m_IsDoOnce) {
						this->m_isDoing = true;
					}
				}
			}
			// 終了
			void Dispose(void) noexcept {
				if (this->m_Job.joinable()) {
					this->m_Job.join();
					// this->m_Job.detach();
				}
				this->m_Doing = nullptr;
				this->m_EndDoing = nullptr;
			}
		};
		// Polygon32bitIndexed3Dで描画をするためのクラス
		// スレッドの結果反映中のデータを描画に使うと描画結果がおかしくなるのでバッファを作って対応する
		// が、コピーすると時間がかかるので参照先を入れ替えるようにして実現する。
		class VERTEX3DData {
			int												m_Now{ 0 };		// 今描画を担当している方
			char		padding[4]{};
			size_t											m_32Size{ 0 };	// ポリゴンの最大数
			std::array<std::vector<VERTEX3D>, 2>			m_vert32;		// 頂点リスト
			std::array<std::vector<uint32_t>, 2>			m_index32;		// 頂点IDリスト
			std::array<size_t, 2>							m_32Num{};		// 登録した頂点数
		public:
			// 元データをリセットせず表示状だけ破棄する
			void		Reset(void) noexcept {
				for (int loop = 0; loop < 2; ++loop) {
					this->m_32Num[static_cast<size_t>(loop)] = 0;
				}
			}
			const size_t& GetOutNum(void) const noexcept { return this->m_32Num[static_cast<size_t>(1 - this->m_Now)]; }
		public:
			// 初期化 あらかじめある程度確保しておく
			void		Init(size_t size) noexcept {
				for (int loop = 0; loop < 2; ++loop) {
					this->m_vert32[static_cast<size_t>(loop)].resize(size * 4);
					this->m_index32[static_cast<size_t>(loop)].resize(size * 6);
					this->m_32Num[static_cast<size_t>(loop)] = 0;
				}
				this->m_Now = 0;
				this->m_32Size = size;
			}
			// 終了
			void		Dispose(void) noexcept {
				Reset();
				// データも破棄する
				for (int loop = 0; loop < 2; ++loop) {
					this->m_vert32[static_cast<size_t>(loop)].clear();
					this->m_index32[static_cast<size_t>(loop)].clear();
				}
				this->m_32Size = 0;
			}
			// 登録数をリセット
			void		StartRegist(void) noexcept {
				this->m_32Num[static_cast<size_t>(this->m_Now)] = 0;
			}
			// 1枚確保
			size_t		RegistPlane(void) noexcept {
				++this->m_32Num[static_cast<size_t>(this->m_Now)];
				if (this->m_32Num[static_cast<size_t>(this->m_Now)] > this->m_32Size) {
					this->m_32Size = this->m_32Num[static_cast<size_t>(this->m_Now)];
					// 両方ともリサイズする
					for (int loop = 0; loop < 2; ++loop) {
						this->m_vert32[static_cast<size_t>(loop)].resize(this->m_32Size * 4);
						this->m_index32[static_cast<size_t>(loop)].resize(this->m_32Size * 6);
					}
				}
				size_t Now = this->m_32Num[static_cast<size_t>(this->m_Now)] - 1;
				uint32_t ZERO = static_cast<uint32_t>(Now * 4);
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 0] = ZERO;
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 1] = ZERO + 1;
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 2] = ZERO + 2;
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 3] = ZERO + 3;
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 4] = ZERO + 2;
				this->m_index32[static_cast<size_t>(this->m_Now)][Now * 6 + 5] = ZERO + 1;
				return Now;
			}
			// 確保済の1枚について1頂点の情報を書き込む
			void		SetParamToPlane(int index, const Util::VECTOR3D& pos, const Util::VECTOR3D& norm, int Uoffset, int Voffset) noexcept {
				VERTEX3D& Vert = this->m_vert32[static_cast<size_t>(this->m_Now)][static_cast<size_t>(index)];
				Vert.pos = pos.get();
				Vert.norm = norm.get();
				Vert.dif = GetColorU8(255, 255, 255, 255);
				Vert.spc = GetColorU8(92, 92, 92, 255);
				Vert.u = TexTileU * static_cast<float>(Uoffset);
				Vert.v = TexTileV * static_cast<float>(Voffset);
			}
			// 登録を完了し次の描画に備える
			void		EndRegist(void) noexcept {
				this->m_Now = 1 - this->m_Now;
			}
			// 描画する
			void		Draw(const Draw::GraphHandle& GrHandle) const noexcept {
				size_t OutputID = static_cast<size_t>(1 - this->m_Now);
				if (this->m_32Num[OutputID] == 0) { return; }
				DrawPolygon32bitIndexed3D(this->m_vert32[OutputID].data(), static_cast<int>(this->m_32Num[OutputID] * 4), this->m_index32[OutputID].data(), static_cast<int>(this->m_32Num[OutputID] * 6 / 3), GrHandle.get(), true);
			}
			void		DrawByShader(void) const noexcept {
				size_t OutputID = static_cast<size_t>(1 - this->m_Now);
				if (this->m_32Num[OutputID] == 0) { return; }
				DrawPolygon32bitIndexed3DToShader2(this->m_vert32[OutputID].data(), static_cast<int>(this->m_32Num[OutputID] * 4), this->m_index32[OutputID].data(), static_cast<int>(this->m_32Num[OutputID] * 6 / 3));
			}
		};
		// スレッドが変更し描画で参照するデータ
		class DrawThreadData {
			Util::VECTOR3D				m_DrawCenterPos{};	// 描画時の中心座標
			Util::VECTOR3D				m_CamVec{};			// 描画時の視点の向き
			VERTEX3DData				m_Vert32{};			// 描画先
			ThreadJobs					m_Jobs{};			// 生成用スレッド
		public:
			DrawThreadData(void) noexcept {}
			DrawThreadData(const DrawThreadData&) = delete;
			DrawThreadData(DrawThreadData&&) = delete;
			DrawThreadData& operator=(const DrawThreadData&) = delete;
			DrawThreadData& operator=(DrawThreadData&&) = delete;
			virtual ~DrawThreadData(void) noexcept {}
		public:
			// 描画情報の取得
			const Util::VECTOR3D& GetDrawCenterPos(void) const noexcept { return this->m_DrawCenterPos; }
			const Util::VECTOR3D& GetCamVec(void) const noexcept { return this->m_CamVec; }
			// ジョブのセットアップ
			void SetupJob(std::function<void()> Doing, std::function<void()> EndDoing, bool IsDoOnce) noexcept { this->m_Jobs.Init(Doing, EndDoing, IsDoOnce); }
			// 描画情報の書き込み
			void		SetDrawInfo(const Util::VECTOR3D& camPos, const Util::VECTOR3D& camVec) noexcept {
				this->m_DrawCenterPos = camPos;
				this->m_CamVec = camVec;
			}
			// 登録開始
			void		StartRegist(void) noexcept { this->m_Vert32.StartRegist(); }
			// 登録終了
			void		EndRegist(void) noexcept { this->m_Vert32.EndRegist(); }
			// 描画先のポインタ
			VERTEX3DData* GetVert32Ptr(void) noexcept { return &this->m_Vert32; }
#if defined(_DEBUG)
			const VERTEX3DData& GetVert32(void) const noexcept { return this->m_Vert32; }
#endif
		public:
			// 初期化
			void		Init(size_t size) noexcept { this->m_Vert32.Init(size); }
			// 毎フレーム更新する内容
			void		Update(bool value) noexcept {
				if (!value) {
					this->m_Vert32.Reset();// 生成データ自体を破棄
				}
				this->m_Jobs.Update(value);
			}
			// 描画
			void		Draw(const Draw::GraphHandle& GrHandle) const noexcept {
				auto prev = DxLib::GetDrawMode();
				DxLib::SetDrawMode(DX_DRAWMODE_ANISOTROPIC);
				this->m_Vert32.Draw(GrHandle);
				DxLib::SetDrawMode(prev);
			}
			void		DrawByShader(void) const noexcept {
				this->m_Vert32.DrawByShader();
			}
			// 終了
			void		Dispose(void) noexcept {
				this->m_Vert32.Dispose();
				this->m_Jobs.Dispose();
			}
		};
	private:
		std::array<CellsData, TotalCellLayer>						m_CellxN{};					// 各層の描画ポリゴンデータ
		int															m_MaxDrawLOD = 1;			// 実体の描画限界
		int															m_ShadowMaxDrawLOD = 1;		// 影の描画限界
		Draw::GraphHandle											m_tex{};					// 表示に使用するテクスチャ
		std::array<DrawThreadData, TotalCellLayer + TotalCellLayer>	m_DrawThreadDatas;			// 0~TotalCellLayer-1 = 表示ポリゴンスレッド用 / TotalCellLayer~ = 影スレッド用
		int															m_ThreadCounter = 0;
		Util::VECTOR3D												m_DrawCenterPos{};
		Util::VECTOR3D												m_CamVec{};
		Util::VECTOR3D												m_ShadowDrawCenterPos{};
		Util::VECTOR3D												m_ShadowCamVec{};
		char		padding[4]{};
	private:
		// 各方向に向いているポリゴンの追加
		void			AddPlaneXPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		void			AddPlaneXMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		void			AddPlaneYPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		void			AddPlaneYMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		void			AddPlaneZPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		void			AddPlaneZMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept;
		// XZ方向に走査してポリゴンをなるべく少ないポリゴン数で表示する
		void			AddPlanesXY(VERTEX3DData* pTarget, bool isDrawXPlus, bool isDrawYPlus, size_t id, const Algorithm::Vector3Int& VCenter, const Algorithm::Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept;
		void			AddPlanesZ(VERTEX3DData* pTarget, bool isDrawZPlus, size_t id, const Algorithm::Vector3Int& VCenter, const Algorithm::Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept;
		// 視界から見て映るものだけをテクスチャ関係込みで更新
		// UseCenterFrustumCulling=中心を起点に視錐台カリングをするかどうか
		// IsPersは中心を起点に透視射影をするか
		// useTextureはテクスチャを使用するかどうか(使用しない場合はUV計算などがオミットされます)
		void			AddCubes(size_t id, size_t threadID, bool UseCenterFrustumCulling, bool IsPers, bool useTexture) noexcept;
	public:
		// 基準ボクセルタイルリストを取得
		const CellsData& GetReferenceCells(void) const noexcept { return this->m_CellxN[ReferenceCell]; }
		// ブロックを直接指定
		void			SetBlick(int Xvoxel, int Yvoxel, int Zvoxel, int8_t ID, bool CalcOther = true) noexcept;
		// ボクセルとの線での当たり判定を取る
		int				CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal = nullptr) const noexcept;
		// 対人を想定した壁判定を行う
		bool			CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius, const std::vector<const Draw::MV1*>& addonColObj) const noexcept;
		// ボクセルデータをロードする
		void			LoadCellsFile(const char* Path) noexcept;
		// ボクセルデータをセーブする
		void			SaveCellsFile(const char* Path) const noexcept;
		// 所定の座標にボクセルデータをロードする
		void			LoadCellsClip(int Xvoxel, int Yvoxel, int Zvoxel, const char* Path) noexcept;
		// 所定の範囲のボクセルデータをセーブする
		void			SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax, const char* Path) const noexcept;
		// 描写距離を設定する
		void			SettingChange(int DrawLOD, int ShadowLOD) noexcept {
			//todo::LOD設定
			/*
			auto* pOption = Util::OptionParam::Instance();
			switch (pOption->GetParam(pOption->GetOptionType(Util::OptionType::ObjectLevel))->GetSelect()) {
			case 0:
				this->m_ShadowMaxDrawLOD = 0;// 表示
				this->m_MaxDrawLOD = 1;// 2段目まで表示
				break;
			case 1:
				this->m_ShadowMaxDrawLOD = 1;// 表示
				this->m_MaxDrawLOD = 2;// 2段目まで表示
				break;
			case 2:
				this->m_ShadowMaxDrawLOD = 1;// 表示
				this->m_MaxDrawLOD = 3;// 2段目まで表示
				break;
			default:
				break;
			}
			//*/
			//*
			this->m_ShadowMaxDrawLOD = ShadowLOD;// 表示
			// this->m_ShadowMaxDrawLOD = -1;// 非表示
			this->m_MaxDrawLOD = DrawLOD;// 2段目まで表示
			//*/
		}
		inline float	GetDrawFarMax(void) const noexcept { return static_cast<float>(std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus)) * CellScale; }

		void			SetDrawInfo(const Util::VECTOR3D& CenterPos, const Util::VECTOR3D& CamVec) noexcept {
			this->m_DrawCenterPos = CenterPos;
			this->m_CamVec = CamVec;
		}
		void			SetShadowDrawInfo(const Util::VECTOR3D& CenterPos, const Util::VECTOR3D& CamVec) noexcept {
			this->m_ShadowDrawCenterPos = CenterPos;
			this->m_ShadowCamVec = CamVec;
		}
	public:// コンストラクタ、デストラクタ
		VoxelControl(void) noexcept {}
		VoxelControl(const VoxelControl&) = delete;
		VoxelControl(VoxelControl&&) = delete;
		VoxelControl& operator=(const VoxelControl&) = delete;
		VoxelControl& operator=(VoxelControl&&) = delete;

		virtual ~VoxelControl(void) noexcept {}
	public:
		void			Load(const char* Path) noexcept;				// 事前読み込みが必要なデータのロード
		void			InitStart(void) noexcept;			// 各データの用意開始
		void			InitEnd(void) noexcept;				// 構成したデータのセットアップ
		void			Update(void) noexcept;				// 毎フレームよぶ更新処理
		void			DrawShadow(void) const noexcept;	// 影用描画
		void			Draw(void) const noexcept;			// 実描画
		void			DrawByShader(void) const noexcept;
		void			Dispose(void) noexcept;				// 各データの破棄
		void			Dispose_Load(void) noexcept;		// 事前読み込みしたデータの破棄
	};
}
