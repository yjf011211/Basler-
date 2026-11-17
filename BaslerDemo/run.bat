@echo off
rem === BaslerDemo one-click run (sets Qt + pylon DLL paths) ===
cd /d %~dp0
if not exist "%~dp0release\BaslerDemo.exe" (
    echo [FAIL] release\BaslerDemo.exe not found. Run build.bat first.
    pause
    exit /b 1
)
set PATH=D:\Qt\5.9.9\msvc2017_64\bin;D:\Basler\pylon 6\Runtime\x64;%PATH%
start "" "%~dp0release\BaslerDemo.exe"
