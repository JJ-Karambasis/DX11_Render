@echo off

set Warnings=-W4 -wd4100 -wd4201 -wd4805 -wd4189 -wd4291 -wd4996 -wd4706 -wd4533 -wd4127
set Common=-Od -nologo -FC -Z7 -Oi -EHsc- %Warnings%

IF NOT EXIST .\Bin mkdir .\Bin
pushd .\Bin
cl %Common% -I..\AKCommon ..\DX11_Render.cpp -link DXGI.lib D3D11.lib D3DCompiler.lib -opt:ref -incremental:no -out:DX11_Render.exe
popd