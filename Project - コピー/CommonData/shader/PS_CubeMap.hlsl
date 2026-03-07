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
	float4 color0           : SV_TARGET0;	// 色
};

// 定数バッファピクセルシェーダー基本パラメータ
struct DX_D3D11_PS_CONST_BUFFER_BASE {
	float4		FactorColor;			// アルファ値等

	float		MulAlphaColor;			// カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float		AlphaTestRef;			// アルファテストで使用する比較値
	float2		Padding1;

	int		    AlphaTestCmpMode;		// アルファテスト比較モード( DX_CMP_NEVER など )
	int3		Padding2;

	float4		IgnoreTextureColor;	// テクスチャカラー無視処理用カラー
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1) {
	DX_D3D11_PS_CONST_BUFFER_BASE		g_Base;
};


// プログラムとのやり取りのために使うレジスタ1
cbuffer cbMULTIPLYCOLOR_CBUFFER1 : register(b2)
{
    float2 dispsize;
}

// プログラムとのやり取りのために使うレジスタ
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4 caminfo;
}

cbuffer cbLIGHTCAMERA_MATRIX : register(b4)
{
    matrix g_LightViewMatrix; // ライトのワールド　→　ビュー行列
    matrix g_LightProjectionMatrix; // ライトのビュー　　→　射影行列
};


//テクスチャ
SamplerState g_Register0MapSampler : register(s0); // ディフューズマップサンプラ
Texture2D g_Register0MapTexture : register(t0); // ディフューズマップテクスチャ

SamplerState g_Register1MapSampler : register(s1); // 法線マップサンプラ
Texture2D g_Register1MapTexture : register(t1); // 法線マップテクスチャ

SamplerState g_Register2MapSampler : register(s2); // 深度マップサンプラ
Texture2D g_Register2MapTexture : register(t2); // 深度マップテクスチャ


SamplerState dynamicCubeMapSampler : register(s3);
TextureCube dynamicCubeMapTexture : register(t3);

//関数
float4 GetTexColor1(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register1MapTexture.Sample(g_Register1MapSampler, texCoord, offset);
}
float4 GetTexColor2(float2 texCoord, int2 offset = int2(0, 0)) {
	return g_Register2MapTexture.Sample(g_Register2MapSampler, texCoord, offset);
}

float3 DisptoProjNorm(float2 screenUV) {
    screenUV /= 0.5f;
    screenUV.x = screenUV.x - 1.f;
    screenUV.y = 1.f - screenUV.y;

	float3 position;
    position.x = screenUV.x * caminfo.z * dispsize.x / dispsize.y;
    position.y = screenUV.y * caminfo.z;
	position.z = 1.f;

	return position;
}

PS_OUTPUT main(PS_INPUT PSInput) {
	//戻り値
	PS_OUTPUT PSOutput;
	//反射をどれだけ見せるか
    float uvFactor = 2.0 * length(PSInput.TextureCoord0 - float2(0.5, 0.5));
    uvFactor *= uvFactor;
    uvFactor *= uvFactor;
    float edge = max(0.0, 1.0 - uvFactor);
    float Per = GetTexColor2(PSInput.TextureCoord0).g * edge;
	//ノーマル座標取得
    float3 normal = GetTexColor1(PSInput.TextureCoord0).xyz * 2.f - 1.f;
    
	//処理
    float4 lWorldPosition;
	//キューブマップからの反射
    lWorldPosition.xyz = DisptoProjNorm(PSInput.TextureCoord0);
    lWorldPosition.w = 0.f;
	
	// ワールド座標を射影座標に変換
    float4 LPPosition1 = mul(g_LightViewMatrix, lWorldPosition);

    lWorldPosition.xyz = normal;
    lWorldPosition.w = 0.f;


    float4 LPPosition2 = mul(g_LightViewMatrix, lWorldPosition);
    float3 LPPosition3 = reflect(LPPosition1.xyz, LPPosition2.xyz);
    
    float4 RefColor = dynamicCubeMapTexture.Sample(dynamicCubeMapSampler, LPPosition3);

    PSOutput.color0 = lerp(float4(0.f, 0.f, 0.f, 0.f), RefColor, Per);
	return PSOutput;
}
