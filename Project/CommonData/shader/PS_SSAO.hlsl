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

SamplerState g_NormalMapSampler : register(s1); // 法線マップサンプラ
Texture2D g_NormalMapTexture : register(t1); // 法線マップテクスチャ

SamplerState g_DepthMapSampler : register(s2); // 深度マップサンプラ
Texture2D g_DepthMapTexture : register(t2); // 深度マップテクスチャ


#define SAMPLES 16
#define INTENSITY 1.25
#define SCALE 0.0
#define BIAS 0.33
#define SAMPLE_RAD 20.0
#define MAX_DISTANCE 1250.0

#define MOD3 float3(0.1031,0.11369,0.13787)

float hash12(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * MOD3);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float3 DisptoProj(float2 screenUV)
{
    float2 pos;

    pos.x = screenUV.x * 2.0f - 1.0f;
    pos.y = 1.0f - screenUV.y * 2.0f;

    float3 V_to_Eye;
    V_to_Eye.x = pos.x * caminfo.z * dispsize.x / dispsize.y;
    V_to_Eye.y = pos.y * caminfo.z;
    V_to_Eye.z = 1.0f;

    return V_to_Eye * (g_DepthMapTexture.Sample(g_DepthMapSampler, screenUV).r / (caminfo.y * 0.005f)); //距離
}

float AmbientOcclusion(in float3 diff, in float3 normal)
{
    float len = length(diff);
    float dotnrm = dot(normal, diff / len); //単位ベクトル
    if (dotnrm < 0)
    {
        return 0;
    }
    float d = (1.0 / (1.0 + len * SCALE)); //近いほど高い係数をかける
    float ao = max(0.0, dotnrm - BIAS) * d;
    ao *= smoothstep(MAX_DISTANCE, MAX_DISTANCE * 0.5, len);
    return ao;

}

float SSAO(float2 tcoord, float3 pos, float3 normal, float rad)
{
    float goldenAngle = 2.4;
    float inv = 1.0 / float(SAMPLES);

    float radian = hash12(tcoord * 100.0) * 6.28;
    float rStep = inv * rad;
    float2 spiralUV;

    float radius = 0.0;
    float ao = 0.0;
    for (int i = 0; i < SAMPLES; i++)
    {
        spiralUV.x = sin(radian);
        spiralUV.y = cos(radian);
        radius += rStep;
        ao += AmbientOcclusion(DisptoProj(tcoord + spiralUV * radius) - pos, normal);
        radian += goldenAngle;
    }
    ao *= inv;
    ao = 1 - ao * INTENSITY;
    return ao;
}

PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    float3 normal = g_NormalMapTexture.Sample(g_NormalMapSampler, PSInput.TextureCoord0).xyz;
    normal.x = normal.x * 2.0f - 1.0f;
    normal.y = normal.y * 2.0f - 1.0f;
    normal.z = normal.z * 2.0f - 1.0f;

    float3 pos = DisptoProj(PSInput.TextureCoord0);

    float ao = 1.0;
    float rad = SAMPLE_RAD / pos.z;
    ao = SSAO(PSInput.TextureCoord0, pos, normal, rad);
    

    //ao = lerp(ao, 1.0f, abs(normal.x)/1.5f);
    //ao = lerp(ao, 1.0f, saturate(pos.z / 1000.f));
    
    PSOutput.color0 = float4(ao, ao, ao, 1.0f);
    
    if (g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.TextureCoord0).r / 100.f < 0.1f)
    {
        PSOutput.color0.r = 1.f;
        PSOutput.color0.g = 1.f;
        PSOutput.color0.b = 1.f;
        PSOutput.color0.a = 1.f;
    }
    
    return PSOutput;
}