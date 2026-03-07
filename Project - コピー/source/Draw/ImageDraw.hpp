#pragma once
#define NOMINMAX
#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#pragma warning( push, 3 )
#include "DxLib.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#pragma warning( pop )

#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"

namespace Draw {
	class Camera3DInfo {
		Util::VECTOR3D	m_pos{};
		Util::VECTOR3D	m_vec{};
		Util::VECTOR3D	m_up{ Util::VECTOR3D::up() };	// カメラ
		float		m_fov{ DX_PI_F / 2 };		// カメラ
		float		m_near{ 0.1f };
		float		m_far{ 10.f };	// ニアファー
	public:
		const auto& GetCamPos(void)const noexcept { return this->m_pos; }
		const auto& GetCamVec(void)const noexcept { return this->m_vec; }
		const auto& GetCamUp(void)const noexcept { return this->m_up; }
		const auto& GetCamFov(void)const noexcept { return this->m_fov; }
		const auto& GetCamNear(void)const noexcept { return this->m_near; }
		const auto& GetCamFar(void)const noexcept { return this->m_far; }
	public:
		void			SetCamPos(const Util::VECTOR3D& cam_pos, const Util::VECTOR3D& cam_vec, const Util::VECTOR3D& cam_up) noexcept {
			this->m_pos = cam_pos;
			this->m_vec = cam_vec;
			this->m_up = cam_up;
		}
		void			SetNowCamPos(void) noexcept {
			SetCamPos(DxLib::GetCameraPosition(), DxLib::GetCameraTarget(), DxLib::GetCameraUpVector());
		}
		void			SetCamInfo(float cam_fov_, float cam_near_, float cam_far_) noexcept {
			this->m_fov = cam_fov_;
			this->m_near = cam_near_;
			this->m_far = cam_far_;
		}
		void FlipCamInfo(void) const noexcept {
			SetUpCamInfo(this->m_pos, this->m_vec, this->m_up, this->m_fov, this->m_near, this->m_far);
		}

		const Util::Matrix4x4 GetViewMatrix(void) const noexcept {
			DxLib::MATRIX mat_view;					// ビュー行列
			DxLib::VECTOR vec_from = this->m_pos.get();		// カメラの位置
			DxLib::VECTOR vec_lookat = this->m_vec.get();  // カメラの注視点
			DxLib::VECTOR vec_up = this->m_up.get();    // カメラの上方向
			CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
			return mat_view;
		}

		const Util::Matrix4x4 GetProjectionMatrix(void) const noexcept {
			DxLib::MATRIX mat_view;					// プロジェクション行列
			CreatePerspectiveFovMatrix(&mat_view, this->m_fov, this->m_near, this->m_far);
			return mat_view;
		}

