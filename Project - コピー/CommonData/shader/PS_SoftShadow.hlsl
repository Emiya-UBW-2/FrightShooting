// ピクセルシェーダーの入力
struct PS_INPUT
{
    float4 Diffuse : COLOR0; // ディフューズカラー
    float4 Specular : COLOR1; // スペキュラカラー
    float2 TexCoords0 : TEXCOORD0; // テクスチャ座標
    float4 LPPosition : TEXCOORD1; // ライトからみた座標( xとyはライトの射影座標、zはビュー座標 )
	float4 LPPosition2 : TEXCOORD2; // ライトからみた座標( xとyはライトの射影座標、zはビュー座標 )
    float4 Position : SV_POSITION; // 座標( プロジェクション空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 Color0 : SV_TARGET0; // 色
};

// プログラムとのやり取りのために使うレジスタ2
cbuffer cbMULTIPLYCOLOR_CBUFFER2 : register(b3) {
	float4	g_param;
}

SamplerState g_DepthMapSampler : register(s1); // 深度バッファテクスチャ
Texture2D g_DepthMapTexture : register(t1); // 深度バッファテクスチャ

SamplerState g_DepthMapSampler2 : register(s2); // 深度バッファテクスチャ
Texture2D g_DepthMapTexture2 : register(t2); // 深度バッファテクスチャ


// main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;

	float comp;
	int total;

	PSOutput.Color0.rgb = 0.f;
	PSOutput.Color0.a = 1.f;

    float Threshold = g_param.z;
    float Threshold2 = g_param.w;

	// 深度バッファテクスチャから深度を取得( +補正値 )
	{
        float LightDepth = PSInput.LPPosition.z;
        comp = 0;
        if ((int) g_param.x == 3)
        {
            const int xs = 3;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold))
                    {
                        comp++;
                    }
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
        if ((int) g_param.x == 2)
        {
            const int xs = 2;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold))
                    {
                        comp++;
                    }
                    total++;
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
        if ((int) g_param.x == 1)
        {
            const int xs = 1;
    		[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture.Sample(g_DepthMapSampler, PSInput.LPPosition.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold))
                    {
                        comp++;
                    }
                }
            }
            PSOutput.Color0.r = comp / ((xs * 2 + 1) * (xs * 2 + 1));
        }
    }

	//*
	// 深度バッファテクスチャから深度を取得( +補正値 )
	{
        if ((int) g_param.x == 3)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 3;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold2))
                    {
                        comp++;
                    }
                    total++;
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
        if ((int) g_param.x == 2)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 2;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold2))
                    {
                        comp++;
                    }
                    total++;
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
        if ((int) g_param.x == 1)
        {
            float LightDepth = PSInput.LPPosition2.z;
            comp = 0;
            total = 0;
            const int xs = 1;
	    	[fastopt]
            for (int x = -xs; x <= xs; x++)
            {
			[fastopt]
                for (int y = -xs; y <= xs; y++)
                {
                    float TextureDepth2 = g_DepthMapTexture2.Sample(g_DepthMapSampler2, PSInput.LPPosition2.xy, int2(x, y)).r;
                    if ((TextureDepth2 > 0.f) && LightDepth > (TextureDepth2 + Threshold2))
                    {
                        comp++;
                    }
                    total++;
                }
            }
            PSOutput.Color0.r = max(PSOutput.Color0.r, comp / total);
        }
    }

    /*
    if ((int) g_param.x > 2 && PSOutput.Color0.r < 128.f / 255.f)
    {
        PSOutput.Color0.r = 0.f;
    }
	//*/
	
    PSOutput.Color0.r = lerp(PSOutput.Color0.r, 0.f, saturate(PSInput.Position.w / g_param.y));

	// 出力パラメータを返す
    return PSOutput;
}
