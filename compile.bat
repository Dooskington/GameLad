@ECHO OFF
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
IF "%1"=="" (
    @ECHO Please specify the full path to your VCPKG install.
    GOTO :EOF
)

SET VCPKG_DIR=%1
SET VCPKG_TOOLS_DIR=%VCPKG_DIR%\downloads\tools
SET CMAKE_EXE=%VCPKG_TOOLS_DIR%\cmake-3.10.2-windows\cmake-3.10.2-win32-x86\bin\cmake.exe

RMDIR /s/q build
MKDIR build
PUSHD build
%VCPKG_DIR%\vcpkg.exe install sdl2:%VSCMD_ARG_TGT_ARCH%-windows
%CMAKE_EXE% .. "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -G "NMake Makefiles" -DVCPKG_TARGET_TRIPLET=%VSCMD_ARG_TGT_ARCH%-windows
nmake
POPD

ENDLOCAL