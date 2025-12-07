set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tvs_5_0 VS_SoftShadow_Rigid.hlsl
ShaderCompiler.exe /Tvs_5_0 VS_SoftShadow.hlsl
ShaderCompiler.exe /Tps_5_0 PS_SoftShadow.hlsl

pause

cd %NowDir%