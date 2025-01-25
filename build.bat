@echo off

pushd build
cl -Zi ../src/win32_gewuln.cpp user32.lib gdi32.lib
popd
