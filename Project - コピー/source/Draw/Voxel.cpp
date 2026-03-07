#include	"Voxel.hpp"

#pragma warning( push, 3 )
#include <fstream>
#pragma warning( pop )

#include "../Util/Option.hpp"

namespace BG {
	// LODに準じて描画しない範囲
	static constexpr int DrawMinXPlus = DrawMaxXPlus / MulPer - 1;
	static constexpr int DrawMinXMinus = DrawMaxXMinus / MulPer;
	static constexpr int DrawMinZPlus = DrawMaxZPlus / MulPer - 1;
	static constexpr int DrawMinZMinus = DrawMaxZMinus / MulPer;
	static constexpr int DrawMinYPlus = DrawMaxYPlus / MulPer - 1;
	static constexpr int DrawMinYMinus = DrawMaxYMinus / MulPer;
	// プラスマイナスの3ビット式
	static constexpr int GetXYZPM(bool isPlusX, bool isPlusY, bool isPlusZ) noexcept {
		return (isPlusX ? (1 << 2) : 0) | (isPlusY ? (1 << 1) : 0) | (isPlusZ ? (1 << 0) : 0);
	}

	void		VoxelControl::AddPlaneXPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		if (cellx.GetCellBuf(Voxel1).GetCellTexID() >= 7) {
			return;
		}
		int Now = static_cast<int>(pTarget->RegistPlane());

