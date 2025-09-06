@echo off
REM Build the project
call build.bat

REM Check if build was successful
if %errorlevel% equ 0 (
    echo Running application...
    .\build\win64_gewuln.exe
) else (
    echo Build failed with error level %errorlevel%
)
