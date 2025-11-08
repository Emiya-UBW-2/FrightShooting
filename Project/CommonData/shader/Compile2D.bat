set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tps_4_0 PS_lens.hlsl
ShaderCompiler.exe /Tps_4_0 PS_SSR.hlsl
ShaderCompiler.exe /Tps_4_0 PS_CubeMap.hlsl
ShaderCompiler.exe /Tps_4_0 PS_SSAO.hlsl
ShaderCompiler.exe /Tps_4_0 PS_DoF.hlsl
ShaderCompiler.exe /Tps_4_0 PS_BlackOut.hlsl
ShaderCompiler.exe /Tps_4_0 PS_FXAA.hlsl
ShaderCompiler.exe /Tps_4_0 PS_GodRay.hlsl
ShaderCompiler.exe /Tps_4_0 PS_Depth.hlsl

pause

cd %NowDir%