#pragma warning(disable:4464)
#pragma once
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"

#include "ImageDraw.hpp"

namespace Draw {
	class MV1 : public Util::DXHandle {
	public:
		class AnimControler {
			class MV1AnimeHandle : public Util::DXHandle {
				const MV1*	m_pBaseModel{};
			public:
				MV1AnimeHandle(void) noexcept {}
				MV1AnimeHandle(const MV1AnimeHandle& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
				}
				MV1AnimeHandle(MV1AnimeHandle&& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
					Util::DXHandle::SetHandleDirect(o.get());
				}
				MV1AnimeHandle& operator=(const MV1AnimeHandle& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
					return *this;
				}
				MV1AnimeHandle& operator=(MV1AnimeHandle&& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
					Util::DXHandle::SetHandleDirect(o.get());
					return *this;
				}
				virtual ~MV1AnimeHandle(void) noexcept {}
			protected:
				void			Dispose_Sub(void) noexcept override {
					MV1DetachAnim(this->m_pBaseModel->get(), Util::DXHandle::get());
					this->m_pBaseModel = nullptr;
				}
			public:
				void			Attach(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
					this->m_pBaseModel = model;
					if (model_haveanim && (this->m_pBaseModel->get() != model_haveanim->get())) {
						Util::DXHandle::SetHandleDirect(MV1AttachAnim(this->m_pBaseModel->get(), static_cast<int>(AnimIndex), model_haveanim->get()));
					}
					else {
						Util::DXHandle::SetHandleDirect(MV1AttachAnim(this->m_pBaseModel->get(), static_cast<int>(AnimIndex)));
					}
				}
				float			GetTotalTime(void) const noexcept { return MV1GetAttachAnimTotalTime(this->m_pBaseModel->get(), Util::DXHandle::get()); }
				void			SetTime(float Time) noexcept { MV1SetAttachAnimTime(this->m_pBaseModel->get(), Util::DXHandle::get(), Time); }
				void			SetBlendRate(float Per) noexcept { MV1SetAttachAnimBlendRate(this->m_pBaseModel->get(), Util::DXHandle::get(), Per); }
			};
		private:
			MV1AnimeHandle	m_handle;
			float			m_per{ 0.f };
			float			m_time{ 0.f };
			float			m_AllTime{ 0.f };

