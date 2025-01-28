@echo off

pushd build

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cl -Zi ../src/win32_gewuln.cpp ..\src\glad.c ^
/I"D:\MyProjects\cpp\third_party\glfw-3.4.bin.WIN64\include" ^
/I"D:\MyProjects\cpp\third_party\glad\include" ^
/DGLFW_STATIC /link /LIBPATH:"D:\MyProjects\cpp\third_party\glfw-3.4.bin.WIN64\lib-vc2019" glfw3dll.lib ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" OpenGL32.lib

popd