		int zscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
			Xofs = Voxel1.z % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel2, 1, 0, 1), Util::VECTOR3D::vget(1.f, 0.f, 0.f), Xofs + zscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel1, 1, 0, 0), Util::VECTOR3D::vget(1.f, 0.f, 0.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel2, 1, 1, 1), Util::VECTOR3D::vget(1.f, 0.f, 0.f), Xofs + zscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel1, 1, 1, 0), Util::VECTOR3D::vget(1.f, 0.f, 0.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlaneXMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		if (cellx.GetCellBuf(Voxel1).GetCellTexID() >= 7) {
			return;
		}
		int Now = static_cast<int>(pTarget->RegistPlane());

		int zscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
			Xofs = Voxel1.z % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel2, 0, 1, 1), Util::VECTOR3D::vget(-1.f, 0.f, 0.f), Xofs + zscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel1, 0, 1, 0), Util::VECTOR3D::vget(-1.f, 0.f, 0.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel2, 0, 0, 1), Util::VECTOR3D::vget(-1.f, 0.f, 0.f), Xofs + zscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel1, 0, 0, 0), Util::VECTOR3D::vget(-1.f, 0.f, 0.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlaneYPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		if (cellx.GetCellBuf(Voxel1).GetCellTexID() >= 7) {
			return;
		}
		int Now = static_cast<int>(pTarget->RegistPlane());

		int zscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
			Xofs = Voxel1.z % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 0;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel2, 1, 1, 1), Util::VECTOR3D::vget(0.f, 1.f, 0.f), Xofs + zscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel1, 1, 1, 0), Util::VECTOR3D::vget(0.f, 1.f, 0.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel2, 0, 1, 1), Util::VECTOR3D::vget(0.f, 1.f, 0.f), Xofs + zscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel1, 0, 1, 0), Util::VECTOR3D::vget(0.f, 1.f, 0.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlaneYMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		int Now = static_cast<int>(pTarget->RegistPlane());

		int zscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			zscale = (Voxel2.z - Voxel1.z + 1) * cellx.ScaleRate;
			Xofs = Voxel1.z % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 2;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel2, 0, 0, 1), Util::VECTOR3D::vget(0.f, -1.f, 0.f), Xofs + zscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel1, 0, 0, 0), Util::VECTOR3D::vget(0.f, -1.f, 0.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel2, 1, 0, 1), Util::VECTOR3D::vget(0.f, -1.f, 0.f), Xofs + zscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel1, 1, 0, 0), Util::VECTOR3D::vget(0.f, -1.f, 0.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlaneZPlus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		if (cellx.GetCellBuf(Voxel1).GetCellTexID() >= 7) {
			return;
		}
		int Now = static_cast<int>(pTarget->RegistPlane());

		int xscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			xscale = (Voxel2.x - Voxel1.x + 1) * cellx.ScaleRate;
			Xofs = Voxel2.x % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel1, 0, 0, 1), Util::VECTOR3D::vget(0.f, 0.f, 1.f), Xofs + xscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel2, 1, 0, 1), Util::VECTOR3D::vget(0.f, 0.f, 1.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel1, 0, 1, 1), Util::VECTOR3D::vget(0.f, 0.f, 1.f), Xofs + xscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel2, 1, 1, 1), Util::VECTOR3D::vget(0.f, 0.f, 1.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlaneZMinus(VERTEX3DData* pTarget, size_t id, const Algorithm::Vector3Int& Voxel1, const Algorithm::Vector3Int& Voxel2, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		if (cellx.GetCellBuf(Voxel1).GetCellTexID() >= 7) {
			return;
		}
		int Now = static_cast<int>(pTarget->RegistPlane());

		int xscale{};
		int Xofs{};
		int Yofs{};
		if (useTexture) {
			xscale = (Voxel2.x - Voxel1.x + 1) * cellx.ScaleRate;
			Xofs = Voxel2.x % 2 == 0;
			Yofs = cellx.GetCellBuf(Voxel1).GetCellTexID() + 1;
		}
		pTarget->SetParamToPlane(Now * 4 + 0, cellx.GetWorldPosOffset(Voxel1, 0, 1, 0), Util::VECTOR3D::vget(0.f, 0.f, -1.f), Xofs + xscale, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 1, cellx.GetWorldPosOffset(Voxel2, 1, 1, 0), Util::VECTOR3D::vget(0.f, 0.f, -1.f), Xofs + 0, Yofs + 0);
		pTarget->SetParamToPlane(Now * 4 + 2, cellx.GetWorldPosOffset(Voxel1, 0, 0, 0), Util::VECTOR3D::vget(0.f, 0.f, -1.f), Xofs + xscale, Yofs + 1);
		pTarget->SetParamToPlane(Now * 4 + 3, cellx.GetWorldPosOffset(Voxel2, 1, 0, 0), Util::VECTOR3D::vget(0.f, 0.f, -1.f), Xofs + 0, Yofs + 1);
	}
	void		VoxelControl::AddPlanesXY(VERTEX3DData* pTarget,
		bool isDrawXPlus, bool isDrawYPlus,
		size_t id,
		const Algorithm::Vector3Int& VCenter, const Algorithm::Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		int Min = 0;
		int Max = 0;
		bool PrevPut = false;
		int8_t PutBlockID = s_EmptyBlick;
		int8_t CanDrawXYZ = 0b000000;
		Algorithm::Vector3Int Vofset = Vofs;
		for (Vofset.z = MaxminT; Vofset.z <= MaxmaxT; ++Vofset.z) {
			const auto& CellBuff = cellx.GetCellBuf(VCenter + Vofset);
			bool CheckInside = false;
			if (!cellx.isReferenceCell()) {
				bool CheckInsideX = (DrawMinXMinus < Vofset.x) && (Vofset.x < DrawMinXPlus);
				bool CheckInsideY = (DrawMinYMinus < Vofset.y) && (Vofset.y < DrawMinYPlus);
				bool CheckInsideZ = (DrawMinZMinus < Vofset.z) && (Vofset.z < DrawMinZPlus);
				CheckInside = CheckInsideX && CheckInsideY && CheckInsideZ;
			}
			bool CheckBlockID = useTexture && (PutBlockID != CellBuff.GetID());

			bool IsPutPoint = !(
				(Vofset.z == MaxmaxT)// 操作の端っこについた
				|| CheckInside// 描画してはならない地点に入った
				|| (PrevPut && CheckBlockID)// 置けるところだがテクスチャが変わった
				|| !CellBuff.CanDraw()// 描画してはいけないブロックの地点に入った
				|| (!cellx.isInside((VCenter + Vofset).x, (VCenter + Vofset).y, (VCenter + Vofset).z))//範囲外なので置けない
				);
			if (IsPutPoint) {
				if (!PrevPut) {
					// ブロックが置ける部分なので観測開始
					Min = Vofset.z;
					PutBlockID = CellBuff.GetID();
					CanDrawXYZ = 0b000000;
				}
				// 置ける地点中は全ブロックが隠れていないか確認
				if (!((CanDrawXYZ & (1 << 0)) != 0) && !CellBuff.IsOcclusion(0)) {
					CanDrawXYZ |= (1 << 0);
				}
				if (!((CanDrawXYZ & (1 << 1)) != 0) && !CellBuff.IsOcclusion(1)) {
					CanDrawXYZ |= (1 << 1);
				}
				if (!((CanDrawXYZ & (1 << 2)) != 0) && !CellBuff.IsOcclusion(2)) {
					CanDrawXYZ |= (1 << 2);
				}
				if (!((CanDrawXYZ & (1 << 3)) != 0) && !CellBuff.IsOcclusion(3)) {
					CanDrawXYZ |= (1 << 3);
				}
			}
			else {
				if (PrevPut) {
					Max = Vofset.z - 1;
					// 置けない部分なので今まで置けていた分をまとめてポリゴン化
					Algorithm::Vector3Int V1 = Vofset; V1.z = Min; V1 += VCenter;
					Algorithm::Vector3Int V2 = Vofset; V2.z = Max; V2 += VCenter;
					bool CheckFill = false;// 埋まっている判定であろうと絶対埋めないといけないフラグ
					if (!cellx.isReferenceCell()) {
						bool CheckFillX = (DrawMinXMinus <= Vofset.x) && (Vofset.x <= DrawMinXPlus);
						bool CheckFillY = (DrawMinYMinus <= Vofset.y) && (Vofset.y <= DrawMinYPlus);
						bool CheckFillZ = (Min <= DrawMinZPlus) && (DrawMinZMinus <= Max);
						CheckFill = CheckFillX && CheckFillY && CheckFillZ;
					}
					if (isDrawXPlus) {
						if (CheckFill || ((CanDrawXYZ & (1 << 0)) != 0)) {
							AddPlaneXPlus(pTarget, id, V1, V2, useTexture);
						}
					}
					else {
						if (CheckFill || ((CanDrawXYZ & (1 << 1)) != 0)) {
							AddPlaneXMinus(pTarget, id, V1, V2, useTexture);
						}
					}
					if (isDrawYPlus) {
						if (CheckFill || ((CanDrawXYZ & (1 << 2)) != 0)) {
							AddPlaneYPlus(pTarget, id, V1, V2, useTexture);
						}
					}
					else {
						if (CheckFill || ((CanDrawXYZ & (1 << 3)) != 0)) {
							AddPlaneYMinus(pTarget, id, V1, V2, useTexture);
						}
					}
					// テクスチャ変化の場合だけもう一回判定させるドン
					if (CheckBlockID) {
						--Vofset.z;
					}
				}
			}
			PrevPut = IsPutPoint;
		}
	}
	void		VoxelControl::AddPlanesZ(VERTEX3DData* pTarget,
		bool isDrawZPlus,
		size_t id,
		const Algorithm::Vector3Int& VCenter, const Algorithm::Vector3Int& Vofs, int MaxminT, int MaxmaxT, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		int Min = 0;
		int Max = 0;
		bool PrevPut = false;
		int8_t PutBlockID = s_EmptyBlick;
		int8_t CanDrawXYZ = 0b000000;
		Algorithm::Vector3Int Vofset = Vofs;
		for (Vofset.x = MaxminT; Vofset.x <= MaxmaxT; ++Vofset.x) {
			const auto& CellBuff = cellx.GetCellBuf(VCenter + Vofset);
			bool CheckInside = false;
			if (!cellx.isReferenceCell()) {
				bool CheckInsideX = (DrawMinXMinus < Vofset.x) && (Vofset.x < DrawMinXPlus);
				bool CheckInsideY = (DrawMinYMinus < Vofset.y) && (Vofset.y < DrawMinYPlus);
				bool CheckInsideZ = (DrawMinZMinus < Vofset.z) && (Vofset.z < DrawMinZPlus);
				CheckInside = CheckInsideX && CheckInsideY && CheckInsideZ;
			}
			bool CheckBlockID = useTexture && (PutBlockID != CellBuff.GetID());

			bool IsPutPoint = !(
				(Vofset.x == MaxmaxT)// 操作の端っこについたので置けない
				|| CheckInside// 描画してはならない地点に入ったので置けない
				|| (PrevPut && CheckBlockID)// 置けるところだがテクスチャが変わったので置けない
				|| !CellBuff.CanDraw()// 描画してはいけないブロックの地点に入ったので置けない
				|| (!cellx.isInside((VCenter + Vofset).x, (VCenter + Vofset).y, (VCenter + Vofset).z))//範囲外なので置けない
				);
			if (IsPutPoint) {
				if (!PrevPut) {
					// ブロックが置ける部分なので観測開始
					Min = Vofset.x;
					PutBlockID = CellBuff.GetID();
					CanDrawXYZ = 0b000000;
				}
				// 置ける地点中は全ブロックが隠れていないか確認
				if (!((CanDrawXYZ & (1 << 4)) != 0) && !CellBuff.IsOcclusion(4)) {
					CanDrawXYZ |= (1 << 4);
				}
				if (!((CanDrawXYZ & (1 << 5)) != 0) && !CellBuff.IsOcclusion(5)) {
					CanDrawXYZ |= (1 << 5);
				}
			}
			else {
				if (PrevPut) {
					// 置けない部分なので今まで置けていた分をまとめてポリゴン化
					Max = Vofset.x - 1;
					bool CheckFill = false;// 埋まっている判定であろうと絶対埋めないといけないフラグ
					if (!cellx.isReferenceCell()) {
						bool CheckFillX = (Min <= DrawMinXPlus) && (DrawMinXMinus <= Max);
						bool CheckFillY = (DrawMinYMinus <= Vofset.y) && (Vofset.y <= DrawMinYPlus);
						bool CheckFillZ = (DrawMinZMinus <= Vofset.z) && (Vofset.z <= DrawMinZPlus);
						CheckFill = CheckFillX && CheckFillY && CheckFillZ;
					}
					Algorithm::Vector3Int V1 = Vofset; V1.x = Min; V1 += VCenter;
					Algorithm::Vector3Int V2 = Vofset; V2.x = Max; V2 += VCenter;
					if (isDrawZPlus) {
						if (CheckFill || ((CanDrawXYZ & (1 << 4)) != 0)) {
							AddPlaneZPlus(pTarget, id, V1, V2, useTexture);
						}
					}
					else {
						if (CheckFill || ((CanDrawXYZ & (1 << 5)) != 0)) {
							AddPlaneZMinus(pTarget, id, V1, V2, useTexture);
						}
					}
					// テクスチャ変化の場合だけもう一回判定させるドン
					if (CheckBlockID) {
						--Vofset.x;
					}
				}
			}
			PrevPut = IsPutPoint;
		}
	}
	void		VoxelControl::AddCubes(size_t id, size_t threadID, bool UseCenterFrustumCulling, bool IsPers, bool useTexture) noexcept {
		CellsData& cellx = this->m_CellxN[id];
		DrawThreadData& Draws = this->m_DrawThreadDatas[threadID];
		Algorithm::Vector3Int VCenter = cellx.GetVoxelPoint(Draws.GetDrawCenterPos());// 描画の中心となる場所を指定しておく

		// 三次元の内積はaX*bX+aY*bY+aZ*bZなので各辺が確定した時に求めておく
		float CamDotMinX = 0.f;
		float CamDotMaxX = 0.f;
		float CamDotMinZ = 0.f;
		float CamDotMaxZ = 0.f;
		if (UseCenterFrustumCulling) {
			CamDotMinX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXMinus) + 0.5f);
			CamDotMaxX = Draws.GetCamVec().x * (static_cast<float>(DrawMaxXPlus) + 0.5f);
			CamDotMinZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZMinus) + 0.5f);
			CamDotMaxZ = Draws.GetCamVec().z * (static_cast<float>(DrawMaxZPlus) + 0.5f);
		}
		Algorithm::Vector3Int Vofs{};
		for (Vofs.y = DrawMaxYMinus; Vofs.y <= DrawMaxYPlus; ++Vofs.y) {
			float CamVecDotY = Draws.GetCamVec().y * (static_cast<float>(Vofs.y) + 0.5f);
			if (UseCenterFrustumCulling) {
				// 矩形がカメラの平面寄り裏にある場合(4点がすべて裏にある場合)はスキップ
				if (
					((CamDotMinX + CamVecDotY + CamDotMinZ) <= 0.0f) &&// Dot
					((CamDotMaxX + CamVecDotY + CamDotMaxZ) <= 0.0f) &&// Dot
					((CamDotMaxX + CamVecDotY + CamDotMinZ) <= 0.0f) &&// Dot
					((CamDotMinX + CamVecDotY + CamDotMaxZ) <= 0.0f)// Dot
					) {
					continue;
				}
			}
			// X
			for (Vofs.z = DrawMaxZMinus; Vofs.z <= DrawMaxZPlus; ++Vofs.z) {
				int xMaxminT = DrawMaxXMinus;
				int xMaxmaxT = DrawMaxXPlus;
				if (UseCenterFrustumCulling) {
					float CamVecDotZ = Draws.GetCamVec().z * (static_cast<float>(Vofs.z) + 0.5f);
					if (!Algorithm::CullingLine(&xMaxminT, &xMaxmaxT, CamDotMinX + CamVecDotY + CamVecDotZ, CamDotMaxX + CamVecDotY + CamVecDotZ)) {
						continue;
					}
				}
				AddPlanesZ(Draws.GetVert32Ptr(),
					IsPers ? (Vofs.z < 0) : (Draws.GetCamVec().z < 0.0f),
					id, VCenter, Vofs, xMaxminT, xMaxmaxT, useTexture);
			}
			// Z
			for (Vofs.x = DrawMaxXMinus; Vofs.x <= DrawMaxXPlus; ++Vofs.x) {
				int zMaxminT = DrawMaxZMinus;
				int zMaxmaxT = DrawMaxZPlus;
				if (UseCenterFrustumCulling) {
					float CamVecDotX = Draws.GetCamVec().x * (static_cast<float>(Vofs.x) + 0.5f);
					if (!Algorithm::CullingLine(&zMaxminT, &zMaxmaxT, CamVecDotX + CamVecDotY + CamDotMinZ, CamVecDotX + CamVecDotY + CamDotMaxZ)) {
						continue;
					}
				}
				AddPlanesXY(Draws.GetVert32Ptr(),
					IsPers ? (Vofs.x < 0) : (Draws.GetCamVec().x < 0.0f),
					IsPers ? (Vofs.y < 0) : (Draws.GetCamVec().y < 0.0f),
					id, VCenter, Vofs, zMaxminT, zMaxmaxT, useTexture);
			}
		}
	}

	int			VoxelControl::CheckLine(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, Util::VECTOR3D* Normal) const noexcept {
		int HitCount = 0;

		if (isnan<float>(StartPos.x) || isnan<float>(StartPos.y) || isnan<float>(StartPos.z)) {
			return HitCount;
		}
		if (isnan<float>((*EndPos).x) || isnan<float>((*EndPos).y) || isnan<float>((*EndPos).z)) {
			return HitCount;
		}
		float scale = 200.f * Scale3DRate;
		Util::VECTOR3D SP = StartPos; SP.y = 0.f;
		Util::VECTOR3D EP = (*EndPos); SP.y = 0.f;
		if ((SP.magnitude() > scale) || (EP.magnitude() > scale)) {
			return HitCount;
		}


		Algorithm::Vector3Int Start = GetReferenceCells().GetVoxelPoint(StartPos);
		Algorithm::Vector3Int End = GetReferenceCells().GetVoxelPoint(*EndPos);// *EndPos

		for (int Xofs = -1; Xofs <= 1; ++Xofs) {
			for (int Yofs = -1; Yofs <= 1; ++Yofs) {
				for (int Zofs = -1; Zofs <= 1; ++Zofs) {
					bool isHit = false;
					Algorithm::Bresenham3D(
						Start.x + Xofs, Start.y + Yofs, Start.z + Zofs,
						End.x + Xofs, End.y + Yofs, End.z + Zofs,
						[&](const Algorithm::Vector3Int& Voxel) {
							if (!GetReferenceCells().isInside(Voxel.x, Voxel.y, Voxel.z)) { return false; }
							if (!GetReferenceCells().GetCellBuf(Voxel).CanDraw()) { return false; }
							Util::VECTOR3D MinPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0, 0, 0);
							MinPos = MinPos + Util::VECTOR3D::vget(-0.1f, -0.1f, -0.1f) * Scale3DRate;
							Util::VECTOR3D MaxPos = GetReferenceCells().GetWorldPosOffset(Voxel, 1, 1, 1);
							MaxPos = MaxPos + Util::VECTOR3D::vget(0.1f, 0.1f, 0.1f) * Scale3DRate;
							Util::VECTOR3D tmpEndPos = *EndPos;
							Util::VECTOR3D tmpNormal;
							if (Algorithm::ColRayBox(StartPos, &tmpEndPos, MinPos, MaxPos, &tmpNormal)) {
								if ((tmpEndPos - StartPos).sqrMagnitude() < (*EndPos - StartPos).sqrMagnitude()) {
									*EndPos = tmpEndPos;
									if (Normal) {
										*Normal = tmpNormal;
									}
								}
								isHit = true;
								++HitCount;
							}
							return false;
						});
					if (isHit) {
						return HitCount;
					}
				}
			}
		}
		return HitCount;
	}
	bool		VoxelControl::CheckWall(const Util::VECTOR3D& StartPos, Util::VECTOR3D* EndPos, const Util::VECTOR3D& AddCapsuleMin, const Util::VECTOR3D& AddCapsuleMax, float Radius, const std::vector<const Draw::MV1*>& addonColObj) const noexcept {
		Util::VECTOR3D MoveVector = *EndPos - StartPos;
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		std::vector<MV1_COLL_RESULT_POLY> kabes;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		Algorithm::Vector3Int Start = GetReferenceCells().GetVoxelPoint(StartPos);
		Algorithm::Vector3Int End = GetReferenceCells().GetVoxelPoint(*EndPos);// *EndPos

		for (int Xofs = -3; Xofs <= 3; ++Xofs) {
			for (int Yofs = 3; Yofs <= 6; ++Yofs) {
				for (int Zofs = -3; Zofs <= 3; ++Zofs) {
					Algorithm::Bresenham3D(
						Start.x + Xofs, Start.y + Yofs, Start.z + Zofs,
						End.x + Xofs, End.y + Yofs, End.z + Zofs,
						[&](const Algorithm::Vector3Int& Voxel) {
							if (!GetReferenceCells().isInside(Voxel.x, Voxel.y, Voxel.z)) { return false; }
							const auto& CellBuff = GetReferenceCells().GetCellBuf(Voxel);
							if (!CellBuff.CanDraw()) { return false; }
							Util::VECTOR3D MinPos = GetReferenceCells().GetWorldPosOffset(Voxel, 0, 0, 0);
							Util::VECTOR3D MaxPos = GetReferenceCells().GetWorldPosOffset(Voxel, 1, 1, 1);

							MV1_COLL_RESULT_POLY tmp{};
							// Left
							if (!CellBuff.IsOcclusion(1)) {
								tmp.Position[0] = Util::VECTOR3D::vget(MinPos.x, MinPos.y, MinPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MinPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MinPos.x, MinPos.y, MaxPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(-1.f, 0.f, 0.f).get();
								kabes.emplace_back(tmp);
								tmp.Position[0] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MinPos.y, MaxPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MinPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(-1.f, 0.f, 0.f).get();
								kabes.emplace_back(tmp);
							}
							// Right
							if (!CellBuff.IsOcclusion(0)) {
								tmp.Position[0] = Util::VECTOR3D::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(1.f, 0.f, 0.f).get();
								kabes.emplace_back(tmp);
								tmp.Position[0] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MinPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(1.f, 0.f, 0.f).get();
								kabes.emplace_back(tmp);
							}
							// Back
							if (!CellBuff.IsOcclusion(5)) {
								tmp.Position[0] = Util::VECTOR3D::vget(MinPos.x, MinPos.y, MinPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MinPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MinPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(0.f, 0.f, -1.f).get();
								kabes.emplace_back(tmp);
								tmp.Position[0] = Util::VECTOR3D::vget(MaxPos.x, MaxPos.y, MinPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MinPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MinPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(0.f, 0.f, -1.f).get();
								kabes.emplace_back(tmp);
							}
							// Forward
							if (!CellBuff.IsOcclusion(4)) {
								tmp.Position[0] = Util::VECTOR3D::vget(MinPos.x, MinPos.y, MaxPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(0.f, 0.f, 1.f).get();
								kabes.emplace_back(tmp);
								tmp.Position[0] = Util::VECTOR3D::vget(MaxPos.x, MaxPos.y, MaxPos.z).get();
								tmp.Position[1] = Util::VECTOR3D::vget(MinPos.x, MaxPos.y, MaxPos.z).get();
								tmp.Position[2] = Util::VECTOR3D::vget(MaxPos.x, MinPos.y, MaxPos.z).get();
								tmp.Normal = Util::VECTOR3D::vget(0.f, 0.f, 1.f).get();
								kabes.emplace_back(tmp);
							}
							return true;
						});
				}
			}
		}

		// *
		for (auto& ad : addonColObj) {
			MV1_COLL_RESULT_POLY_DIM HitDim = MV1CollCheck_Sphere(ad->get(), -1, StartPos.get(), AddCapsuleMax.magnitude() + MoveVector.magnitude());
			// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
			for (int i = 0; i < HitDim.HitNum; ++i) {
				kabes.emplace_back(HitDim.Dim[i]);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
			}
			MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
		}
		// */

		bool HitFlag = false;
		// 壁ポリゴンとの当たり判定処理
		if (kabes.size() > 0) {
			HitFlag = false;
			for (auto& kabe : kabes) {
				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Capsule_Triangle((*EndPos + AddCapsuleMin).get(), (*EndPos + AddCapsuleMax).get(), Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
					HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					if (MoveVector.magnitude() > 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
						// 壁に当たったら壁に遮られない移動成分分だけ移動する
						*EndPos = Util::VECTOR3D::Cross(kabe.Normal, Util::VECTOR3D::Cross(MoveVector, kabe.Normal)) + StartPos;
						// EndPos->y(StartPos.y);
						bool isHit = false;
						for (auto& kabe2 : kabes) {
							if (HitCheck_Capsule_Triangle((*EndPos + AddCapsuleMin).get(), (*EndPos + AddCapsuleMax).get(), Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
								isHit = true;
								break;// 当たっていたらループから抜ける
							}
						}
						if (!isHit) {
							HitFlag = false;
							break;// どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
						}
					}
					else {
						break;
					}
				}
			}
			if (HitFlag) {		// 壁に当たっていたら壁から押し出す処理を行う
				for (int loop = 0; loop < 32; ++loop) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
					bool HitF = false;
					for (auto& kabe : kabes) {
						// プレイヤーと当たっているかを判定
						if (HitCheck_Capsule_Triangle((*EndPos + AddCapsuleMin).get(), (*EndPos + AddCapsuleMax).get(), Radius, kabe.Position[0], kabe.Position[1], kabe.Position[2])) {
							*EndPos = *EndPos + VScale(kabe.Normal, 0.015f * Scale3DRate);					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							bool isHit = false;
							for (auto& kabe2 : kabes) {
								// 当たっていたらループを抜ける
								if (HitCheck_Capsule_Triangle((*EndPos + AddCapsuleMin).get(), (*EndPos + AddCapsuleMax).get(), Radius, kabe2.Position[0], kabe2.Position[1], kabe2.Position[2])) {
									isHit = true;
									break;
								}
							}
							if (!isHit) {// 全てのポリゴンと当たっていなかったらここでループ終了
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {// 全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
						break;
					}
				}
			}
			kabes.clear();
		}
		return HitFlag;
	}

	void		VoxelControl::LoadCellsFile(const char* Path) noexcept {
		std::vector<int8_t>				SaveCellIDList{};
		std::ifstream fin{};
		fin.open(Path, std::ios::in | std::ios::binary);
		SaveCellIDList.resize(static_cast<size_t>(256 * 256 * 256));
		fin.read((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0])) * 256 * 256 * 256);
		fin.close();
		for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
			for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
				for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
					SetBlick(Xvoxel, Yvoxel, Zvoxel, SaveCellIDList[GetReferenceCells().GetCellIndex(Xvoxel, Yvoxel, Zvoxel)], false);
				}
			}
		}
	}
	void		VoxelControl::SaveCellsFile(const char* Path) const noexcept {
		std::vector<int8_t>				SaveCellIDList{};
		SaveCellIDList.resize(static_cast<size_t>(256 * 256 * 256));
		for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
			for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
				for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
					SaveCellIDList[GetReferenceCells().GetCellIndex(Xvoxel, Yvoxel, Zvoxel)] = GetReferenceCells().GetCellBuf(Xvoxel, Yvoxel, Zvoxel).GetID();
				}
			}
		}

		std::ofstream fout{};
		fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
		fout.write((char*)SaveCellIDList.data(), static_cast<size_t>(sizeof(SaveCellIDList[0])) * 256 * 256 * 256);
		fout.close(); // ファイルを閉じる
	}
	void		VoxelControl::LoadCellsClip(int Xvoxel, int Yvoxel, int Zvoxel, const char* Path) noexcept {
		std::vector<int8_t>				SaveCellIDList{};
		int XTotal = 1;
		int YTotal = 1;
		int ZTotal = 1;
		std::ifstream fin{};
		fin.open(Path, std::ios::in | std::ios::binary);
		fin.read((char*)&XTotal, sizeof(XTotal));
		fin.read((char*)&YTotal, sizeof(YTotal));
		fin.read((char*)&ZTotal, sizeof(ZTotal));
		SaveCellIDList.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
		fin.read((char*)SaveCellIDList.data(), static_cast<std::streamsize>(sizeof(SaveCellIDList[0])) * XTotal * YTotal * ZTotal);
		fin.close(); // ファイルを閉じる

		for (int Xofs = 0; Xofs < XTotal; ++Xofs) {
			for (int Yofs = 0; Yofs < YTotal; ++Yofs) {
				for (int Zofs = 0; Zofs < ZTotal; ++Zofs) {
					SetBlick(Xofs + Xvoxel, Yofs + Yvoxel, Zofs + Zvoxel, SaveCellIDList[static_cast<size_t>(Xofs * YTotal * ZTotal + Yofs * ZTotal + Zofs)], false);
				}
			}
		}
	}
	void		VoxelControl::SaveCellsClip(int XMin, int XMax, int YMin, int YMax, int ZMin, int ZMax, const char* Path) const noexcept {
		std::vector<int8_t>				SaveCellIDList{};
		int XTotal = (XMax - XMin + 1);
		int YTotal = (YMax - YMin + 1);
		int ZTotal = (ZMax - ZMin + 1);

		SaveCellIDList.resize(static_cast<size_t>(XTotal * YTotal * ZTotal));
		for (int xm = 0; xm < XTotal; ++xm) {
			for (int ym = 0; ym < YTotal; ++ym) {
				for (int zm = 0; zm < ZTotal; ++zm) {
					SaveCellIDList[static_cast<size_t>(xm * YTotal * ZTotal + ym * ZTotal + zm)] = GetReferenceCells().GetCellBuf(XMin + xm, YMin + ym, ZMin + zm).GetID();
				}
			}
		}

		std::ofstream fout{};
		fout.open(Path, std::ios::out | std::ios::binary | std::ios::trunc);
		fout.write((char*)&XTotal, sizeof(XTotal));
		fout.write((char*)&YTotal, sizeof(YTotal));
		fout.write((char*)&ZTotal, sizeof(ZTotal));
		fout.write((char*)SaveCellIDList.data(), static_cast<std::streamsize>(sizeof(SaveCellIDList[0]) * XTotal * YTotal * ZTotal));
		fout.close(); // ファイルを閉じる
	}

	void		VoxelControl::SetBlick(int Xvoxel, int Yvoxel, int Zvoxel, int8_t ID, bool CalcOther) noexcept {
		this->m_CellxN[ReferenceCell].SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetID(ID);
		if (CalcOther) {
			// 簡易版を更新
			for (int loop = 1; loop < TotalCellLayer; ++loop) {
				CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];

				int xm = Xvoxel / cellx.ScaleRate;
				int ym = Yvoxel / cellx.ScaleRate;
				int zm = Zvoxel / cellx.ScaleRate;

				cellx.SetCellBuf(xm, ym, zm).SetID(GetReferenceCells().isFill(xm, ym, zm, cellx.ScaleRate));
			}
			// 遮蔽検索
			for (auto& cellx : this->m_CellxN) {
				int xm = Xvoxel / cellx.ScaleRate;
				int ym = Yvoxel / cellx.ScaleRate;
				int zm = Zvoxel / cellx.ScaleRate;

				cellx.CalcOcclusion(xm + 1, ym, zm);
				cellx.CalcOcclusion(xm - 1, ym, zm);
				cellx.CalcOcclusion(xm, ym + 1, zm);
				cellx.CalcOcclusion(xm, ym - 1, zm);
				cellx.CalcOcclusion(xm, ym, zm + 1);
				cellx.CalcOcclusion(xm, ym, zm - 1);
			}
		}
	}

	void		VoxelControl::Load(const char* Path) noexcept {
#if defined(_DEBUG)
		{
			std::vector<WIN32_FIND_DATA> data_t;
			Util::GetFileNamesInDirectory("data/MapChip/*", &data_t);
			std::vector<std::string> dataStr;
			for (const auto& d : data_t) {
				std::string p = d.cFileName;
				if (p.find(".png") != std::string::npos) {
					dataStr.emplace_back("data/MapChip/" + p);
				}
			}
			int max = static_cast<int>(dataStr.size());
			int XSize = 128 * 2;
			int YSize = 128 * 3 * max;

			Draw::ScreenHandle Screen;
			Screen.Make(XSize, YSize, false);
			Screen.SetDraw_Screen(true);
			for (int loop = 0; loop < max; ++loop) {
				Draw::GraphHandle Graph; Graph.Load(dataStr[static_cast<size_t>(loop)]);
				Screen.SetDraw_Screen(false);
				Graph.DrawGraph(128 * 0, (128 * 3) * loop, false);
				Graph.DrawGraph(128 * 1, (128 * 3) * loop, false);
			}
			SaveDrawValidGraphToPNG(Screen.get(), 0, 0, XSize, YSize, Path);
		}
#endif
	this->m_tex.Load(Path);
	{
		int YSize = 128 * 3;
		this->m_tex.GetSize(nullptr, &YSize);
		TexTileV = 1.f / (static_cast<float>(YSize) / 128.f);
	}
	}
	void		VoxelControl::InitStart(void) noexcept {
		// 全階層の初期化
		for (int loop = 0; loop < TotalCellLayer; ++loop) {
			CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];
			cellx.Init(loop);
		}
		// 全て空にする
		for (int Xvoxel = 0; Xvoxel < GetReferenceCells().All; ++Xvoxel) {
			for (int Zvoxel = 0; Zvoxel < GetReferenceCells().All; ++Zvoxel) {
				for (int Yvoxel = 0; Yvoxel < GetReferenceCells().All; ++Yvoxel) {
					SetBlick(Xvoxel, Yvoxel, Zvoxel, s_EmptyBlick, false);
				}
			}
		}
		// 描画モードの指定
		SetUseZBufferFlag(TRUE);									// Zバッファーを使う
		SetUseZBuffer3D(TRUE);										// 3DのZバッファーを使う
		SetWriteZBuffer3D(TRUE);									// 3DのZバッファーに書き込む
		SetTextureAddressMode(DX_TEXADDRESS_WRAP);					// テクスチャのUVが超過したばあいに先頭に戻るようにする
		// 描画ポリゴンを生成するスレッドの初期化
		constexpr size_t size = (DrawMaxXPlus - DrawMaxXMinus) * (DrawMaxYPlus - DrawMaxYMinus) * (DrawMaxZPlus - DrawMaxZMinus) / 2 * 3 / 100;// 想定される描画数の割り出し
		for (auto& Vert : this->m_DrawThreadDatas) {
			Vert.Init(size);
		}
	}
	void		VoxelControl::InitEnd(void) noexcept {
		// 簡略版を制作
		for (int loop = 0; loop < TotalCellLayer; ++loop) {
			if (ReferenceCell == loop) { continue; }
			CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];
			for (int Xvoxel = 0; Xvoxel < cellx.All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < cellx.All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < cellx.All; ++Zvoxel) {
						cellx.SetCellBuf(Xvoxel, Yvoxel, Zvoxel).SetID(GetReferenceCells().isFill(Xvoxel, Yvoxel, Zvoxel, cellx.ScaleRate));
					}
				}
			}
		}
		// 遮蔽検索
		for (auto& cellx : this->m_CellxN) {
			for (int Xvoxel = 0; Xvoxel < cellx.All; ++Xvoxel) {
				for (int Yvoxel = 0; Yvoxel < cellx.All; ++Yvoxel) {
					for (int Zvoxel = 0; Zvoxel < cellx.All; ++Zvoxel) {
						cellx.CalcOcclusion(Xvoxel, Yvoxel, Zvoxel);
					}
				}
			}
		}
		// 描画ポリゴンを生成するスレッドを作成
		this->m_DrawThreadDatas[0].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[0].SetDrawInfo(this->m_DrawCenterPos, this->m_CamVec);
				this->m_DrawThreadDatas[0].StartRegist();
				AddCubes(0, 0, true, true, true);
			},
			[this]() {
				this->m_DrawThreadDatas[0].EndRegist();
			}, false);
		this->m_DrawThreadDatas[1].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[1].SetDrawInfo(this->m_DrawCenterPos, this->m_CamVec);
				this->m_DrawThreadDatas[1].StartRegist();
				AddCubes(1, 1, true, true, true);
			},
			[this]() {
				this->m_DrawThreadDatas[1].EndRegist();
			}, false);
		this->m_DrawThreadDatas[2].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[2].SetDrawInfo(this->m_DrawCenterPos, this->m_CamVec);
				this->m_DrawThreadDatas[2].StartRegist();
				AddCubes(2, 2, true, true, true);
			},
			[this]() {
				this->m_DrawThreadDatas[2].EndRegist();
			}, false);
		this->m_DrawThreadDatas[3].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[3].SetDrawInfo(this->m_DrawCenterPos, this->m_CamVec);
				this->m_DrawThreadDatas[3].StartRegist();
				AddCubes(3, 3, true, true, true);
			},
			[this]() {
				this->m_DrawThreadDatas[3].EndRegist();
			}, false);
		this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].SetDrawInfo(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].StartRegist();
				AddCubes(0, static_cast<size_t>(TotalCellLayer + 0), false, false, true);
			},
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].EndRegist();
			}, false);
		this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].SetDrawInfo(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].StartRegist();
				AddCubes(1, static_cast<size_t>(TotalCellLayer + 1), false, false, true);
			},
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 1)].EndRegist();
			}, false);
		this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].SetDrawInfo(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].StartRegist();
				AddCubes(2, static_cast<size_t>(TotalCellLayer + 2), false, false, true);
			},
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 0)].SetDrawInfo(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 2)].EndRegist();
			}, false);
		this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].SetupJob(
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].SetDrawInfo(this->m_ShadowDrawCenterPos, this->m_ShadowCamVec);
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].StartRegist();
				AddCubes(3, static_cast<size_t>(TotalCellLayer + 3), false, false, true);
			},
			[this]() {
				this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + 3)].EndRegist();
			}, false);
		this->m_ThreadCounter = 0;
		// 
		SettingChange(1/*2段目まで表示*/, 0/*1段目まで表示*/);

		MATERIALPARAM Param{};
		Param.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);						// ディフューズカラー
		Param.Ambient = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);						// アンビエントカラー
		Param.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// スペキュラカラー
		Param.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);						// エミッシブカラー
		Param.Power = 500.0f;													// スペキュラハイライトの鮮明度
		SetMaterialParam(Param);
	}
	void		VoxelControl::Update(void) noexcept {
		for (int loop = 0; loop < TotalCellLayer; ++loop) {
			if ((loop != 0) && (loop != this->m_ThreadCounter)) { continue; }
			CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];
			// ポリゴンアップデート
			this->m_DrawThreadDatas[static_cast<size_t>(loop)].Update(this->m_MaxDrawLOD >= cellx.GetLODRate());
			// 影ポリゴンアップデート
			this->m_DrawThreadDatas[static_cast<size_t>(TotalCellLayer + loop)].Update(this->m_ShadowMaxDrawLOD >= cellx.GetLODRate());
		}
		++this->m_ThreadCounter %= TotalCellLayer;

