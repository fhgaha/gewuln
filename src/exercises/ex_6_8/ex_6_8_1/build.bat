@echo off

set proj_path=D:\MyProjects\cpp\gewuln
set ex_group_name=ex_6_8
set src_file_name=ex_6_8_1
set to_build=%src_file_name%.cpp
set third_party_path=D:\MyProjects\cpp\third_party
set glad_c_path=%third_party_path%\glad\src\glad.c
set build_dir_path=%proj_path%\src\exercises\%ex_group_name%\%src_file_name%\build

pushd %build_dir_path%

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cl -Zi /EHsc ..\%to_build% %glad_c_path% ^
%glad_c_path% ^
%proj_path%/includes/gewuln/shader.cpp ^
/I%proj_path%/includes ^
/I"%third_party_path%\glfw-3.4.bin.WIN64\include" ^
/I"%third_party_path%\glad\include" ^
/DGLFW_STATIC /link /LIBPATH:"%third_party_path%\glfw-3.4.bin.WIN64\lib-vc2019" glfw3dll.lib ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" OpenGL32.lib

popd
