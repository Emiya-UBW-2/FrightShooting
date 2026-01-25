#pragma warning(disable:5259)
#include "PostPass.hpp"
#include "Camera.hpp"

const Draw::PostPassScreenBufferPool* Util::SingletonBase<Draw::PostPassScreenBufferPool>::m_Singleton = nullptr;
const Draw::PostPassEffect* Util::SingletonBase<Draw::PostPassEffect>::m_Singleton = nullptr;

namespace Draw {
	// 
	// --------------------------------------------------------------------------------------------------
	// ポストプロセスエフェクト
	// --------------------------------------------------------------------------------------------------
	// 継承クラス
	class PostPassBloom : public PostPassEffect::PostPassBase {
		static const int EXTEND = 4;
	public:
		PostPassBloom(void) noexcept {}
		PostPassBloom(const PostPassBloom&) = delete;
		PostPassBloom(PostPassBloom&&) = delete;
		PostPassBloom& operator=(const PostPassBloom&) = delete;
		PostPassBloom& operator=(PostPassBloom&&) = delete;

		virtual ~PostPassBloom(void) noexcept {}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::Bloom))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			int xsize = DrawerMngr->GetRenderDispWidth();
			int ysize = DrawerMngr->GetRenderDispHeight();

			int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
			int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;

			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();
			const Draw::ScreenHandle* pGaussScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();

			pScreenBuffer->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_TWO_COLOR, 250, ColorPalette::Black, 255, ColorPalette::Gray50, 255);
			pGaussScreen->GraphFilterBlt(*pScreenBuffer, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pGaussScreen->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 1000);
			TargetGraph->SetDraw_Screen(false);
			{
				auto Prev = DxLib::GetDrawMode();
				DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				pGaussScreen->DrawExtendGraph(0, 0, xsize, ysize, true);
				pGaussScreen->DrawExtendGraph(0, 0, xsize, ysize, true);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				DxLib::SetDrawMode(Prev);
			}
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
		}
	};
	class PostPassDoF : public PostPassEffect::PostPassBase {
	private:
		Shader2DController				m_Shader;			// シェーダー
	public:
		PostPassDoF(void) noexcept {}
		PostPassDoF(const PostPassDoF&) = delete;
		PostPassDoF(PostPassDoF&&) = delete;
		PostPassDoF& operator=(const PostPassDoF&) = delete;
		PostPassDoF& operator=(PostPassDoF&&) = delete;

		virtual ~PostPassDoF(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/PS_DoF.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::DoF))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			int xsize = DrawerMngr->GetRenderDispWidth();
			int ysize = DrawerMngr->GetRenderDispHeight();
			const Draw::ScreenHandle* pNearScreen = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();
			const Draw::ScreenHandle* pFarScreen = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();

			pNearScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_GAUSS, 8, 2000);
			pFarScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_GAUSS, 8, 20);
			TargetGraph->SetDraw_Screen();
			{
				pGbuffer->GetColorBuffer().SetUseTextureToShader(0);
				pNearScreen->SetUseTextureToShader(1);
				pFarScreen->SetUseTextureToShader(2);
				pGbuffer->GetDepthBuffer().SetUseTextureToShader(3);
				this->m_Shader.SetDispSize(xsize, ysize);
				this->m_Shader.SetParam(3,
					PostPassParts->GetDoFParam().m_near, PostPassParts->GetDoFParam().m_far,
					PostPassParts->GetDoFParam().m_near_Max, PostPassParts->GetDoFParam().m_far_Min);
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
				SetUseTextureToShader(3, InvalidID);
			}

			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
		}
	};
	class PostPassSSR : public PostPassEffect::PostPassBase {
	private:
		Shader2DController				m_Shader1;		// シェーダー
		Shader2DController				m_Shader2;		// シェーダー
		const GraphHandle*				m_SkyBox{ nullptr };
	public:
		PostPassSSR(void) noexcept {}
		PostPassSSR(const PostPassSSR&) = delete;
		PostPassSSR(PostPassSSR&&) = delete;
		PostPassSSR& operator=(const PostPassSSR&) = delete;
		PostPassSSR& operator=(PostPassSSR&&) = delete;

		virtual ~PostPassSSR(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader1.Init("CommonData/shader/PS_CubeMap.pso");
			this->m_Shader2.Init("CommonData/shader/PS_SSR.pso");
			this->m_SkyBox = GraphPool::Instance()->Get("data/Image/SkyCube.dds")->Get();
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader1.Dispose();
			this->m_Shader2.Dispose();
			this->m_SkyBox = nullptr;
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::Reflection))->GetSelect() > 0;
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* CameraParts = Camera::Camera3D::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();

			int ReflectionLevel = pOption->GetParam(pOption->GetOptionType(Util::OptionType::Reflection))->GetSelect();

			int EXTEND = 0;

			if (ReflectionLevel == 1) {
				EXTEND = 4;
			}
			else if (ReflectionLevel == 2) {
				EXTEND = 4;
			}
			else if (ReflectionLevel == 3) {
				EXTEND = 2;
			}
			if (EXTEND != 0) {
				int xsize = DrawerMngr->GetRenderDispWidth();
				int ysize = DrawerMngr->GetRenderDispHeight();

				int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
				int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;
				const Draw::ScreenHandle* pColorScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
				const Draw::ScreenHandle* pNormalScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
				const Draw::ScreenHandle* pDepthScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true, true)->PopBlankScreen();
				const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
				const Draw::ScreenHandle* pScreenBuffer2 = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();

				pColorScreen->GraphFilterBlt(pGbuffer->GetColorBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				pNormalScreen->GraphFilterBlt(pGbuffer->GetNormalBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				pDepthScreen->GraphFilterBlt(pGbuffer->GetDepthBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
				pScreenBuffer->SetDraw_Screen();
				{
					pColorScreen->SetUseTextureToShader(0);
					pNormalScreen->SetUseTextureToShader(1);
					pDepthScreen->SetUseTextureToShader(2);
					this->m_SkyBox->SetUseTextureToShader(3);
					if (ReflectionLevel == 1) {
						this->m_Shader1.SetDispSize(xsizeEx, ysizeEx);
						this->m_Shader1.SetParam(3, 70.f, Scale3DRate, std::tan(CameraParts->GetCameraForDraw().GetCamFov() / 2.f), 0.f);
						this->m_Shader1.SetCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
						this->m_Shader1.Draw();
					}
					else {
						this->m_Shader2.SetDispSize(xsizeEx, ysizeEx);
						this->m_Shader2.SetParam(3, 70.f, Scale3DRate, std::tan(CameraParts->GetCameraForDraw().GetCamFov() / 2.f), 0.f);
						this->m_Shader2.SetCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
						this->m_Shader2.Draw();
					}
					SetUseTextureToShader(0, InvalidID);
					SetUseTextureToShader(1, InvalidID);
					SetUseTextureToShader(2, InvalidID);
					SetUseTextureToShader(3, InvalidID);
				}
				// ぼかしを入れる
				pScreenBuffer->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 150);
				pScreenBuffer2->SetDraw_Screen();
				{
					pScreenBuffer->DrawExtendGraph(0, 0, xsize, ysize, true);
				}
				pScreenBuffer2->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 150);
				TargetGraph->SetDraw_Screen(false);
				{
					TargetGraph->DrawGraph(0, 0, true);
					pScreenBuffer2->DrawGraph(0, 0, true);
				}
				ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
				ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
				ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true, true);
				ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
				ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			}
		}
	};
	class PostPassSSAO : public PostPassEffect::PostPassBase {
		static const int EXTEND = 4;
	private:
		Shader2DController				m_Shader;		// シェーダー
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/PS_SSAO.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::SSAO))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* CameraParts = Camera::Camera3D::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();

			int xsize2 = DrawerMngr->GetRenderDispWidth() / 2;
			int ysize2 = DrawerMngr->GetRenderDispHeight() / 2;

			int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
			int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;

			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(xsize2, ysize2, true, false, 24)->PopBlankScreen();
			const Draw::ScreenHandle* pScreenBuffer2 = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true, false, 24)->PopBlankScreen();
			const Draw::ScreenHandle* pColorScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
			const Draw::ScreenHandle* pNormalScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
			const Draw::ScreenHandle* pDepthScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true, true)->PopBlankScreen();

			pColorScreen->GraphFilterBlt(pGbuffer->GetColorBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pNormalScreen->GraphFilterBlt(pGbuffer->GetNormalBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pDepthScreen->GraphFilterBlt(pGbuffer->GetDepthBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			// シェーダーを適用
			pScreenBuffer2->SetDraw_Screen(false);
			{
				pColorScreen->SetUseTextureToShader(0);
				pNormalScreen->SetUseTextureToShader(1);
				pDepthScreen->SetUseTextureToShader(2);
				this->m_Shader.SetDispSize(xsizeEx, ysizeEx);
				this->m_Shader.SetParam(3, 0.0f, Scale3DRate, std::tan(CameraParts->GetCameraForDraw().GetCamFov() / 2.f), 0.f);
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			// ぼかしを入れる
			pScreenBuffer->SetDraw_Screen();
			{
				auto Prev = DxLib::GetDrawMode();
				DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
				pScreenBuffer2->DrawExtendGraph(0, 0, xsize2, ysize2, true);
				DxLib::SetDrawMode(Prev);
			}
			pScreenBuffer->GraphFilter(DX_GRAPH_FILTER_BILATERAL_BLUR);
			// 
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
				pScreenBuffer->DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			ScreenBufferPool->ResetUseCount(xsize2, ysize2, true, false, 24);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true, false, 24);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true, true);
		}
	};
	class PostPassGodRay : public PostPassEffect::PostPassBase {
		static const int EXTEND = 2;
	private:
		Shader2DController				m_Shader;		// シェーダー
		Draw::ScreenHandle				m_Min;			// 描画スクリーン
		Draw::SoftImageHandle			m_SoftImage;
		int								m_GodRayRed{ InvalidID };
		float							m_GodRayTime{ 0.f };
		float							m_range{ 1.f };
		bool							m_IsUpdateSoftImage{ false };
		char		padding[3]{};
	public:
		PostPassGodRay(void) noexcept {}
		PostPassGodRay(const PostPassGodRay&) = delete;
		PostPassGodRay(PostPassGodRay&&) = delete;
		PostPassGodRay& operator=(const PostPassGodRay&) = delete;
		PostPassGodRay& operator=(PostPassGodRay&&) = delete;

		virtual ~PostPassGodRay(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/PS_GodRay.pso");
			this->m_Min.Make(1, 1, true);
			this->m_SoftImage.Make(1, 1);
			this->m_IsUpdateSoftImage = false;
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
			this->m_Min.Dispose();
			this->m_SoftImage.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* pOption = Util::OptionParam::Instance();
			return
				(pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0) &&
				pOption->GetParam(pOption->GetOptionType(Util::OptionType::GodRay))->IsActive() &&
				PostPassParts->GetGodRayParam().IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* CameraParts = Camera::Camera3D::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
			int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;

			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();
			const Draw::ScreenHandle* pScreenBuffer2 = ScreenBufferPool->GetBlankScreen(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true)->PopBlankScreen();

			pScreenBuffer->SetDraw_Screen();
			{
				pGbuffer->GetDepthBuffer().SetUseTextureToShader(0);
				PostPassParts->GetShadowDraw()->GetDepthScreen().SetUseTextureToShader(1);
				PostPassParts->GetShadowDraw()->GetDepthFarScreen().SetUseTextureToShader(2);
				{
					float Power = 1.f;
					switch (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect()) {
					case 1:
						Power = 50.f;
						break;
					case 2:
						Power = 100.f;
						break;
					case 3:
						Power = 150.f;
						break;
					default:
						break;
					}
					this->m_Shader.SetDispSize(xsizeEx, ysizeEx);
					this->m_Shader.SetParam(3, Power, 0.f, std::tan(CameraParts->GetCameraForDraw().GetCamFov() / 2.f), 0.f);
					this->m_Shader.SetCameraMatrix(4, PostPassParts->GetCamViewMat().inverse(), PostPassParts->GetCamProjectionMat().inverse());
					this->m_Shader.SetCameraMatrix(5, PostPassParts->GetShadowDraw()->GetCamViewMatrix(false), PostPassParts->GetShadowDraw()->GetCamProjectionMatrix(false));
					this->m_Shader.SetCameraMatrix(6, PostPassParts->GetShadowDraw()->GetCamViewMatrix(true), PostPassParts->GetShadowDraw()->GetCamProjectionMatrix(true));
					this->m_Shader.Draw();
				}
				SetUseTextureToShader(0, InvalidID);
				SetUseTextureToShader(1, InvalidID);
				SetUseTextureToShader(2, InvalidID);
			}
			//*
			this->m_GodRayTime += DrawerMngr->GetDeltaTime();
			if (this->m_GodRayTime > 0.5f) {
				this->m_GodRayTime -= 0.5f;
				if (!this->m_IsUpdateSoftImage) {
					this->m_Min.SetDraw_Screen();
					auto Prev = DxLib::GetDrawMode();
					DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
					pScreenBuffer->DrawExtendGraph(0, 0, 1, 1, true);
					DxLib::SetDrawMode(Prev);
				}
				else {
					this->m_Min.SetDraw_Screen(false);
					this->m_SoftImage.GetDrawScreen(0, 0, 1, 1);
					this->m_SoftImage.GetPixel(0, 0, &this->m_GodRayRed, nullptr, nullptr, nullptr);
				}
				this->m_IsUpdateSoftImage ^= 1;

			}
			PostPassParts->SetGodRayPerByPostPass(1.f - std::clamp(static_cast<float>(this->m_GodRayRed) / 128.f, 0.f, 1.f));
			//*/

			pScreenBuffer2->SetDraw_Screen();
			{
				pScreenBuffer->DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
			}
			pScreenBuffer2->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 300);
			TargetGraph->SetDraw_Screen();
			{
				pGbuffer->GetColorBuffer().DrawGraph(0, 0, true);
				SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(255.f * PostPassParts->GetGodRayParam().GetGodRayPerRet()));
				//SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(255.f));
				pScreenBuffer2->DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
			ScreenBufferPool->ResetUseCount(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
		}
	};
	class PostPassDistortion : public PostPassEffect::PostPassBase {
	public:
		PostPassDistortion(void) noexcept {}
		PostPassDistortion(const PostPassDistortion&) = delete;
		PostPassDistortion(PostPassDistortion&&) = delete;
		PostPassDistortion& operator=(const PostPassDistortion&) = delete;
		PostPassDistortion& operator=(PostPassDistortion&&) = delete;

		virtual ~PostPassDistortion(void) noexcept {}
	private:
		// 画面を歪ませながら描画する関数
		void		DrawCircleScreen(
			int CenterX,			// 円の中心座標X
			int CenterY,			// 円の中心座標Y
			float Radius,			// 円のサイズ
			float Absorption,		// 内側の円に引き込まれるドット数
			const Draw::ScreenHandle& ScreenHandle// 画面グラフィックハンドル
		) {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			const int CIRCLE_ANGLE_VERTEX_NUM = 16;			// 円周の頂点数
			const int CIRCLE_RADIUS_VERTEX_NUM = 8;			// 半径の頂点数

			float CenterDistance;
			float GraphCenterDistance;
			float AbsorptionDistance;
			float AbsorptionMoveX, AbsorptionMoveY;
			float Angle;
			float Sin, Cos;
			COLOR_U8 DiffuseColor;
			VERTEX2D* Vert;
			WORD* Ind;
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleCosTable{};
			std::array<float, CIRCLE_ANGLE_VERTEX_NUM> AngleSinTable{};
			std::array<float, CIRCLE_RADIUS_VERTEX_NUM> InCircleCosTable{};

			// スタックに積むには大きいので static 配列にしました
			static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
			static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

			// 最初に普通に描画
			ScreenHandle.DrawGraph(0, 0, false);

			// 描画カラーを作成しておく
			DiffuseColor = GetColorU8(255, 255, 255, 255);

			// 外周部分用の Sin, Cos テーブルを作成する
			Angle = 0.0f;
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop, Angle += DX_PI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM) {
				AngleSinTable[loop] = std::sin(Angle);
				AngleCosTable[loop] = std::cos(Angle);
			}

			// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
			Angle = 0.0f;
			for (size_t loop = 0; loop < CIRCLE_RADIUS_VERTEX_NUM; ++loop, Angle += (DX_PI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1)) {
				InCircleCosTable[loop] = std::cos(Angle);
			}

			// ポリゴン頂点インデックスの準備
			Ind = Index;
			for (int loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				for (WORD loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM - 1; ++loop2, Ind += 6) {
					Ind[0] = static_cast<WORD>(loop * CIRCLE_RADIUS_VERTEX_NUM + loop2);
					Ind[1] = static_cast<WORD>(Ind[0] + 1);
					if (loop == CIRCLE_ANGLE_VERTEX_NUM - 1) {
						Ind[2] = loop2;
						Ind[3] = static_cast<WORD>(loop2 + 1);
					}
					else {
						Ind[2] = static_cast<WORD>(Ind[0] + CIRCLE_RADIUS_VERTEX_NUM);
						Ind[3] = static_cast<WORD>(Ind[0] + 1 + CIRCLE_RADIUS_VERTEX_NUM);
					}
					Ind[4] = Ind[2];
					Ind[5] = Ind[1];
				}
			}

			// バイリニア補間描画にする
			DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);


			// 外側のドーナツ部分を描画

			// 中心に向かうにしたがって中心方向にテクスチャ座標をずらす
			Vert = Vertex;
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable[loop];
				Cos = AngleCosTable[loop];

				for (int loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM; ++loop2, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = Radius;

					// 中心に引き込まれる距離を算出
					AbsorptionDistance = Absorption * static_cast<float>(loop2) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// 中心に向かって移動する距離を算出
					AbsorptionMoveX = Cos * AbsorptionDistance;
					AbsorptionMoveY = Sin * AbsorptionDistance;

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Vert->pos.x + AbsorptionMoveX) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
					Vert->v = (Vert->pos.y + AbsorptionMoveY) / static_cast<float>(DrawerMngr->GetRenderDispHeight());

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 歪んだドーナツの描画
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);


			// 内側の盛り上がっているように見える部分を描画

			// Cosテーブルにしたがってテクスチャ座標をずらす
			Vert = Vertex;
			for (size_t loop = 0; loop < CIRCLE_ANGLE_VERTEX_NUM; ++loop) {
				// 使用する Sin, Cos の値をセット
				Sin = AngleSinTable[loop];
				Cos = AngleCosTable[loop];

				for (int loop2 = 0; loop2 < CIRCLE_RADIUS_VERTEX_NUM; ++loop2, ++Vert) {
					// 円の中心までの距離を算出
					CenterDistance = InCircleCosTable[static_cast<size_t>(loop2)] * Radius;

					// 画像座標視点での円の中心までの距離を算出
					GraphCenterDistance = static_cast<float>((CIRCLE_RADIUS_VERTEX_NUM - 1) - loop2) * (Absorption + Radius) / static_cast<float>(CIRCLE_RADIUS_VERTEX_NUM - 1);

					// スクリーン座標の決定
					Vert->pos.x = Cos * CenterDistance + static_cast<float>(CenterX);
					Vert->pos.y = Sin * CenterDistance + static_cast<float>(CenterY);
					Vert->pos.z = 0.0f;

					// テクスチャ座標のセット
					Vert->u = (Cos * GraphCenterDistance + static_cast<float>(CenterX)) / static_cast<float>(DrawerMngr->GetRenderDispWidth());
					Vert->v = (Sin * GraphCenterDistance + static_cast<float>(CenterY)) / static_cast<float>(DrawerMngr->GetRenderDispHeight());

					// その他のパラメータをセット
					Vert->rhw = 1.0f;
					Vert->dif = DiffuseColor;
				}
			}

			// 中心の盛り上がって見える部分を描画
			DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle.get(), FALSE);
		}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::Distortion))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* PostPassParts = PostPassEffect::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true)->PopBlankScreen();
			pScreenBuffer->SetDraw_Screen();
			{
				TargetGraph->DrawGraph(0, 0, true);
			}
			// TargetGraph->GraphFilterBlt(*pScreenBuffer, DX_GRAPH_FILTER_DOWN_SCALE, 1);
			TargetGraph->SetDraw_Screen();
			{
				// 画面を歪ませて描画
				DrawCircleScreen(
					DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2,
					static_cast<float>(DrawerMngr->GetRenderDispWidth() * 2 / 3), PostPassParts->GetDistortionPer(), *pScreenBuffer);
			}
			ScreenBufferPool->ResetUseCount(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
		}
	};
	class PostPassMotionBlur : public PostPassEffect::PostPassBase {
	private:
		class BlurScreen {
			static const size_t MAX = 5;
		private:
			std::array<Draw::ScreenHandle, MAX>	m_screen;
			size_t							m_current{ 0 };
			int								m_alpha{ 0 };
			int								m_screenWidth{ 0 }, m_screenHeight{ 0 };
			int								m_offsetX1{ 0 }, m_offsetX2{ 0 }, m_offsetY1{ 0 }, m_offsetY2{ 0 };
			char		padding[4]{};
			size_t							m_notBlendDraw{ 0 };
		public:
			BlurScreen(void) noexcept {}
			BlurScreen(const BlurScreen&) = delete;
			BlurScreen(BlurScreen&&) = delete;
			BlurScreen& operator=(const BlurScreen&) = delete;
			BlurScreen& operator=(BlurScreen&&) = delete;

			~BlurScreen(void) noexcept {}
		public:
			void		Init(int t_alpha, int t_offsetX1, int t_offsetY1, int t_offsetX2, int t_offsetY2) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				for (size_t loop = 0; loop < MAX; ++loop) {
					this->m_screen[loop].Make(DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight());
				}
				this->m_current = 0;
				this->m_alpha = t_alpha;
				this->m_offsetX1 = t_offsetX1;
				this->m_offsetX2 = t_offsetX2;
				this->m_offsetY1 = t_offsetY1;
				this->m_offsetY2 = t_offsetY2;

				this->m_notBlendDraw = 0;
			}
			void		Release(void) noexcept {
				for (size_t loop = 0; loop < MAX; ++loop) {
					this->m_screen[loop].Dispose();
				}
			}
		public:
			auto* PostRenderBlurScreen(std::function<void()> doing) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				size_t next = ((this->m_current != 0) ? this->m_current : MAX) - 1;
				this->m_screen[this->m_current].SetDraw_Screen();
				{
					doing();
					if (++this->m_notBlendDraw > MAX) {
						int Prev = DxLib::GetDrawMode();
						DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
						DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->m_alpha);
						this->m_screen[next].DrawExtendGraph(this->m_offsetX1, this->m_offsetY1, DrawerMngr->GetDispWidth() + this->m_offsetX2, DrawerMngr->GetDispHeight() + this->m_offsetY2, false);
						DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						DxLib::SetDrawMode(Prev);
					}
				}
				auto Cur = this->m_current;
				this->m_current = next;
				return &this->m_screen[Cur];
			}
		};
	private:
		BlurScreen				m_BlurScreen;
	public:
		PostPassMotionBlur(void) noexcept {}
		PostPassMotionBlur(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur(PostPassMotionBlur&&) = delete;
		PostPassMotionBlur& operator=(const PostPassMotionBlur&) = delete;
		PostPassMotionBlur& operator=(PostPassMotionBlur&&) = delete;

		virtual ~PostPassMotionBlur(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_BlurScreen.Init(96, -6, -6, 6, 6);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_BlurScreen.Release();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::MotionBlur))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			Draw::ScreenHandle* buf = this->m_BlurScreen.PostRenderBlurScreen([&]() {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				TargetGraph->DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), false);
				});
			TargetGraph->SetDraw_Screen(false);
			{
				auto* DrawerMngr = Draw::MainDraw::Instance();
				buf->DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), false);
			}
		}
	};
	class PostPassVignette : public PostPassEffect::PostPassBase {
	private:
		Draw::ScreenHandle	m_bkScreen;
	public:
		PostPassVignette(void) noexcept {}
		PostPassVignette(const PostPassVignette&) = delete;
		PostPassVignette(PostPassVignette&&) = delete;
		PostPassVignette& operator=(const PostPassVignette&) = delete;
		PostPassVignette& operator=(PostPassVignette&&) = delete;

		virtual ~PostPassVignette(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			{
				this->m_bkScreen.Make(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), false);
				this->m_bkScreen.SetDraw_Screen(true);
				this->m_bkScreen.FillGraph(255, 255, 255);
				{
					int p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 64.f));
						DxLib::DrawLine(0, y / p, DrawerMngr->GetRenderDispWidth(), y / p, DxLib::GetColor(c, c, c));
					}
					p = 1;
					for (int y = 0; y < 255; y += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - y) / 255.f, 1.5f) * 128.f));
						DxLib::DrawLine(0, DrawerMngr->GetRenderDispHeight() - y / p, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight() - y / p, DxLib::GetColor(c, c, c));
					}
				}
			}
			Draw::ScreenHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), false);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_bkScreen.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true)->PopBlankScreen();
			pScreenBuffer->GraphBlendBlt(*TargetGraph, this->m_bkScreen, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
				pScreenBuffer->DrawGraph(0, 0, true);
			}
			ScreenBufferPool->ResetUseCount(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
		}
	};
	class PostPassCornerBlur : public PostPassEffect::PostPassBase {
	private:
		static const int EXTEND = 4;
	private:
		Draw::ScreenHandle		m_bkScreen2;
	public:
		PostPassCornerBlur(void) noexcept {}
		PostPassCornerBlur(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur(PostPassCornerBlur&&) = delete;
		PostPassCornerBlur& operator=(const PostPassCornerBlur&) = delete;
		PostPassCornerBlur& operator=(PostPassCornerBlur&&) = delete;

		virtual ~PostPassCornerBlur(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			{
				this->m_bkScreen2.Make(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), false);
				this->m_bkScreen2.SetDraw_Screen(false);
				{
					this->m_bkScreen2.FillGraph(0, 0, 0);
					int xr = DrawerMngr->GetRenderDispWidth() * 60 / 100;
					int yr = DrawerMngr->GetRenderDispHeight() * 70 / 100;

					DrawOval(DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2, xr, yr, ColorPalette::White, TRUE);

					int p = 1;
					for (int r = 0; r < 255; r += p) {
						uint8_t c = static_cast<uint8_t>(255 - static_cast<int>(std::powf(static_cast<float>(255 - r) / 255.f, 1.5f) * 255.f));

						DrawOval(DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2, xr - r / p, yr - r / p, DxLib::GetColor(c, c, c), FALSE, 2);
					}
				}
			}
			Draw::ScreenHandle::SetDraw_Screen(static_cast<int>(DX_SCREEN_BACK), false);
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_bkScreen2.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();

			int xsize = DrawerMngr->GetRenderDispWidth();
			int ysize = DrawerMngr->GetRenderDispHeight();

			int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
			int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;

			const Draw::ScreenHandle* pScreenBuffer = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();
			const Draw::ScreenHandle* pAberrationScreen = ScreenBufferPool->GetBlankScreen(xsizeEx, ysizeEx, true)->PopBlankScreen();

			pAberrationScreen->GraphFilterBlt(*TargetGraph, DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			pAberrationScreen->GraphFilter(DX_GRAPH_FILTER_GAUSS, 8, 1000);
			pScreenBuffer->GraphBlendBlt(*TargetGraph, this->m_bkScreen2, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_BLEND_R);
			TargetGraph->SetDraw_Screen(false);
			{
				pAberrationScreen->DrawExtendGraph(0, 0, xsize, ysize, false);
				pScreenBuffer->DrawGraph(0, 0, true);
			}
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			ScreenBufferPool->ResetUseCount(xsizeEx, ysizeEx, true);
		}
	};
	class PostPassFXAA : public PostPassEffect::PostPassBase {
	private:
		Shader2DController				m_Shader;
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/PS_FXAA.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::AntiAliasing))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* DrawerMngr = Draw::MainDraw::Instance();
			TargetGraph->SetDraw_Screen();
			{
				pGbuffer->GetColorBuffer().SetUseTextureToShader(0);
				this->m_Shader.SetDispSize(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight());
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	class PostPassAberration : public PostPassEffect::PostPassBase {
	public:
		PostPassAberration(void) noexcept {}
		PostPassAberration(const PostPassAberration&) = delete;
		PostPassAberration(PostPassAberration&&) = delete;
		PostPassAberration& operator=(const PostPassAberration&) = delete;
		PostPassAberration& operator=(PostPassAberration&&) = delete;

		virtual ~PostPassAberration(void) noexcept {}
	protected:
		bool		IsActive_Sub(void) noexcept override {
			auto* pOption = Util::OptionParam::Instance();
			return pOption->GetParam(pOption->GetOptionType(Util::OptionType::ScreenEffect))->IsActive();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer*) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();

			int xsize = DrawerMngr->GetRenderDispWidth();
			int ysize = DrawerMngr->GetRenderDispHeight();

			const Draw::ScreenHandle* pScreenRed = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();
			const Draw::ScreenHandle* pScreenGreen = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();
			const Draw::ScreenHandle* pScreenBlue = ScreenBufferPool->GetBlankScreen(xsize, ysize, true)->PopBlankScreen();

			pScreenRed->SetDraw_Screen(false);
			pScreenRed->FillGraph(0, 0, 0);
			pScreenGreen->SetDraw_Screen(false);
			pScreenGreen->FillGraph(0, 0, 0);
			pScreenBlue->SetDraw_Screen(false);
			pScreenBlue->FillGraph(0, 0, 0);
			pScreenRed->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_BLEND_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			pScreenGreen->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_BLEND_G, DX_RGBA_SELECT_SRC_B, DX_RGBA_SELECT_SRC_A);
			pScreenBlue->GraphBlend(*TargetGraph, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
				DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_BLEND_B, DX_RGBA_SELECT_SRC_A);
			TargetGraph->SetDraw_Screen(false);
			{
				TargetGraph->FillGraph(0, 0, 0);
				SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
				pScreenRed->DrawRotaGraph(DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2, 1.f + 0.010f * PostPassParts->GetAberrationPower(), 0.f, true);
				pScreenGreen->DrawRotaGraph(DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2, 1.f + 0.005f * PostPassParts->GetAberrationPower(), 0.f, true);
				pScreenBlue->DrawRotaGraph(DrawerMngr->GetRenderDispWidth() / 2, DrawerMngr->GetRenderDispHeight() / 2, 1.f, 0.f, true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
			ScreenBufferPool->ResetUseCount(xsize, ysize, true);
		}
	};
	class PostPassBlackout : public PostPassEffect::PostPassBase {
	private:
		Shader2DController				m_Shader;			// シェーダー
	public:
		PostPassBlackout(void) noexcept {}
		PostPassBlackout(const PostPassBlackout&) = delete;
		PostPassBlackout(PostPassBlackout&&) = delete;
		PostPassBlackout& operator=(const PostPassBlackout&) = delete;
		PostPassBlackout& operator=(PostPassBlackout&&) = delete;

		virtual ~PostPassBlackout(void) noexcept {}
	protected:
		void		Load_Sub(void) noexcept override {
			this->m_Shader.Init("CommonData/shader/PS_BlackOut.pso");
		}
		void		Dispose_Sub(void) noexcept override {
			this->m_Shader.Dispose();
		}
		void		SetEffect_Sub(Draw::ScreenHandle* TargetGraph, PostPassEffect::Gbuffer* pGbuffer) noexcept override {
			auto* PostPassParts = PostPassEffect::Instance();
			auto* DrawerMngr = Draw::MainDraw::Instance();
			if (PostPassParts->GetBlackOutParamPer() == 0.f) { return; }
			// レンズ
			TargetGraph->SetDraw_Screen(false);
			{
				pGbuffer->GetColorBuffer().SetUseTextureToShader(0);	// 使用するテクスチャをセット
				this->m_Shader.SetDispSize(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight());
				this->m_Shader.SetParam(3, PostPassParts->GetBlackOutParamPer(), 0.f, 0.f, 0.f);
				this->m_Shader.Draw();
				SetUseTextureToShader(0, InvalidID);
			}
		}
	};
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	PostPassEffect::PostPassEffect(void) noexcept {
		//
		Init();
		// ポストプロセスの追加
		size_t now = 0;
		this->m_PostPass[now] = std::make_unique<PostPassBloom>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassDoF>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassSSR>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassSSAO>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassGodRay>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassMotionBlur>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassVignette>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassCornerBlur>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassFXAA>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassAberration>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassBlackout>(); ++now;
		this->m_PostPass[now] = std::make_unique<PostPassDistortion>(); ++now;
	}
};
