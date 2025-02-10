@REM main build script

@echo off

set third_party_path=D:\MyProjects\cpp\third_party
set glad_c_path=%third_party_path%\glad\src\glad.c

pushd build

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cl -Zi /std:c++20 /EHsc ../src/win64_gewuln.cpp ^
%glad_c_path% ^
../includes/stb/stb_image_impl.cpp ^
/I../includes ^
/I"%~dp0..\third_party\glfw-3.4.bin.WIN64\include" ^
/I"%~dp0..\third_party\glad\include" ^
/DGLFW_STATIC /link /LIBPATH:"%~dp0..\third_party\glfw-3.4.bin.WIN64\lib-vc2019" glfw3dll.lib ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" OpenGL32.lib

popd
