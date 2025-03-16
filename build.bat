@REM main build script

@echo off

set third_party_path=D:\MyProjects\cpp\third_party
set glad_c_path=%third_party_path%\glad-4.6-core\src\glad.c
set sources=../includes/gewuln/texture.cpp ../includes/gewuln/shader.cpp ../includes/gewuln/resource_manager.cpp ../src/game.cpp ../includes/gewuln/mesh.cpp

pushd build

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cl -Zi /std:c++20 /EHsc ../src/win64_gewuln.cpp ^
%glad_c_path% ^
../includes/stb/stb_image_impl.cpp ^
%sources% ^
/I../includes ^
/I"%~dp0..\third_party\glfw-3.4.bin.WIN64\include" ^
/I"%~dp0..\third_party\glad-4.6-core\include" ^
/DGLFW_STATIC ^
/link ^
/LIBPATH:"%~dp0..\third_party\glfw-3.4.bin.WIN64\lib-vc2019" glfw3dll.lib ^
/LIBPATH:"D:\MyProjects\cpp\third_party\assimp\assimp-5.4.3\lib\Debug" assimp-vc142-mtd.lib ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" OpenGL32.lib

popd