		static void SetUpCamInfo(const Util::VECTOR3D& campos, const Util::VECTOR3D& camvec, const Util::VECTOR3D& camup, float fov, float near_, float far_) noexcept {
			DxLib::SetCameraNearFar(near_, far_);
			DxLib::SetupCamera_Perspective(fov);
			DxLib::SetCameraPositionAndTargetAndUpVec(campos.get(), camvec.get(), camup.get());
		}
	};
	// フォントハンドル
	class GraphHandle : public Util::DXHandle {
	public:
		GraphHandle(void) noexcept {}
		GraphHandle(const GraphHandle&) = delete;
		GraphHandle(GraphHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
		}
		GraphHandle& operator=(const GraphHandle&) = delete;
		GraphHandle& operator=(GraphHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}
		virtual ~GraphHandle(void) noexcept {}
	protected:
		void	Dispose_Sub(void) noexcept override {
			DxLib::DeleteGraph(Util::DXHandle::get());
		}
	public:
		void Load_Tex(std::basic_string_view<TCHAR> FileName) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MV1LoadTextureWithStrLen(FileName.data(), FileName.length()));
		}
		void Load(std::basic_string_view<TCHAR> FileName, bool NotUse3DFlag = false) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::LoadGraphWithStrLen(FileName.data(), FileName.length(), NotUse3DFlag));
		}
		void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int* HandleArray, bool NotUse3DFlag = false) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag));
		}
		void DerivationGraph(int x, int y, int xsize, int ysize, const GraphHandle& baseImage) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::DerivationGraph(x, y, xsize, ysize, baseImage.get()));
		}
		void CreateGraphFromBmp(const BITMAPINFO* RGBBmpInfo, const void* RGBBmpImage, const BITMAPINFO* AlphaBmpInfo = nullptr, const void* AlphaBmpImage = nullptr, bool TextureFlag = true, bool ReverseFlag = false) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::CreateGraphFromBmp(RGBBmpInfo, RGBBmpImage, AlphaBmpInfo, AlphaBmpImage, TextureFlag ? TRUE : FALSE, ReverseFlag ? TRUE : FALSE));
		}
		static void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int  XSize, int  YSize, std::vector<GraphHandle>* Handles, bool NotUse3DFlag = false) noexcept {
			int* HandleArray = new int[static_cast<size_t>(AllNum)];
			DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag);

			Handles->clear();
			for (size_t loop = 0; loop < static_cast<size_t>(AllNum); ++loop) {
				Handles->emplace_back();
				Handles->back().SetHandleDirect(HandleArray[loop]);
			}
			delete[] HandleArray;

			return;
		}
		void MV1GetTexture(int MHandle, int TexIndex)noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MV1GetTextureGraphHandle(MHandle, TexIndex));
		}
	public:
		void DrawGraph(int posx, int posy, bool trns) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::DrawGraph(posx, posy, Util::DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraph(int posx, int posy, float Exrate, float rad, bool trns) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraph(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), Util::DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraphF(float posx, float posy, float Exrate, float rad, bool trns) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraphF(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), Util::DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawRotaGraph3(int posx, int posy,
			int cx, int cy,
			float ExtRateX, float ExtRateY, float rad, bool trns) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::DrawRotaGraph3(posx, posy, cx, cy, static_cast<double>(ExtRateX), static_cast<double>(ExtRateY), static_cast<double>(rad), Util::DXHandle::get(), (trns ? TRUE : FALSE));
		}

		void DrawExtendGraph(int posx1, int posy1, int posx2, int posy2, bool trns) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::DrawExtendGraph(posx1, posy1, posx2, posy2, Util::DXHandle::get(), (trns ? TRUE : FALSE));
		}
		void FillGraph(int r, int g, int b) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::FillGraph(Util::DXHandle::get(), r, g, b);
		}

		template <typename... Args>
		void GraphFilter(int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) const noexcept {
			DxLib::GraphFilter(Util::DXHandle::get(), FilterType, args...);
		}

		template <typename... Args>
		void GraphFilterBlt(const GraphHandle& targetImage, int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) const noexcept {
			DxLib::GraphFilterBlt(targetImage.get(), Util::DXHandle::get(), FilterType, args...);
		}

		template <typename... Args>
		void GraphBlend(const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) const noexcept {
			DxLib::GraphBlend(Util::DXHandle::get(), BlendImage.get(), BlendRatio, BlendType, args...);
		}

		template <typename... Args>
		void GraphBlendBlt(const GraphHandle& BaseImage, const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) const noexcept {
			DxLib::GraphBlendBlt(BaseImage.get(), BlendImage.get(), Util::DXHandle::get(), BlendRatio, BlendType, args...);
		}


		// GetGraphSize
		void GetSize(int* xsize, int* ysize) const noexcept {
			if (!Util::DXHandle::IsActive()) { return; }
			DxLib::GetGraphSize(Util::DXHandle::get(), xsize, ysize);
		}
		// 
		void SetUseTextureToShader(int ID) const noexcept {
			DxLib::SetUseTextureToShader(ID, Util::DXHandle::get());
		}
		void SetRenderTargetToShader(int ID, int SurfaceIndex = 0, int MipLevel = 0) const noexcept {
			DxLib::SetRenderTargetToShader(ID, Util::DXHandle::get(), SurfaceIndex, MipLevel);
		}
	};

	class ScreenHandle :public GraphHandle {
	public:
		ScreenHandle(void) noexcept {}
		ScreenHandle(const ScreenHandle&) = delete;
		ScreenHandle(ScreenHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
		}
		ScreenHandle& operator=(const ScreenHandle&) = delete;
		ScreenHandle& operator=(ScreenHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}
		virtual ~ScreenHandle(void) noexcept {}
	public:
		void Make(int SizeX, int SizeY, bool trns = false) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, (trns ? TRUE : FALSE)));
		}
		void MakeDepth(int SizeX, int SizeY) noexcept {
			// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
			auto prevMip = DxLib::GetCreateDrawValidGraphChannelNum();
			auto prevFloatType = DxLib::GetDrawValidFloatTypeGraphCreateFlag();
			auto prevBit = DxLib::GetCreateGraphChannelBitDepth();
			DxLib::SetCreateDrawValidGraphChannelNum(2);
			DxLib::SetDrawValidFloatTypeGraphCreateFlag(TRUE);
			DxLib::SetCreateGraphChannelBitDepth(32);
			Util::DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, FALSE));
			DxLib::SetCreateDrawValidGraphChannelNum(prevMip);
			DxLib::SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
			DxLib::SetCreateGraphChannelBitDepth(prevBit);
		}
	public:
		void SetDraw_Screen(bool Clear = true) const noexcept {
			DxLib::SetDrawScreen(Util::DXHandle::get());
			if (Clear) {
				DxLib::ClearDrawScreen();
			}
		}
		// 
		static void SetDraw_Screen(int handle, bool Clear = true) noexcept {
			DxLib::SetDrawScreen(handle);
			if (Clear) {
				DxLib::ClearDrawScreen();
			}
		}
	};
	//
	class SoftImageHandle : public Util::DXHandle {
	public:
		SoftImageHandle(void) noexcept {}
		SoftImageHandle(const SoftImageHandle&) = delete;
		SoftImageHandle(SoftImageHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
		}
		SoftImageHandle& operator=(const SoftImageHandle&) = delete;
		SoftImageHandle& operator=(SoftImageHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}
		virtual ~SoftImageHandle(void) noexcept {}
	protected:
		void	Dispose_Sub(void) noexcept override {
			DxLib::DeleteSoftImage(Util::DXHandle::get());
		}
	public:
		void	Make(int xsize, int ysize) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MakeRGB8ColorSoftImage(xsize, ysize));
		}
		void	GetDrawScreen(int x1, int y1, int x2, int y2) noexcept {
			DxLib::GetDrawScreenSoftImage(x1, y1, x2, y2, Util::DXHandle::get());
		}
		void	GetPixel(int x, int y, int* r, int* g, int* b, int* a) noexcept {
			DxLib::GetPixelSoftImage(Util::DXHandle::get(), x, y, r, g, b, a);
		}

	};
	//
	class Graphhave {
		// カスタム項目
		std::string		m_FilePath;
		GraphHandle		m_Handle;
	public:
		Graphhave(std::string_view FilePath) noexcept;
		Graphhave(const Graphhave&) = delete;
		Graphhave(Graphhave&&) = delete;
		Graphhave& operator=(const Graphhave&) = delete;
		Graphhave& operator=(Graphhave&&) = delete;

		~Graphhave(void) noexcept {
			this->m_Handle.Dispose();
		}
	public:
		bool			Equal(std::string_view FilePath) const noexcept {
			return (this->m_FilePath == FilePath);
		}
	public:
		const GraphHandle* Get(void) const noexcept { return &this->m_Handle; }
	};
	// フォントプール
	class GraphPool : public Util::SingletonBase<GraphPool> {
	private:
		friend class Util::SingletonBase<GraphPool>;
	public:
	private:
		std::vector<std::unique_ptr<Graphhave>>	m_Pools;
	private:
		GraphPool(void) noexcept {}
		GraphPool(const GraphPool&) = delete;
		GraphPool(GraphPool&&) = delete;
		GraphPool& operator=(const GraphPool&) = delete;
		GraphPool& operator=(GraphPool&&) = delete;
	public:
		std::unique_ptr<Graphhave>& Get(std::string_view FilePath) noexcept {
			auto Find = std::find_if(this->m_Pools.begin(), this->m_Pools.end(), [&](const std::unique_ptr<Graphhave>& tgt) {return tgt->Equal(FilePath); });
			if (Find != this->m_Pools.end()) {
				return *Find;
			}
			this->m_Pools.emplace_back(std::make_unique<Graphhave>(FilePath));
			return this->m_Pools.back();
		}
	};
}
