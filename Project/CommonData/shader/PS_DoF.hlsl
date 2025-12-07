// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Position : SV_POSITION; // 座標
    float4 DiffuseColor : COLOR0; // ディフューズカラー
    float4 SpecularColor : COLOR1; // スペキュラカラー
    float2 TextureCoord0 : TEXCOORD0; // テクスチャ座標０
    float2 TextureCoord1 : TEXCOORD1; // テクスチャ座標１
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
	float4 color0 : SV_TARGET0; // 色
};


// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE
{
	float4 FactorColor; // アルファ値等

	float MulAlphaColor; // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float AlphaTestRef; // アルファテストで使用する比較値
	float2 Padding1;

	int AlphaTestCmpMode; // アルファテスト比較モード( DX_CMP_NEVER など )
	int3 Padding2;

	float4 IgnoreTextureColor; // テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
	DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// プログラムとのやり取りのために使うレジスタ1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
	float2 dispsize;
}

// プログラムとのやり取りのために使うレジスタ2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3)
{
	float4 caminfo;
}

SamplerState g_DiffuseMapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_DiffuseMapTexture : register(t0); // ディフューズマップテクスチャ

SamplerState g_Diffuse2MapSampler : register(s1); // ディフューズマップサンプラ(ぼかし)近
Texture2D g_Diffuse2MapTexture : register(t1); // ディフューズマップテクスチャ(ぼかし)

SamplerState g_Diffuse3MapSampler : register(s2); // ディフューズマップサンプラ(ぼかし)遠
Texture2D g_Diffuse3MapTexture : register(t2); // ディフューズマップテクスチャ(ぼかし)

SamplerState g_DepthMapSampler : register(s3); // 深度マップサンプラ
Texture2D g_DepthMapTexture : register(t3); // 深度マップテクスチャ


PS_OUTPUT main(PS_INPUT PSInput)
{
    float2 UV = PSInput.TextureCoord0;
    UV.y = 1.f - UV.y;

	PS_OUTPUT PSOutput;

	float per = 0.f;
    float Depth = g_DepthMapTexture.Sample(g_DepthMapSampler, UV).r;

	float near_min = caminfo.z;
	float near_max = caminfo.x;
	float far_min = caminfo.y;
	float far_max = caminfo.w;

	//無し
	if (Depth == 0.f) {
		per = 0.f;
        PSOutput.color0 = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, UV);
    }
	//近
	else if (Depth < near_max) {
		per = 1.f - (Depth - near_min) / (near_max - near_min);
        if (per <= 0.f)
        {
            per = 0.f;
        }
        else if (per >= 1.f)
        {
            per = 1.f;
        }
        PSOutput.color0 = lerp(
		g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, UV),
		g_Diffuse2MapTexture.Sample(g_Diffuse2MapSampler, UV),
		per);
    }
	//中
	else if (near_max < Depth && Depth < far_min) {
		per = 0.f;
        PSOutput.color0 = g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, UV);
    }
	//遠
	else if (far_min < Depth) {
		per = (Depth - far_min) / (far_max - far_min);
        if (per <= 0.f)
        {
            per = 0.f;
        }
        else if (per >= 1.f)
        {
            per = 1.f;
        }
        PSOutput.color0 = lerp(
		g_DiffuseMapTexture.Sample(g_DiffuseMapSampler, UV),
		g_Diffuse3MapTexture.Sample(g_Diffuse3MapSampler, UV),
		per);
    }
	else {
		per = 1.f;
        PSOutput.color0 = g_Diffuse2MapTexture.Sample(g_Diffuse2MapSampler, UV);
    }

	return PSOutput;
}