			float			m_per_prev{ -1.f };
			float			m_time_prev{ -1.f };
			char		padding[4]{};
		public:
			AnimControler(void) noexcept {}
			AnimControler(const AnimControler&) = delete;
			AnimControler(AnimControler&& o) noexcept {
				this->m_handle = o.m_handle;
				this->m_per = o.m_per;
				this->m_time = o.m_time;
				this->m_AllTime = o.m_AllTime;

				this->m_per_prev = o.m_per_prev;
				this->m_time_prev = o.m_time_prev;
			}
			AnimControler& operator=(const AnimControler&) = delete;
			AnimControler& operator=(AnimControler&& o) noexcept {
				this->m_handle = o.m_handle;
				this->m_per = o.m_per;
				this->m_time = o.m_time;
				this->m_AllTime = o.m_AllTime;

				this->m_per_prev = o.m_per_prev;
				this->m_time_prev = o.m_time_prev;
				return *this;
			}
			virtual ~AnimControler(void) noexcept {}
		public:
			const float&	GetPer(void) const noexcept { return this->m_per; }
			const float&	GetTime(void) const noexcept { return this->m_time; }
			float			GetTotalTime(void) const noexcept { return this->m_AllTime; }
			float			GetTimePer(void) const noexcept { return (GetTotalTime() > 0) ? GetTime() / GetTotalTime() : 1.f; }
		public:
			void			Reset(void) noexcept {
				this->m_per = 0.f;
				this->m_time = 0.f;
			}
			void			SetPer(float value) noexcept { this->m_per = value; }
			void			SetTime(float value) noexcept { this->m_time = value; }
		public:
			void			Set(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
				this->m_handle.Attach(model, AnimIndex, model_haveanim);
				Reset();
				FlipAnim();
				this->m_AllTime = this->m_handle.GetTotalTime();
			}
			void			Update(bool loop, float speed) noexcept {
				this->m_time += speed * 30.f * DeltaTime;
				if (loop) {
					if (speed >= 0.f) {
						if (this->m_time >= GetTotalTime()) {
							this->m_time = 0.f;
						}
					}
					else {
						if (this->m_time <= 0.f) {
							this->m_time = GetTotalTime();
						}
					}
				}
				else {
					this->m_time = std::clamp(this->m_time, 0.f, GetTotalTime());
				}
			}
			void			FlipAnim(void) noexcept {
				if (this->m_time_prev != GetTime()) {
					this->m_time_prev = GetTime();
					this->m_handle.SetTime(GetTime());
				}
				if (this->m_per_prev != GetPer()) {
					this->m_per_prev = GetPer();
					this->m_handle.SetBlendRate(GetPer());
				}
			}
		};
	private:
		std::vector<AnimControler>	m_AnimControler;
		size_t						m_AnimControlerSize{};
	public:
		MV1(void) noexcept {}
		MV1(const MV1&) = delete;
		MV1(MV1&&) = delete;
		MV1& operator=(const MV1&) = delete;
		MV1& operator=(MV1&&) = delete;
		virtual ~MV1(void) noexcept {}
	protected:
		void			Dispose_Sub(void) noexcept override {
			MV1DeleteModel(Util::DXHandle::get());
			this->m_AnimControler.clear();
			this->m_AnimControlerSize = 0;
		}
	public:
		// 読み込み
		static void		Load(std::basic_string_view<TCHAR> FileName, MV1* targetMV1, int mode = DX_LOADMODEL_PHYSICS_LOADCALC) noexcept {
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(mode);
			}
			targetMV1->SetHandleDirect(DxLib::MV1LoadModelWithStrLen(FileName.data(), FileName.length()));
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			targetMV1->m_AnimControler.clear();
			targetMV1->m_AnimControlerSize = 0;
			return;
		}
		static void		SetAnime(MV1* targetMV1, const MV1& animPaletteMV1) noexcept {
			// モデルハンドルが読めていない場合は待つ
			{
				while (ProcessMessage() == 0) {
					if ((CheckHandleASyncLoad(targetMV1->get()) == TRUE) && (CheckHandleASyncLoad(animPaletteMV1.get()) == TRUE)) {
						continue;
					}
					break;
				}
			}
			// エラー
			if (MV1GetAnimNum(animPaletteMV1.get()) < 0) {
				printfDx("error");
				WaitKey();
			}
			targetMV1->m_AnimControlerSize = static_cast<size_t>(MV1GetAnimNum(animPaletteMV1.get()));
			targetMV1->m_AnimControler.resize(targetMV1->m_AnimControlerSize);
			if (targetMV1->GetAnimNum() > 0) {
				for (size_t loop = 0; loop < targetMV1->GetAnimNum(); ++loop) {
					targetMV1->SetAnim(loop).Set(targetMV1, loop, &animPaletteMV1);
				}
			}
			return;
		}
		void			Duplicate(const MV1& o) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MV1DuplicateModel(o.get()));
		}
	public: // getter
		// 共通
		float			GetOpacityRate(void) const noexcept { return MV1GetOpacityRate(Util::DXHandle::get()); }
		Util::Matrix4x4	GetMatrix(void) const noexcept { return MV1GetMatrix(Util::DXHandle::get()); }
		// Frame
		Util::Matrix4x4	GetFrameLocalMatrix(int frame) const noexcept { return MV1GetFrameLocalMatrix(Util::DXHandle::get(), frame); }
		Util::Matrix4x4	GetFrameLocalWorldMatrix(int frame) const noexcept { return MV1GetFrameLocalWorldMatrix(Util::DXHandle::get(), frame); }
		Util::VECTOR3D	GetFramePosition(int frame) const noexcept { return MV1GetFramePosition(Util::DXHandle::get(), frame); }
		int				GetFrameNum(void) const noexcept { return MV1GetFrameNum(Util::DXHandle::get()); }
		int				GetFrameParent(int frame) const noexcept { return MV1GetFrameParent(Util::DXHandle::get(), frame); }
		int				GetFrameChild(int frame, int p2) const noexcept { return MV1GetFrameChild(Util::DXHandle::get(), frame, p2); }
		int				GetFrameChildNum(int frame) const noexcept { return MV1GetFrameChildNum(Util::DXHandle::get(), frame); }
		std::string		GetFrameName(int frame) const noexcept { return MV1GetFrameName(Util::DXHandle::get(), frame); }
		Util::Matrix4x4	GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameLocalWorldMatrix(GetFrameParent(frame)); }
		Util::Matrix4x4	GetChildFrameWorldMatrix(int frame, int ID) const noexcept { return GetFrameLocalWorldMatrix(GetFrameChild(frame, ID)); }
		// Material
		int				GetMaterialNum(void) const noexcept { return MV1GetMaterialNum(Util::DXHandle::get()); }
		std::string		GetMaterialName(int material) const noexcept { return MV1GetMaterialName(Util::DXHandle::get(), material); }
		// Mesh
		int				GetMeshNum(void) const noexcept { return MV1GetMeshNum(Util::DXHandle::get()); }
		Util::VECTOR3D	GetMeshMaxPosition(int mesh) const noexcept { return MV1GetMeshMaxPosition(Util::DXHandle::get(), mesh); }
		Util::VECTOR3D	GetMeshMinPosition(int mesh) const noexcept { return MV1GetMeshMinPosition(Util::DXHandle::get(), mesh); }
		bool			GetMeshSemiTransState(int mesh) const noexcept { return (MV1GetMeshSemiTransState(Util::DXHandle::get(), mesh) == TRUE); }
		// Shape
		int				SearchShape(const char* str) const noexcept { return MV1SearchShape(Util::DXHandle::get(), str); }
	public: // setter
		// 描画
		bool			DrawModel(void) const noexcept { return MV1DrawModel(Util::DXHandle::get()) == TRUE; }
		bool			DrawFrame(int frame) const noexcept { return MV1DrawFrame(Util::DXHandle::get(), frame) == TRUE; }
		bool			DrawMesh(int mesh) const noexcept { return MV1DrawMesh(Util::DXHandle::get(), mesh) == TRUE; }
		void			Draw(bool IsBoned) noexcept {
			for (int loop = 0, max = MV1GetTriangleListNum(this->get()); loop < max; ++loop) {
				switch (MV1GetTriangleListVertexType(this->get(), loop)) {
				case DX_MV1_VERTEX_TYPE_1FRAME:
				case DX_MV1_VERTEX_TYPE_NMAP_1FRAME:
					if (IsBoned) { continue; }
					// 剛体メッシュの場合の処理
					MV1DrawTriangleList(this->get(), loop);
					break;
				case DX_MV1_VERTEX_TYPE_4FRAME:
				case DX_MV1_VERTEX_TYPE_NMAP_4FRAME:
					if (!IsBoned) { continue; }
					// スキニングメッシュの場合の処理
					MV1DrawTriangleList(this->get(), loop);
					break;
				default:
					break;
				}
			}
		}
		// 共通
		bool			SetOpacityRate(float alpha) const noexcept { return MV1SetOpacityRate(Util::DXHandle::get(), alpha) == TRUE; }
		bool			SetMatrix(const Util::Matrix4x4& mat) const noexcept { return MV1SetMatrix(Util::DXHandle::get(), mat.get()) == TRUE; }
		// Frame
		bool			SetFrameLocalMatrix(int frame, Util::Matrix4x4 mat) const noexcept { return MV1SetFrameUserLocalMatrix(Util::DXHandle::get(), frame, mat.get()) == TRUE; }
		void			ResetFrameUserLocalMatrix(int frame) const noexcept { MV1ResetFrameUserLocalMatrix(Util::DXHandle::get(), frame); }
		// Material
		void			SetMaterialDrawAlphaTestAll(bool Enable, int mode, int param) const noexcept { MV1SetMaterialDrawAlphaTestAll(Util::DXHandle::get(), Enable ? TRUE : FALSE, mode, param); }
		void			SetMaterialTypeAll(int mode) const noexcept { MV1SetMaterialTypeAll(Util::DXHandle::get(), mode); }
		void			SetMaterialDifColor(int material, COLOR_F color) const noexcept { MV1SetMaterialDifColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialSpcColor(int material, COLOR_F color) const noexcept { MV1SetMaterialSpcColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialAmbColor(int material, COLOR_F color) const noexcept { MV1SetMaterialAmbColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialSpcPower(int material, float power) const noexcept { MV1SetMaterialSpcPower(Util::DXHandle::get(), material, power); }
		void			SetMaterialDrawAddColorAll(int r, int g, int b) const noexcept { MV1SetMaterialDrawAddColorAll(Util::DXHandle::get(), r, g, b); }
		void			SetMaterialOpacityRate(int material, float alpha) const noexcept { MV1SetMeshOpacityRate(Util::DXHandle::get(), material, alpha); }
		// Shape
		bool			SetShapeRate(int Shapeindex, float Per) const noexcept { return MV1SetShapeRate(Util::DXHandle::get(), Shapeindex, Per) == TRUE; }
		// アニメーション
		AnimControler&	SetAnim(size_t animeID) noexcept { return this->m_AnimControler[animeID]; }
		const AnimControler&	GetAnim(size_t animeID) const noexcept { return this->m_AnimControler[animeID]; }
		size_t			GetAnimNum(void) const noexcept { return this->m_AnimControlerSize; }
		void			FlipAnim(size_t animeID) noexcept { SetAnim(animeID).FlipAnim(); }
		void			FlipAnimAll(void) noexcept {
			for (auto& a : this->m_AnimControler) {
				a.FlipAnim();
			}
		}
		// 物理演算
		bool			PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(Util::DXHandle::get()) == TRUE; }
		bool			PhysicsCalculation(float TimeMS) const noexcept { return MV1PhysicsCalculation(Util::DXHandle::get(), TimeMS) == TRUE; }
		// 当たり判定
		bool			SetupCollInfo(int x = 32, int y = 8, int z = 32, int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1SetupCollInfo(Util::DXHandle::get(), frame, x, y, z, mesh) == 0; }
		bool			TerminateCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1TerminateCollInfo(Util::DXHandle::get(), frame, mesh) == 0; }
		bool			RefreshCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1RefreshCollInfo(Util::DXHandle::get(), frame, mesh) == 0; }
		MV1_COLL_RESULT_POLY		CollCheck_Line(const Util::VECTOR3D& start, const Util::VECTOR3D& end, int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1CollCheck_Line(Util::DXHandle::get(), frame, start.get(), end.get(), mesh); }
		MV1_COLL_RESULT_POLY_DIM	CollCheck_Sphere(const Util::VECTOR3D& start, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1CollCheck_Sphere(Util::DXHandle::get(), frame, start.get(), range, mesh); }
		MV1_COLL_RESULT_POLY_DIM	CollCheck_Capsule(const Util::VECTOR3D& start, const Util::VECTOR3D& end, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept { return MV1CollCheck_Capsule(Util::DXHandle::get(), frame, start.get(), end.get(), range, mesh); }
		// その他
		bool			SetTextureGraphHandle(int texindex, const Draw::GraphHandle& texHandle, bool trans) const noexcept { return MV1SetTextureGraphHandle(Util::DXHandle::get(), texindex, texHandle.get(), trans ? TRUE : FALSE) == TRUE; }
		void			SetDifColorScale(COLOR_F color) const noexcept { MV1SetDifColorScale(Util::DXHandle::get(), color); }
		void			SetAmbColorScale(COLOR_F color) const noexcept { MV1SetAmbColorScale(Util::DXHandle::get(), color); }
		void			SetSpcColorScale(COLOR_F color) const noexcept { MV1SetSpcColorScale(Util::DXHandle::get(), color); }
		void			SaveModelToMV1File(std::basic_string_view<TCHAR> FileName, int  SaveType = MV1_SAVETYPE_NORMAL, int AnimMHandle = InvalidID, int AnimNameCheck = TRUE, int Normal8BitFlag = 1, int Position16BitFlag = 1, int Weight8BitFlag = 0, int Anim16BitFlag = 1) const noexcept { MV1SaveModelToMV1FileWithStrLen(Util::DXHandle::get(), FileName.data(), FileName.length(), SaveType, AnimMHandle, AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag, Anim16BitFlag); }
		void			SetPrioritizePhysicsOverAnimFlag(bool flag) const noexcept { MV1SetPrioritizePhysicsOverAnimFlag(Util::DXHandle::get(), flag ? TRUE : FALSE); }
	};




	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// IK
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static void			IK_move_Arm(
		const Util::VECTOR3D& Localyvec,
		const Util::VECTOR3D& Localzvec,
		Draw::MV1* pObj,
		float XPer,
		int Arm, const Util::Matrix4x4& FrameBaseLocalMatArm,
		int Arm2, const Util::Matrix4x4& FrameBaseLocalMatArm2,
		int Wrist, const Util::Matrix4x4& FrameBaseLocalMatWrist,
		const Util::VECTOR3D& DirPos, const Util::VECTOR3D& Diryvec, const Util::VECTOR3D& Dirzvec) noexcept {

		pObj->ResetFrameUserLocalMatrix(Arm);
		pObj->ResetFrameUserLocalMatrix(Arm2);
		pObj->ResetFrameUserLocalMatrix(Wrist);
		auto matBase = Util::Matrix3x3::Get33DX(pObj->GetParentFrameWorldMatrix(Arm)).inverse();
		// 基準
		auto vec_a1 = Util::Matrix3x3::Vtrans((DirPos - pObj->GetFramePosition(Arm)).normalized(), matBase);// 基準
		auto vec_a1L1 = Util::VECTOR3D(Util::VECTOR3D::vget(XPer, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();// x=0とする
		float cos_t = Util::GetCosFormula((Util::VECTOR3D(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2)).magnitude(), (Util::VECTOR3D(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm)).magnitude(), (Util::VECTOR3D(pObj->GetFramePosition(Arm)) - DirPos).magnitude());
		auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
		// 上腕
		pObj->SetFrameLocalMatrix(Arm, Util::Matrix3x3::RotVec2(
			Util::Matrix3x3::Vtrans(Util::VECTOR3D(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm), matBase),
			vec_t
		).Get44DX() * FrameBaseLocalMatArm);
		// 下腕
		matBase = Util::Matrix3x3::Get33DX(pObj->GetParentFrameWorldMatrix(Arm2)).inverse();
		pObj->SetFrameLocalMatrix(Arm2, Util::Matrix3x3::RotVec2(
			Util::Matrix3x3::Vtrans(Util::VECTOR3D(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2), matBase),
			Util::Matrix3x3::Vtrans(DirPos - pObj->GetFramePosition(Arm2), matBase)
		).Get44DX() * FrameBaseLocalMatArm2);
		// 手
		matBase = Util::Matrix3x3::Get33DX(pObj->GetParentFrameWorldMatrix(Wrist)).inverse();
		{
			Util::Matrix3x3 mat1;
			mat1 = Util::Matrix3x3::RotVec2(Util::Matrix3x3::Vtrans(Util::Matrix3x3::Vtrans(Localzvec, Util::Matrix3x3::Get33DX(pObj->GetFrameLocalWorldMatrix(Wrist))), matBase), Util::Matrix3x3::Vtrans(Dirzvec, matBase));
			pObj->SetFrameLocalMatrix(Wrist, mat1.Get44DX() * FrameBaseLocalMatWrist);
			auto xvec = Util::Matrix3x3::Vtrans(Localyvec, Util::Matrix3x3::Get33DX(pObj->GetFrameLocalWorldMatrix(Wrist)));
			mat1 = Util::Matrix3x3::RotAxis(Localzvec, Util::VECTOR3D::Angle(xvec, Util::VECTOR3D::Cross(Diryvec, Dirzvec)) * ((Util::VECTOR3D::Dot(Diryvec, xvec) > 0.f) ? -1.f : 1.f)) * mat1;
			pObj->SetFrameLocalMatrix(Wrist, mat1.Get44DX() * FrameBaseLocalMatWrist);
		}
	}

	static void			IK_RightArm(Draw::MV1* pObj,
		int Arm, const Util::Matrix4x4& FrameBaseLocalMatArm,
		int Arm2, const Util::Matrix4x4& FrameBaseLocalMatArm2,
		int Wrist, const Util::Matrix4x4& FrameBaseLocalMatWrist,
		const Util::Matrix4x4& DirMat) noexcept {
		IK_move_Arm(
			Util::VECTOR3D::up(),
			Util::VECTOR3D::right(),
			pObj, -0.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirMat.pos(), DirMat.yvec() * -1.f, DirMat.zvec());
	}
	static void			IK_LeftArm(Draw::MV1* pObj,
		int Arm, const Util::Matrix4x4& FrameBaseLocalMatArm,
		int Arm2, const Util::Matrix4x4& FrameBaseLocalMatArm2,
		int Wrist, const Util::Matrix4x4& FrameBaseLocalMatWrist,
		const Util::Matrix4x4& DirMat) noexcept {

		IK_move_Arm(
			Util::VECTOR3D::up(),
			Util::VECTOR3D::left(),
			pObj, 0.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirMat.pos(), DirMat.yvec(), DirMat.zvec());
	}
};
