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
struct PS_OUTPUT {
    float4 color0 : SV_TARGET0; // 色
};

// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE {
    float4 FactorColor; // アルファ値等

    float MulAlphaColor; // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
    float AlphaTestRef; // アルファテストで使用する比較値
    float2 Padding1;

    int AlphaTestCmpMode; // アルファテスト比較モード( DX_CMP_NEVER など )
    int3 Padding2;

    float4 IgnoreTextureColor; // テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1) {
    DX_D3D11_PS_CONST_BUFFER_BASE g_Base;
};

// プログラムとのやり取りのために使うレジスタ1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

SamplerState g_Tex1Sampler : register(s0); // ディフューズマップサンプラ
Texture2D g_Tex1Texture : register(t0); // ディフューズマップテクスチャ

SamplerState g_Tex2Sampler : register(s1); // ディフューズマップサンプラ
Texture2D g_Tex2Texture : register(t1); // ディフューズマップテクスチャ

// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
	// テクスチャカラーの読み込み
    float2 T2 = PSInput.TextureCoord0;
    T2.y = 1.f - T2.y;
    float Tex1 = g_Tex1Texture.Sample(g_Tex1Sampler, T2).r;
    float Tex2 = g_Tex2Texture.Sample(g_Tex2Sampler, T2).r;
    
    float Diff = (Tex1 - Tex2) / 10.f;

	// 出力カラー = テクスチャカラー * ディフューズカラー
    PSOutput.color0.rgb = float3(1, 1, 1);
    if (Tex1 < Tex2)
    {
        PSOutput.color0.a = 0.f;
    }
    else if (Tex1 > 0.f && Diff != 0.f)
    {
        if (Tex2 == 0.f)
        {
            PSOutput.color0.a = 1.f;
        }
        else
        {
            PSOutput.color0.a = Diff;
        }
    }
    else
    {
        PSOutput.color0.a = 0.f;
    }

	// 出力パラメータを返す
    return PSOutput;
}

