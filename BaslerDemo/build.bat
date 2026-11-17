@echo off
rem === BaslerDemo one-click build (VS2022 + Qt 5.9 MSVC) ===
call "D:\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cd /d %~dp0
"D:\Qt\5.9.9\msvc2017_64\bin\qmake.exe" BaslerDemo.pro
if errorlevel 1 (
    echo [FAIL] qmake failed
    exit /b 1
)
nmake %1
if errorlevel 1 (
    echo [FAIL] nmake failed
    exit /b 1
)
echo [OK] Build succeeded: %~dp0release\BaslerDemo.exe