#if defined(_DEBUG)
		{
			for (int loop = 0; loop < TotalCellLayer; ++loop) {
				const VERTEX3DData& Vert = this->m_DrawThreadDatas[static_cast<size_t>(loop)].GetVert32();
				if (Vert.GetOutNum() == 0) { continue; }
				printfDx("Vertex[%d]\n", Vert.GetOutNum() * 4);
			}
		}
#endif
	}
	void		VoxelControl::DrawShadow(void) const noexcept {
		for (int threadID = TotalCellLayer; threadID < TotalCellLayer + TotalCellLayer; ++threadID) {
			this->m_DrawThreadDatas[static_cast<size_t>(threadID)].Draw(this->m_tex);
		}
	}
	void		VoxelControl::Draw(void) const noexcept {
		int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
		int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
		float Near = GetCameraNear() / static_cast<float>(MaxLimit);
		float Far = GetCameraFar() / static_cast<float>(MinLimit);

		for (int loop = 0; loop < TotalCellLayer; ++loop) {
			const CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];
			if (cellx.isReferenceCell() && !(cellx.Scale < Far)) { continue; }
			if (!cellx.isReferenceCell() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
			this->m_DrawThreadDatas[static_cast<size_t>(loop)].Draw(this->m_tex);
		}
	}
	void		VoxelControl::DrawByShader(void) const noexcept {
		constexpr int MinLimit = std::min(std::min(DrawMinXPlus, DrawMinZPlus), DrawMinYPlus) * 3 / 4;
		constexpr int MaxLimit = std::min(std::min(DrawMaxXPlus, DrawMaxZPlus), DrawMaxYPlus) * 5 / 4;
		float Near = GetCameraNear() / MaxLimit;
		float Far = GetCameraFar() / MinLimit;

		this->m_tex.SetUseTextureToShader(0);
		for (int loop = 0; loop < TotalCellLayer; ++loop) {
			const CellsData& cellx = this->m_CellxN[static_cast<size_t>(loop)];
			if (cellx.isReferenceCell() && !(cellx.Scale < Far)) { continue; }
			if (!cellx.isReferenceCell() && !(Near < cellx.Scale && cellx.Scale < Far)) { continue; }
			this->m_DrawThreadDatas[static_cast<size_t>(loop)].DrawByShader();
		}
		SetUseTextureToShader(0, InvalidID);
	}

	void		VoxelControl::Dispose(void) noexcept {
		for (auto& Vert : this->m_DrawThreadDatas) {
			Vert.Dispose();
		}
		for (auto& cellx : this->m_CellxN) {
			cellx.Dispose();
		}
	}
	void		VoxelControl::Dispose_Load(void) noexcept {
		this->m_tex.Dispose();
	}
}
