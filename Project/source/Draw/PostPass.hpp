#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#include <array>
#include <functional>
#include <memory>
#pragma warning( pop )
#include "ImageDraw.hpp"

#include "../Util/Enum.hpp"

#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Option.hpp"

#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"


namespace Draw {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// シェーダーを使用する際の補助クラス
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class Shader2DController {
	public:
		// 2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
		class ScreenVertex {
			DxLib::VERTEX2DSHADER Screen_vertex[6] = {};
		public:
			// 頂点データの取得
			const auto* GetScreenVertex(void) const noexcept { return Screen_vertex; }
			// 頂点データの準備
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = DxLib::VGet(static_cast<float>(xp1), static_cast<float>(yp1), 0.0f);
				Screen_vertex[1].pos = DxLib::VGet(static_cast<float>(xp2), static_cast<float>(yp1), 0.0f);
				Screen_vertex[2].pos = DxLib::VGet(static_cast<float>(xp1), static_cast<float>(yp2), 0.0f);
				Screen_vertex[3].pos = DxLib::VGet(static_cast<float>(xp2), static_cast<float>(yp2), 0.0f);
				Screen_vertex[0].dif = DxLib::GetColorU8(255, 255, 255, 255);
				Screen_vertex[1].dif = DxLib::GetColorU8(255, 255, 255, 255);
				Screen_vertex[2].dif = DxLib::GetColorU8(255, 255, 255, 255);
				Screen_vertex[3].dif = DxLib::GetColorU8(255, 255, 255, 255);
				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;

				Screen_vertex[0].rhw = 1.0f;
				Screen_vertex[1].rhw = 1.0f;
				Screen_vertex[2].rhw = 1.0f;
				Screen_vertex[3].rhw = 1.0f;

				Screen_vertex[0].spc = DxLib::GetColorU8(0, 0, 0, 0);
				Screen_vertex[1].spc = DxLib::GetColorU8(0, 0, 0, 0);
				Screen_vertex[2].spc = DxLib::GetColorU8(0, 0, 0, 0);
				Screen_vertex[3].spc = DxLib::GetColorU8(0, 0, 0, 0);

				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;

				Screen_vertex[0].su = 0.0f; Screen_vertex[0].sv = 0.0f;
				Screen_vertex[1].su = 1.0f; Screen_vertex[1].sv = 0.0f;
				Screen_vertex[2].su = 0.0f; Screen_vertex[3].sv = 1.0f;
				Screen_vertex[3].su = 1.0f; Screen_vertex[2].sv = 1.0f;

				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};
		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			DxLib::MATRIX ViewMatrix;
			DxLib::MATRIX ProjectionMatrix;
		};
	private:
		// シェーダーハンドル
		int					m_Shaderhandle{ InvalidID };
		// シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 3>	m_LightCameraMatrixHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		int					m_ShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_Shadercbhandle{};
		ScreenVertex		m_ScreenVertex;					// 頂点データ
	public:
		Shader2DController(void) noexcept {
			// シェーダーハンドル
			this->m_Shaderhandle = InvalidID;
			// シェーダーに渡す追加パラメーターを配するハンドル
			this->m_ShaderSendDispSizeHandle = InvalidID;
			for (auto& h : this->m_Shadercbhandle) {
				h = InvalidID;
			}
		}
		~Shader2DController(void) noexcept {
			Dispose();
		}
	public:
		// 初期化
		void			Init(const char* Shader) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ピクセルシェーダ―周り
			this->m_ShaderSendDispSizeHandle = DxLib::CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : this->m_Shadercbhandle) {
				h = DxLib::CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_Shaderhandle = DxLib::LoadPixelShader(Shader);
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : this->m_LightCameraMatrixHandle) {
				h = DxLib::CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
		}
		// 後始末
		void			Dispose(void) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ピクセルシェーダ―周り
			DxLib::DeleteShaderConstantBuffer(this->m_ShaderSendDispSizeHandle);
			for (auto& h : this->m_Shadercbhandle) {
				DxLib::DeleteShaderConstantBuffer(h);
			}
			DxLib::DeleteShader(this->m_Shaderhandle);
		}
	public:
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			DxLib::UpdateShaderConstantBuffer(BufferHandle);
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetDispSize(int dispx, int dispy) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_ShaderSendDispSizeHandle == -1) { return; }
			auto& BufferHandle = this->m_ShaderSendDispSizeHandle;
			DxLib::FLOAT2* dispsize = (DxLib::FLOAT2*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			DxLib::UpdateShaderConstantBuffer(BufferHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
			this->m_ScreenVertex.SetScreenVertex(dispx, dispy);
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_Shadercbhandle[static_cast<size_t>(Slot - 3)];
			DxLib::FLOAT4* f4 = (DxLib::FLOAT4*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			DxLib::UpdateShaderConstantBuffer(BufferHandle);											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		// 3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw(void) const noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			DxLib::SetUsePixelShader(this->m_Shaderhandle);											// 使用するピクセルシェーダーをセット
			DxLib::MV1SetUseOrigShader(TRUE);
			DxLib::DrawPolygon2DToShader(this->m_ScreenVertex.GetScreenVertex(), 2);
			DxLib::MV1SetUseOrigShader(FALSE);
			DxLib::SetUsePixelShader(InvalidID);
		}
	};
	class ShaderController {
	public:
		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			DxLib::MATRIX ViewMatrix;
			DxLib::MATRIX ProjectionMatrix;
		};
		// DXLIBから引っ張ってきたシェーダー用の定義
		typedef float DX_D3D11_SHADER_FLOAT4[4];
		//
		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4]{};											// ビュー　→　プロジェクション行列
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3]{};												// ワールド　→　ビュー行列
		};
	private:
		// シェーダーハンドル
		int					m_VertexShaderhandle{ InvalidID };
		int					m_GeometryShaderhandle{ InvalidID };
		int					m_PixelShaderhandle{ InvalidID };
		// シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 3>	m_LightCameraMatrixHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		std::array<int, 4>	m_VertexShadercbhandle{};
		int					m_GeometryShaderMatcbhandle{ InvalidID };
		int					m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_PixelShadercbhandle{};
	public:
		ShaderController(void) noexcept {
			// シェーダーハンドル
			this->m_VertexShaderhandle = InvalidID;
			this->m_GeometryShaderhandle = InvalidID;
			this->m_PixelShaderhandle = InvalidID;
			// シェーダーに渡す追加パラメーターを配するハンドル
			for (auto& h : this->m_VertexShadercbhandle) {
				h = InvalidID;
			}
			this->m_GeometryShaderMatcbhandle = InvalidID;
			this->m_PixelShaderSendDispSizeHandle = InvalidID;
			for (auto& h : this->m_PixelShadercbhandle) {
				h = InvalidID;
			}
		}
		~ShaderController(void) noexcept {
			Dispose();
		}
	public:
		// 初期化
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : this->m_VertexShadercbhandle) {
				h = DxLib::CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_VertexShaderhandle = DxLib::LoadVertexShader(VertexShader);
			// ピクセルシェーダ―周り
			this->m_PixelShaderSendDispSizeHandle = DxLib::CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : this->m_PixelShadercbhandle) {
				h = DxLib::CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = DxLib::LoadPixelShader(PixelShader);
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : this->m_LightCameraMatrixHandle) {
				h = DxLib::CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
		}
		void			AddGeometryShader(const char* GeometryShader) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			this->m_GeometryShaderMatcbhandle = DxLib::CreateShaderConstantBuffer(sizeof(DX_D3D11_GS_CONST_BUFFER_BASE));
			this->m_GeometryShaderhandle = DxLib::LoadGeometryShader(GeometryShader);
		}
		// 後始末
		void			Dispose(void) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : this->m_LightCameraMatrixHandle) {
				DxLib::DeleteShaderConstantBuffer(h);
			}
			for (auto& h : this->m_VertexShadercbhandle) {
				DxLib::DeleteShaderConstantBuffer(h);
			}
			DxLib::DeleteShader(this->m_VertexShaderhandle);
			// 
			DxLib::DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DxLib::DeleteShader(this->m_GeometryShaderhandle);
			// ピクセルシェーダ―周り
			DxLib::DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : this->m_PixelShadercbhandle) {
				DxLib::DeleteShaderConstantBuffer(h);
			}
			DxLib::DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			DxLib::UpdateShaderConstantBuffer(BufferHandle);
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_VertexShadercbhandle[static_cast<size_t>(Slot - 4)];
			DxLib::FLOAT4* f4 = (DxLib::FLOAT4*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);		// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			DxLib::UpdateShaderConstantBuffer(BufferHandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		// シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const DxLib::MATRIX* ViewMatrix, const DxLib::MATRIX* ProjectionMatrix) const noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == InvalidID) { return; }
			auto& BufferHandle = this->m_GeometryShaderMatcbhandle;
			DX_D3D11_GS_CONST_BUFFER_BASE* LightCameraMatrixConst = (DX_D3D11_GS_CONST_BUFFER_BASE*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);

			// ビュー変換用行列をセットする
			LightCameraMatrixConst->ViewMatrix[0][0] = ViewMatrix->m[0][0];
			LightCameraMatrixConst->ViewMatrix[0][1] = ViewMatrix->m[1][0];
			LightCameraMatrixConst->ViewMatrix[0][2] = ViewMatrix->m[2][0];
			LightCameraMatrixConst->ViewMatrix[0][3] = ViewMatrix->m[3][0];
			LightCameraMatrixConst->ViewMatrix[1][0] = ViewMatrix->m[0][1];
			LightCameraMatrixConst->ViewMatrix[1][1] = ViewMatrix->m[1][1];
			LightCameraMatrixConst->ViewMatrix[1][2] = ViewMatrix->m[2][1];
			LightCameraMatrixConst->ViewMatrix[1][3] = ViewMatrix->m[3][1];
			LightCameraMatrixConst->ViewMatrix[2][0] = ViewMatrix->m[0][2];
			LightCameraMatrixConst->ViewMatrix[2][1] = ViewMatrix->m[1][2];
			LightCameraMatrixConst->ViewMatrix[2][2] = ViewMatrix->m[2][2];
			LightCameraMatrixConst->ViewMatrix[2][3] = ViewMatrix->m[3][2];
			// 投影変換用行列をセットする
			LightCameraMatrixConst->ProjectionMatrix[0][0] = ProjectionMatrix->m[0][0];
			LightCameraMatrixConst->ProjectionMatrix[0][1] = ProjectionMatrix->m[1][0];
			LightCameraMatrixConst->ProjectionMatrix[0][2] = ProjectionMatrix->m[2][0];
			LightCameraMatrixConst->ProjectionMatrix[0][3] = ProjectionMatrix->m[3][0];
			LightCameraMatrixConst->ProjectionMatrix[1][0] = ProjectionMatrix->m[0][1];
			LightCameraMatrixConst->ProjectionMatrix[1][1] = ProjectionMatrix->m[1][1];
			LightCameraMatrixConst->ProjectionMatrix[1][2] = ProjectionMatrix->m[2][1];
			LightCameraMatrixConst->ProjectionMatrix[1][3] = ProjectionMatrix->m[3][1];
			LightCameraMatrixConst->ProjectionMatrix[2][0] = ProjectionMatrix->m[0][2];
			LightCameraMatrixConst->ProjectionMatrix[2][1] = ProjectionMatrix->m[1][2];
			LightCameraMatrixConst->ProjectionMatrix[2][2] = ProjectionMatrix->m[2][2];
			LightCameraMatrixConst->ProjectionMatrix[2][3] = ProjectionMatrix->m[3][2];
			LightCameraMatrixConst->ProjectionMatrix[3][0] = ProjectionMatrix->m[0][3];
			LightCameraMatrixConst->ProjectionMatrix[3][1] = ProjectionMatrix->m[1][3];
			LightCameraMatrixConst->ProjectionMatrix[3][2] = ProjectionMatrix->m[2][3];
			LightCameraMatrixConst->ProjectionMatrix[3][3] = ProjectionMatrix->m[3][3];

			DxLib::UpdateShaderConstantBuffer(BufferHandle);
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_GEOMETRY, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			DxLib::UpdateShaderConstantBuffer(BufferHandle);
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShaderSendDispSizeHandle;
			DxLib::FLOAT2* dispsize = (DxLib::FLOAT2*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			DxLib::UpdateShaderConstantBuffer(BufferHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShadercbhandle[static_cast<size_t>(Slot - 3)];
			DxLib::FLOAT4* f4 = (DxLib::FLOAT4*)DxLib::GetBufferShaderConstantBuffer(BufferHandle);				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			DxLib::UpdateShaderConstantBuffer(BufferHandle);											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			DxLib::SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		// 3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw_lamda(std::function<void()> doing) const noexcept {
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			DxLib::SetUseVertexShader(this->m_VertexShaderhandle);											// 使用する頂点シェーダーをセット
			DxLib::SetUsePixelShader(this->m_PixelShaderhandle);											// 使用するピクセルシェーダーをセット
			DxLib::SetUseGeometryShader(this->m_GeometryShaderhandle);										// 使用するジオメトリシェーダーをセット
			DxLib::MV1SetUseOrigShader(TRUE);
			doing();
			DxLib::MV1SetUseOrigShader(FALSE);
			DxLib::SetUseVertexShader(InvalidID);
			DxLib::SetUsePixelShader(InvalidID);
			DxLib::SetUseGeometryShader(InvalidID);
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//スクリーンバッファのつかいまわし
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassScreenBufferPool : public Util::SingletonBase<PostPassScreenBufferPool> {
	private:
		friend class Util::SingletonBase<PostPassScreenBufferPool>;
	private:
		class PostPassScreenBuffer {
			static const int			m_Size = 3;
		private:
			std::array<Draw::ScreenHandle, m_Size>	m_Screen{};
			int										m_xSize{};
			int										m_ySize{};
			int										m_ZBufferBitDepth{};
			int										m_UsedLocal{};
			int										m_Used{};
			int										m_UnUseFrame{};
			bool									m_isTrans{};
			bool									m_isDepth{};
			char		padding[6]{};
		public:
			PostPassScreenBuffer(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
				this->m_xSize = xsize;
				this->m_ySize = ysize;
				this->m_isTrans = trans;
				this->m_isDepth = isDepth;
				this->m_ZBufferBitDepth = ZBufferBitDepth;
				int PrevZBufferBitDepth{};
				if (this->m_ZBufferBitDepth != InvalidID) {
					PrevZBufferBitDepth = DxLib::GetCreateDrawValidGraphZBufferBitDepth();
					DxLib::SetCreateDrawValidGraphZBufferBitDepth(this->m_ZBufferBitDepth);
				}
				for (auto& s : this->m_Screen) {
					if (this->m_isDepth) {
						s.MakeDepth(this->m_xSize, this->m_ySize);
					}
					else {
						s.Make(this->m_xSize, this->m_ySize, this->m_isTrans);
					}
				}
				if (this->m_ZBufferBitDepth != InvalidID) {
					DxLib::SetCreateDrawValidGraphZBufferBitDepth(PrevZBufferBitDepth);
				}
				this->m_Used = 0;
				this->m_UnUseFrame = 0;
				ResetUseCount();
			}
			PostPassScreenBuffer(const PostPassScreenBuffer&) = delete;
			PostPassScreenBuffer(PostPassScreenBuffer&&) = delete;
			PostPassScreenBuffer& operator=(const PostPassScreenBuffer&) = delete;
			PostPassScreenBuffer& operator=(PostPassScreenBuffer&&) = delete;
			~PostPassScreenBuffer(void) noexcept {
				for (auto& s : this->m_Screen) {
					s.Dispose();
				}
			}
		public:
			const Draw::ScreenHandle* PopBlankScreen(void) noexcept {
				if (this->m_UsedLocal >= this->m_Size) {
					MessageBox(NULL, "None Blank PostPassScreenBuffer", "", MB_OK);
					exit(InvalidID);
				}
				auto* Ret = &this->m_Screen[static_cast<size_t>(this->m_UsedLocal)];
				++this->m_Used;
				++this->m_UsedLocal;
				return Ret;
			}
			bool IsActive(void) const noexcept {
				return this->m_UnUseFrame <= 5;//5フレーム以上使われていない
			}
			bool Equal(int xsize, int ysize, bool trans, bool isDepth, int ZBufferBitDepth) const noexcept {
				return (
					(this->m_xSize == xsize) &&
					(this->m_ySize == ysize) &&
					(this->m_isTrans == trans) &&
					(this->m_isDepth == isDepth) &&
					this->m_ZBufferBitDepth == ZBufferBitDepth
					);
			}
			void ResetUseCount(void) noexcept {
				this->m_UsedLocal = 0;
			}
		public:
			void Update(void) noexcept {
				if (this->m_Used != 0) {
					this->m_Used = 0;
					this->m_UnUseFrame = 0;
				}
				else {
					++this->m_UnUseFrame;
				}
				ResetUseCount();
			}
		};
	private:
		std::vector<std::unique_ptr<PostPassScreenBuffer>>	m_ScreenBuffer;		// 描画スクリーン
	private:
		PostPassScreenBufferPool(void) noexcept {}
		PostPassScreenBufferPool(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool(PostPassScreenBufferPool&&) = delete;
		PostPassScreenBufferPool& operator=(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool& operator=(PostPassScreenBufferPool&&) = delete;
		virtual ~PostPassScreenBufferPool(void) noexcept {}
	public:
		const std::unique_ptr<PostPassScreenBuffer>& GetBlankScreen(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			auto Find = std::find_if(this->m_ScreenBuffer.begin(), this->m_ScreenBuffer.end(), [&](const std::unique_ptr<PostPassScreenBuffer>& tgt) {
				return  tgt->Equal(xsize, ysize, trans, isDepth, ZBufferBitDepth);
				});
			if (Find != this->m_ScreenBuffer.end()) {
				return (*Find);
			}
			this->m_ScreenBuffer.emplace_back(std::make_unique<PostPassScreenBuffer>(xsize, ysize, trans, isDepth, ZBufferBitDepth));
			return this->m_ScreenBuffer.back();
		}
		void ResetUseCount(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if (s->Equal(xsize, ysize, trans, isDepth, ZBufferBitDepth)) {
					s->ResetUseCount();
				}
			}
		}
	public:
		void FirstUpdate(void) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				s->Update();
			}
			//使われていないスクリーンバッファは消す
			for (size_t loop = 0, max = this->m_ScreenBuffer.size(); loop < max; ++loop) {
				auto& s = this->m_ScreenBuffer[loop];
				if (!s->IsActive()) {
					s.reset();
					std::swap(s, this->m_ScreenBuffer.back());
					this->m_ScreenBuffer.pop_back();
					--loop;
					--max;
				}
			}
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//ポストプロセス
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassEffect : public Util::SingletonBase<PostPassEffect> {
	private:
		friend class Util::SingletonBase<PostPassEffect>;
	public:
		class Gbuffer {
		public:
			Gbuffer(void) noexcept {}
			Gbuffer(const Gbuffer&) = delete;
			Gbuffer(Gbuffer&&) = delete;
			Gbuffer& operator=(const Gbuffer&) = delete;
			Gbuffer& operator=(Gbuffer&&) = delete;
			virtual ~Gbuffer(void) noexcept {}
		private:
			Draw::ScreenHandle			m_ColorScreen;	// そのまま透過なしにしたスクリーン
			Draw::ScreenHandle			m_NormalScreen;	// 法線のGバッファ
			Draw::ScreenHandle			m_DepthScreen;	// 深度のGバッファ
		public:
			const auto& GetColorBuffer(void) const noexcept { return this->m_ColorScreen; }
			const auto& GetNormalBuffer(void) const noexcept { return this->m_NormalScreen; }
			const auto& GetDepthBuffer(void) const noexcept { return this->m_DepthScreen; }
		public:
			void		Load(int xsize, int ysize) noexcept {
				auto Prev = DxLib::GetCreateDrawValidGraphZBufferBitDepth();
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(24);
				this->m_ColorScreen.Make(xsize, ysize, false);
				this->m_NormalScreen.Make(xsize, ysize, false);
				this->m_DepthScreen.MakeDepth(xsize, ysize);
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(Prev);
			}
			void		Dispose(void) noexcept {
				this->m_ColorScreen.Dispose();
				this->m_NormalScreen.Dispose();
				this->m_DepthScreen.Dispose();
			}
			void		Reset(void) noexcept {
				this->m_ColorScreen.SetDraw_Screen();
				this->m_NormalScreen.SetDraw_Screen();
				this->m_NormalScreen.FillGraph(128, 128, 255);
				this->m_DepthScreen.SetDraw_Screen();
				this->m_DepthScreen.FillGraph(255, 0, 0);
			}
		};
	public:
		// ベース
		class PostPassBase {
		protected:
			bool	m_PrevActive{ false };
			char		padding[7]{};
		public:
			PostPassBase(void) noexcept {}
			virtual ~PostPassBase(void) noexcept {}
		protected:
			virtual void Load_Sub(void) noexcept {}
			virtual void Dispose_Sub(void) noexcept {}
			virtual bool IsActive_Sub(void) noexcept { return true; }
			virtual void SetEffect_Sub(Draw::ScreenHandle*, Gbuffer*) noexcept {}
		public:
			bool IsActive(void) noexcept { return IsActive_Sub(); }
			void UpdateActive(bool active) noexcept {
				if (this->m_PrevActive != active) {
					this->m_PrevActive = active;
					if (active) {
						Load_Sub();
					}
					else {
						Dispose_Sub();
					}
				}
			}
			void SetEffect(Draw::ScreenHandle* TargetGraph, Gbuffer* pGbuffer) noexcept {
				if (IsActive()) {
					SetEffect_Sub(TargetGraph, pGbuffer);
				}
			}
		};
	private:
		//
		class ShadowDraw {
			static const int EXTEND = 1;
		private:
			Draw::ScreenHandle			m_BaseShadowHandle;
			Draw::ScreenHandle			m_DepthBaseScreenHandle;
			Draw::ScreenHandle			m_DepthBaseScreenHandle2;
			Draw::ScreenHandle			m_DepthNormalScreenHandle;
			Draw::ScreenHandle			m_DepthScreenHandle;
			Draw::ScreenHandle			m_DepthFarScreenHandle;

			ShaderController			m_Shader;
			ShaderController			m_ShaderRigid;
			Util::VECTOR3D				m_ShadowVec{ Util::VECTOR3D::up() };
			float						m_Scale{ 1.f };

			bool						m_PrevShadow{ false };
			char		padding[7]{};

			std::array<Util::Matrix4x4, 2>	m_CamViewMatrix{};
			std::array<Util::Matrix4x4, 2>	m_CamProjectionMatrix{};
		public:
			ShadowDraw(void) noexcept {
				this->m_PrevShadow = false;
				UpdateActive();
			}
			ShadowDraw(const ShadowDraw&) = delete;
			ShadowDraw(ShadowDraw&&) = delete;
			ShadowDraw& operator=(const ShadowDraw&) = delete;
			ShadowDraw& operator=(ShadowDraw&&) = delete;
			~ShadowDraw(void) noexcept {
				if (this->m_PrevShadow) {
					Dispose();
				}
			}
		private:
			void SetupCam(Util::VECTOR3D Center, float scale) const noexcept {
				DxLib::ClearDrawScreen();
				DxLib::SetupCamera_Ortho(30.f * scale);						// カメラのタイプを正射影タイプにセット、描画範囲も指定
				DxLib::SetCameraNearFar(0.05f * scale, 60.f * scale);		// 描画する奥行き範囲をセット
				// カメラの位置と注視点はステージ全体が見渡せる位置
				auto Vec = this->m_ShadowVec;
				if (Vec.x == 0.f && Vec.z == 0.f) {
					Vec.z = (0.1f);
				}
				DxLib::SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale)).get(), Center.get());
			}
			void			Init(void) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();

				int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
				int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;

				auto Prev = DxLib::GetCreateDrawValidGraphZBufferBitDepth();
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(24);

				this->m_BaseShadowHandle.Make(xsizeEx, ysizeEx, TRUE);
				int size = 2 << 10;
				this->m_DepthBaseScreenHandle.Make(size, size, FALSE);			// 深度バッファ用の作成
				this->m_DepthBaseScreenHandle2.Make(size, size, FALSE);			// 深度バッファ用の作成
				this->m_DepthNormalScreenHandle.Make(size, size, FALSE);			// 深度バッファ用の作成
				this->m_DepthScreenHandle.MakeDepth(size, size);					// 深度バッファの作成
				this->m_DepthFarScreenHandle.MakeDepth(size, size);				// 深度バッファの作成

				DxLib::SetCreateDrawValidGraphZBufferBitDepth(Prev);

				this->m_Shader.Init("CommonData/shader/VS_SoftShadow.vso", "CommonData/shader/PS_SoftShadow.pso");
				this->m_ShaderRigid.Init("CommonData/shader/VS_SoftShadow_Rigid.vso", "CommonData/shader/PS_SoftShadow.pso");
			}
			void			Dispose(void) noexcept {
				this->m_BaseShadowHandle.Dispose();
				this->m_DepthBaseScreenHandle.Dispose();
				this->m_DepthBaseScreenHandle2.Dispose();
				this->m_DepthNormalScreenHandle.Dispose();
				this->m_DepthScreenHandle.Dispose();
				this->m_DepthFarScreenHandle.Dispose();
				this->m_Shader.Dispose();
				this->m_ShaderRigid.Dispose();
			}
		public:
			const auto& GetCamViewMatrix(bool isFar) const noexcept { return this->m_CamViewMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
			const auto& GetCamProjectionMatrix(bool isFar) const noexcept { return this->m_CamProjectionMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
			const auto& GetDepthScreen(void) const noexcept { return this->m_DepthScreenHandle; }
			const auto& GetDepthFarScreen(void) const noexcept { return this->m_DepthFarScreenHandle; }//未使用
		public:
			void			SetVec(const Util::VECTOR3D& Vec) noexcept { this->m_ShadowVec = Vec; }
			void			SetDraw(std::function<void()> doing_rigid, std::function<void()> doing) noexcept {
				auto* CameraParts = Camera::Camera3D::Instance();
				Draw::Camera3DInfo tmp_cam = CameraParts->GetCameraForDraw();

				// 影の結果を出力
				tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.01f * Scale3DRate, 30.f * Scale3DRate);
				this->m_BaseShadowHandle.SetDraw_Screen();
				tmp_cam.FlipCamInfo();
				{
					auto* pOption = Util::OptionParam::Instance();
					const float ShadowLevel = static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect());
					this->m_BaseShadowHandle.SetUseTextureToShader(0);
					this->m_DepthScreenHandle.SetUseTextureToShader(1);
					this->m_DepthFarScreenHandle.SetUseTextureToShader(2);
					this->m_Shader.SetPixelParam(3, ShadowLevel, this->m_Scale * 450.f, 0.f, 0.f);
					this->m_Shader.SetVertexCameraMatrix(4, GetCamViewMatrix(false), GetCamProjectionMatrix(false));
					this->m_Shader.SetVertexCameraMatrix(5, GetCamViewMatrix(true), GetCamProjectionMatrix(true));
					this->m_Shader.Draw_lamda(doing);

					this->m_DepthScreenHandle.SetUseTextureToShader(1);
					this->m_DepthFarScreenHandle.SetUseTextureToShader(2);
					this->m_ShaderRigid.SetPixelParam(3, ShadowLevel, this->m_Scale * 45000.f, 0.f, 0.f);
					this->m_ShaderRigid.SetVertexCameraMatrix(4, GetCamViewMatrix(false), GetCamProjectionMatrix(false));
					this->m_ShaderRigid.SetVertexCameraMatrix(5, GetCamViewMatrix(true), GetCamProjectionMatrix(true));
					this->m_ShaderRigid.Draw_lamda(doing_rigid);
				}
				DxLib::SetUseTextureToShader(1, InvalidID);				// 使用テクスチャの設定を解除
				DxLib::SetUseTextureToShader(2, InvalidID);				// 使用テクスチャの設定を解除
				// 後処理
				this->m_BaseShadowHandle.GraphBlend(this->m_DepthBaseScreenHandle, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_R);
			}
			void			SetActive(bool shadow) noexcept {
				if (this->m_PrevShadow != shadow) {
					this->m_PrevShadow = shadow;
					if (shadow) {
						Init();
					}
					else {
						Dispose();
					}
				}
			}
			bool			UpdateActive(void) noexcept {
				auto* pOption = Util::OptionParam::Instance();
				bool shadow = pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0;
				if (this->m_PrevShadow != shadow) {
					SetActive(shadow);
					if (shadow) {
						return true;
					}
				}
				return false;
			}
		public:
			void			Update(std::function<void()> Shadowdoing, Util::VECTOR3D Center, float Scale) noexcept {
				this->m_Scale = Scale;
				// 影用の深度記録画像の準備を行う
				this->m_DepthBaseScreenHandle.SetRenderTargetToShader(0);
				this->m_DepthNormalScreenHandle.SetRenderTargetToShader(1);
				this->m_DepthScreenHandle.SetRenderTargetToShader(2);
				{
					SetupCam(Center, this->m_Scale * Scale3DRate);
					this->m_CamViewMatrix[0] = DxLib::GetCameraViewMatrix();
					this->m_CamProjectionMatrix[0] = DxLib::GetCameraProjectionMatrix();
					Shadowdoing();
				}
				DxLib::SetRenderTargetToShader(0, InvalidID);
				DxLib::SetRenderTargetToShader(1, InvalidID);
				DxLib::SetRenderTargetToShader(2, InvalidID);
			}
			void			UpdateFar(std::function<void()> Shadowdoing, Util::VECTOR3D Center, float Scale) noexcept {
				// 影用の深度記録画像の準備を行う
				this->m_DepthBaseScreenHandle2.SetRenderTargetToShader(0);
				this->m_DepthNormalScreenHandle.SetRenderTargetToShader(1);
				this->m_DepthFarScreenHandle.SetRenderTargetToShader(2);
				{
					SetupCam(Center, Scale * Scale3DRate);
					this->m_CamViewMatrix[1] = DxLib::GetCameraViewMatrix();
					this->m_CamProjectionMatrix[1] = DxLib::GetCameraProjectionMatrix();
					Shadowdoing();
				}
				DxLib::SetRenderTargetToShader(0, InvalidID);
				DxLib::SetRenderTargetToShader(1, InvalidID);
				DxLib::SetRenderTargetToShader(2, InvalidID);
			}
			void			Draw(void) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				this->m_BaseShadowHandle.DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//this->m_DepthBaseScreenHandle2.DrawExtendGraph(0, 0,1080,1080, true);
			}
		};
		//
		struct DoFParam {
			float						m_near{ 0.f };
			float						m_far{ 0.f };
			float						m_near_Max{ 0.f };
			float						m_far_Min{ 0.f };
		public:
			void			Reset(void) noexcept {
				this->m_near = 0.f;
				this->m_far = 0.f;
				this->m_near_Max = 0.f;
				this->m_far_Min = 0.f;
			}
		};
		struct ScopeParam {
			bool						m_IsActive{ false };
			char		padding[3]{};
			float						m_Xpos{ 0.f };
			float						m_Ypos{ 0.f };
			float						m_Radius{ 0.f };
			float						m_Zoom{ 0.f };
		public:
			void			Reset(void) noexcept {
				this->m_IsActive = false;
				this->m_Zoom = 1.f;
			}
		};
		class GodRayParam {
			float						m_GodRayPer{ 0.5f };
			float						m_GodRayPerByPostPass{ 1.f };
		public:
			auto			GetGodRayPerRet(void) const noexcept { return this->m_GodRayPer * this->m_GodRayPerByPostPass; }
			auto			IsActive(void) const noexcept { return this->m_GodRayPer > 0.f; }
		public:
			void			SetGodRayPer(float value) noexcept { this->m_GodRayPer = value; }
			void			SetGodRayPerByPostPass(float value) noexcept {
				Util::Easing(&this->m_GodRayPerByPostPass, value, 0.995f);
			}
		};
		struct ColorParam {
			int							m_InColorPerMin = 20;
			int							m_InColorPerMax = 255;
			float						m_InColorGamma = 1.1f;
		};
	private:
		std::array<std::unique_ptr<PostPassBase>, 16>	m_PostPass;
		//
		std::unique_ptr<ShadowDraw>	m_ShadowDraw;
		float						m_ShadowScale{ 1.f };
		bool						m_ShadowFarChange{ false };
		bool						m_IsActiveGBuffer{ false };
		char		padding[2]{};
		Draw::ScreenHandle			m_BufferScreen;	// 描画スクリーン

		Draw::ScreenHandle			m_DepthColorScreen;	// 描画スクリーン
		Draw::ScreenHandle			m_DepthNormalScreen;	// 描画スクリーン
		Draw::ScreenHandle			m_DepthBufferScreen;	// 描画スクリーン
		Draw::ScreenHandle			m_DepthBufferScreen2;	// 描画スクリーン
		Gbuffer						m_Gbuffer;
		Util::Matrix4x4				m_CamViewMat{};
		Util::Matrix4x4				m_CamProjectionMat{};
		ColorParam					m_ColorParam{};
		DoFParam					m_DoFParam{};
		ScopeParam					m_ScopeParam{};
		float						m_BlackOutParamPer{ 0.f };
		float						m_AberrationPower{ 1.f };
		float						m_DistortionPer{ 120.f };
		GodRayParam					m_GodRayParam{};
		Util::VECTOR3D				m_AmbientLightVec{};
		Draw::ScreenHandle			m_DepthDiff;
		Shader2DController			m_Shader;
		Shader2DController			m_Shader3D;
	public:
		const auto&		GetBufferScreen(void) const noexcept { return this->m_BufferScreen; }
		const auto&		GetAmbientLightVec(void) const noexcept { return this->m_AmbientLightVec; }
		const auto&		GetCamViewMat(void) const noexcept { return this->m_CamViewMat; }
		const auto&		GetCamProjectionMat(void) const noexcept { return this->m_CamProjectionMat; }
		const auto&		GetShadowDraw(void) const noexcept { return this->m_ShadowDraw; }
		const auto&		GetDoFParam(void) const noexcept { return this->m_DoFParam; }
		const auto&		GetScopeParam(void) const noexcept { return this->m_ScopeParam; }
		const auto&		GetBlackOutParamPer(void) const noexcept { return this->m_BlackOutParamPer; }
		const auto&		GetGodRayParam(void) const noexcept { return this->m_GodRayParam; }
		const auto&		GetAberrationPower(void) const noexcept { return this->m_AberrationPower; }
		const auto&		GetDistortionPer(void) const noexcept { return this->m_DistortionPer; }
	public:
		auto&			SetScopeParam(void) noexcept { return this->m_ScopeParam; }
		void			SetAberrationPower(float value) noexcept { this->m_AberrationPower = value; }
		void			SetDistortionPer(float value) noexcept { this->m_DistortionPer = value; }
		void			SetBlackOutParamPer(float value) noexcept { this->m_BlackOutParamPer = value; }
		void			SetGodRayPerByPostPass(float value) noexcept { this->m_GodRayParam.SetGodRayPerByPostPass(value); }
		void			SetGodRayPer(float value) noexcept { this->m_GodRayParam.SetGodRayPer(value); }
		void			SetShadowScale(float value) noexcept { this->m_ShadowScale = value; }
		// ボケ始める場所を指定(完全にボケるのはニアファーの限度)
		void			SetDoFNearFar(float near_d, float far_d, float near_m, float far_m) noexcept {
			this->m_DoFParam.m_near = near_d;
			this->m_DoFParam.m_far = far_d;
			this->m_DoFParam.m_near_Max = near_m;
			this->m_DoFParam.m_far_Min = far_m;
		}
		//
		void			SetLevelFilter(int inMin, int inMax, float gamma) noexcept {
			this->m_ColorParam.m_InColorPerMin = std::clamp(inMin, 0, 255);
			this->m_ColorParam.m_InColorPerMax = std::clamp(inMax, 0, 255);
			this->m_ColorParam.m_InColorGamma = std::max(1.f, gamma);
		}
		void			ResetAllParams(void) noexcept {
			this->m_ScopeParam.Reset();
			SetAberrationPower(1.f);
			SetDistortionPer(1.f);
			SetBlackOutParamPer(0.f);
			SetGodRayPerByPostPass(0.f);
			SetShadowScale(1.f);
			SetLevelFilter(0, 255, 1.f);
		}
	private:
		PostPassEffect(void) noexcept;
		PostPassEffect(const PostPassEffect&) = delete;
		PostPassEffect(PostPassEffect&&) = delete;
		PostPassEffect& operator=(const PostPassEffect&) = delete;
		PostPassEffect& operator=(PostPassEffect&&) = delete;
		virtual ~PostPassEffect(void) noexcept {
			Dispose();
		}
	private:
		void		Init(void) noexcept {
			PostPassScreenBufferPool::Create();
			//
			this->m_ShadowDraw = std::make_unique<ShadowDraw>();
		}
		void		Dispose(void) noexcept {
			this->m_DepthColorScreen.Dispose();
			this->m_DepthNormalScreen.Dispose();
			this->m_DepthBufferScreen.Dispose();
			this->m_DepthBufferScreen2.Dispose();
			this->m_DepthDiff.Dispose();
			this->m_BufferScreen.Dispose();

			this->m_Shader.Dispose();
			m_Shader3D.Dispose();

			PostPassScreenBufferPool::Release();
			// ポストエフェクト
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P.reset();
			}
			this->m_ShadowDraw.reset();
		}
	private:
		void		UpdateActiveGBuffer(bool ActiveGBuffer) noexcept {
			if (this->m_IsActiveGBuffer != ActiveGBuffer) {
				this->m_IsActiveGBuffer = ActiveGBuffer;
				if (this->m_IsActiveGBuffer) {
					auto* DrawerMngr = Draw::MainDraw::Instance();
					this->m_Gbuffer.Load(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight());
				}
				else {
					this->m_Gbuffer.Dispose();
				}
			}
		}
		void		UpdateBuffer(void) noexcept {
			bool ActiveGBuffer = false;
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				if (!P->IsActive()) { continue; }
				ActiveGBuffer = true;
				break;
			}
			UpdateActiveGBuffer(ActiveGBuffer);
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P->UpdateActive(P->IsActive());
			}
		}
	public:
		//
		void		SetAmbientLight(const Util::VECTOR3D& AmbientLightVec) noexcept {
			this->m_AmbientLightVec = AmbientLightVec;
			this->m_ShadowDraw->SetVec(this->m_AmbientLightVec);
		}
		//
		void		UpdateShadowActive(void) noexcept {
			if (this->m_ShadowDraw->UpdateActive()) {
				SetShadowFarChange();
			}
		}
		void		SetShadowFarChange(void) noexcept { this->m_ShadowFarChange = true; }
		bool		PopShadowFarChange(void) noexcept {
			if (this->m_ShadowFarChange) {
				this->m_ShadowFarChange = false;
				return true;
			}
			return false;
		}
		void		Update_Shadow(std::function<void()> doing, const Util::VECTOR3D& CenterPos, bool IsFar) noexcept {
			// 影用の深度記録画像の準備を行う
			if (!IsFar) {
				this->m_ShadowDraw->Update(doing, CenterPos, this->m_ShadowScale);
			}
			else {
				this->m_ShadowDraw->UpdateFar(doing, CenterPos, this->m_ShadowScale * 4.f);
			}
		}
		void		SetDrawShadow(std::function<void()> setshadowdoing_rigid, std::function<void()> setshadowdoing) noexcept {
			// 影画像の用意
			this->m_ShadowDraw->SetDraw(setshadowdoing_rigid, setshadowdoing);
		}
		void		SetDepthDraw(std::function<void()> done) noexcept {
			return;
			auto* pOption = Util::OptionParam::Instance();
			if (!pOption->GetParam(pOption->GetOptionType(Util::OptionType::Silhouette))->IsActive()) { return; }
			if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			this->m_DepthBufferScreen.SetDraw_Screen();
			this->m_DepthBufferScreen.FillGraph(0, 0, 0);
			auto* CameraParts = Camera::Camera3D::Instance();
			// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
			this->m_DepthColorScreen.SetRenderTargetToShader(0);
			this->m_DepthNormalScreen.SetRenderTargetToShader(1);
			this->m_DepthBufferScreen.SetRenderTargetToShader(2);
			DxLib::ClearDrawScreenZBuffer();
			CameraParts->GetCameraForDraw().FlipCamInfo();
			DxLib::SetCameraNearFar(0.1f * Scale3DRate, 50.f * Scale3DRate);
			{
				done();
			}
			DxLib::SetRenderTargetToShader(0, InvalidID);
			DxLib::SetRenderTargetToShader(1, InvalidID);
			DxLib::SetRenderTargetToShader(2, InvalidID);
		}
		void		SetDepthDraw(std::function<void(int layer)> done) noexcept {
			if (GetUseDirect3DVersion() == DX_DIRECT3D_11) {
				{
					this->m_DepthBufferScreen.SetDraw_Screen();
					this->m_DepthBufferScreen.FillGraph(0, 0, 0);
					this->m_DepthColorScreen.SetDraw_Screen();
					this->m_DepthColorScreen.SetRenderTargetToShader(0);
					this->m_DepthNormalScreen.SetRenderTargetToShader(1);
					this->m_DepthBufferScreen.SetRenderTargetToShader(2);
					{
						auto* CameraParts = Camera::Camera3D::Instance();
						Draw::Camera3DInfo tmp_cam = CameraParts->GetCameraForDraw();
						tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 1.f * Scale3DRate, 3000000.f * Scale3DRate);
						tmp_cam.FlipCamInfo();
						done(0);
					}
					SetRenderTargetToShader(0, InvalidID);
					SetRenderTargetToShader(1, InvalidID);
					SetRenderTargetToShader(2, InvalidID);
				}
				{
					this->m_DepthBufferScreen2.SetDraw_Screen();
					this->m_DepthBufferScreen2.FillGraph(0, 0, 0);
					this->m_DepthColorScreen.SetDraw_Screen();
					this->m_DepthColorScreen.SetRenderTargetToShader(0);
					this->m_DepthNormalScreen.SetRenderTargetToShader(1);
					this->m_DepthBufferScreen2.SetRenderTargetToShader(2);
					{
						auto* CameraParts = Camera::Camera3D::Instance();
						Draw::Camera3DInfo tmp_cam = CameraParts->GetCameraForDraw();
						tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 1.f * Scale3DRate, 3000000.f * Scale3DRate);
						tmp_cam.FlipCamInfo();
						done(1);
					}
					SetRenderTargetToShader(0, InvalidID);
					SetRenderTargetToShader(1, InvalidID);
					SetRenderTargetToShader(2, InvalidID);
				}
				{
					this->m_DepthColorScreen.SetDraw_Screen();
					{
						this->m_DepthBufferScreen2.SetUseTextureToShader(0);
						this->m_Gbuffer.GetDepthBuffer().SetRenderTargetToShader(1);

						auto* DrawerMngr = Draw::MainDraw::Instance();
						int xsize = DrawerMngr->GetRenderDispWidth();
						int ysize = DrawerMngr->GetRenderDispHeight();

						m_Shader3D.SetDispSize(xsize, ysize);
						m_Shader3D.Draw();

						SetUseTextureToShader(0, InvalidID);
						SetUseTextureToShader(1, InvalidID);
					}
					GetBufferScreen().SetDraw_Screen(false);
					{
						SetDrawBright(255, 255, 0);
						this->m_DepthColorScreen.DrawGraph(0, 0, true);
						SetDrawBright(255, 255, 255);
					}
				}
				/*
				for (int loop = 0; loop < 2; ++loop) {
					this->m_DepthBufferScreen.SetDraw_Screen();
					this->m_DepthBufferScreen.FillGraph(0, 0, 0);
					this->m_DepthColorScreen.SetDraw_Screen();
					this->m_DepthColorScreen.SetRenderTargetToShader(0);
					this->m_DepthNormalScreen.SetRenderTargetToShader(1);
					this->m_DepthBufferScreen.SetRenderTargetToShader(2);
					{
						auto* CameraParts = Camera::Camera3D::Instance();
						Draw::Camera3DInfo tmp_cam = CameraParts->GetCameraForDraw();
						tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 1.f * Scale3DRate, 30000.f * Scale3DRate);
						tmp_cam.FlipCamInfo();
						done(loop);
					}
					SetRenderTargetToShader(0, InvalidID);
					SetRenderTargetToShader(1, InvalidID);
					SetRenderTargetToShader(2, InvalidID);

					this->m_DepthColorScreen.SetDraw_Screen();
					{
						m_Gbuffer.GetDepthBuffer().SetUseTextureToShader(0);
						this->m_DepthBufferScreen.SetUseTextureToShader(1);

						auto* DrawerMngr = Draw::MainDraw::Instance();
						int xsize = DrawerMngr->GetRenderDispWidth();
						int ysize = DrawerMngr->GetRenderDispHeight();

						m_Shader3D.SetDispSize(xsize, ysize);
						m_Shader3D.Draw();

						SetUseTextureToShader(0, InvalidID);
						SetUseTextureToShader(1, InvalidID);
					}
					GetBufferScreen().SetDraw_Screen(false);
					{
						SetDrawBright(255, 255, 0);
						this->m_DepthColorScreen.DrawGraph(0, 0, true);
						SetDrawBright(255, 255, 255);
					}
				}
				//*/
			}
		}
		//
		void		StartDraw(void) noexcept {
			// リセット
			if (this->m_IsActiveGBuffer) {
				this->m_Gbuffer.Reset();
			}
			// カメラ
			auto* CameraParts = Camera::Camera3D::Instance();
			this->m_CamViewMat = CameraParts->GetCameraForDraw().GetViewMatrix();
			this->m_CamProjectionMat = CameraParts->GetCameraForDraw().GetProjectionMatrix();
		}
		void		DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept {
			auto* CameraParts = Camera::Camera3D::Instance();
			// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
			this->m_BufferScreen.SetRenderTargetToShader(0);
			if (this->m_IsActiveGBuffer) {
				this->m_Gbuffer.GetNormalBuffer().SetRenderTargetToShader(1);
				this->m_Gbuffer.GetDepthBuffer().SetRenderTargetToShader(2);
			}
			DxLib::ClearDrawScreenZBuffer();
			CameraParts->GetCameraForDraw().FlipCamInfo();
			DxLib::SetCameraNearFar(near_len, far_len);
			{
				done();
			}
			DxLib::SetRenderTargetToShader(0, InvalidID);
			if (this->m_IsActiveGBuffer) {
				DxLib::SetRenderTargetToShader(1, InvalidID);
				DxLib::SetRenderTargetToShader(2, InvalidID);
			}
		}
		void		EndDraw(void) noexcept {
			this->m_BufferScreen.SetDraw_Screen(false);
			auto* pOption = Util::OptionParam::Instance();
			auto* ScreenBufferPool = PostPassScreenBufferPool::Instance();
			// 影
			if (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0) {
				this->m_ShadowDraw->Draw();
			}
			// 色味補正
			this->m_BufferScreen.GraphFilter(DX_GRAPH_FILTER_LEVEL, this->m_ColorParam.m_InColorPerMin, this->m_ColorParam.m_InColorPerMax, static_cast<int>(this->m_ColorParam.m_InColorGamma * 100), 0, 255);
			// ポストプロセス
			ScreenBufferPool->FirstUpdate();
			if (this->m_IsActiveGBuffer) {
				for (auto& P : this->m_PostPass) {
					if (!P) { continue; }
					if (!P->IsActive()) { continue; }
					this->m_Gbuffer.GetColorBuffer().GraphFilterBlt(this->m_BufferScreen, DX_GRAPH_FILTER_DOWN_SCALE, 1);
					P->SetEffect(&this->m_BufferScreen, &this->m_Gbuffer);
				}
			}
			//深度による描画
			{
				if (!pOption->GetParam(pOption->GetOptionType(Util::OptionType::Silhouette))->IsActive()) { return; }
				if (DxLib::GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
				auto* DrawerMngr = Draw::MainDraw::Instance();
				static const int EXTEND = 4;

				int xsizeEx = DrawerMngr->GetRenderDispWidth();
				int ysizeEx = DrawerMngr->GetRenderDispHeight();

				this->m_DepthBufferScreen2.GraphFilterBlt(this->m_Gbuffer.GetDepthBuffer(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);

				this->m_DepthDiff.SetDraw_Screen();
				this->m_DepthBufferScreen.SetUseTextureToShader(0);
				this->m_DepthBufferScreen2.SetUseTextureToShader(1);
				{
					this->m_Shader.SetDispSize(xsizeEx, ysizeEx);
					this->m_Shader.Draw();
				}
				DxLib::SetUseTextureToShader(0, InvalidID);				// 使用テクスチャの設定を解除
				DxLib::SetUseTextureToShader(1, InvalidID);				// 使用テクスチャの設定を解除
				this->m_DepthDiff.GraphFilter(DX_GRAPH_FILTER_GAUSS, 16, 1000);
				//pScreenBuffer->GraphFilter(DX_GRAPH_FILTER_BILATERAL_BLUR);
				this->m_BufferScreen.SetDraw_Screen(false);
				{
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					DxLib::SetDrawBright(0, 128, 0);
					this->m_DepthDiff.DrawExtendGraph(0, 0, DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
					DxLib::SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
		}
	public:
		void		Reset(void) noexcept {
			UpdateActiveGBuffer(false);
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P->UpdateActive(false);
			}
			this->m_ShadowDraw->SetActive(false);
			UpdateBuffer();
			ResetAllParams();
			UpdateShadowActive();

			auto* DrawerMngr = Draw::MainDraw::Instance();

			this->m_BufferScreen.Dispose();

			{
				auto Prev = DxLib::GetCreateDrawValidGraphZBufferBitDepth();
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(24);
				this->m_BufferScreen.Make(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(Prev);
			}

			this->m_DepthColorScreen.Dispose();
			this->m_DepthNormalScreen.Dispose();
			this->m_DepthBufferScreen.Dispose();
			this->m_DepthBufferScreen2.Dispose();
			this->m_DepthDiff.Dispose();
			this->m_Shader.Dispose();

			//auto* pOption = Util::OptionParam::Instance();
			//if (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Silhouette))->IsActive()) 
			{
				static const int EXTEND = 1;

				int xsizeEx = DrawerMngr->GetRenderDispWidth() / EXTEND;
				int ysizeEx = DrawerMngr->GetRenderDispHeight() / EXTEND;
				auto Prev = DxLib::GetCreateDrawValidGraphZBufferBitDepth();
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(24);
				this->m_DepthColorScreen.Make(xsizeEx, ysizeEx, true);
				this->m_DepthNormalScreen.Make(xsizeEx, ysizeEx);
				this->m_DepthBufferScreen.MakeDepth(xsizeEx, ysizeEx);
				this->m_DepthBufferScreen2.MakeDepth(xsizeEx, ysizeEx);
				this->m_DepthDiff.Make(DrawerMngr->GetRenderDispWidth(), DrawerMngr->GetRenderDispHeight(), true);
				DxLib::SetCreateDrawValidGraphZBufferBitDepth(Prev);
				this->m_Shader.Init("CommonData/shader/PS_Depth.pso");
			}

			m_Shader3D.Dispose();
			m_Shader3D.Init("CommonData/shader/PS_Depth.pso");
		}
	};
};
