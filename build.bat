@echo off

pushd build

cl -Zi ../src/win32_gewuln.cpp ^
/I"D:\MyProjects\cpp\third_party\glfw-3.4.bin.WIN32\include" ^
/DGLFW_STATIC /link /LIBPATH:"D:\MyProjects\cpp\third_party\glfw-3.4.bin.WIN32\lib-static-ucrt" ^
glfw3dll.lib OpenGL32.lib 

popd
