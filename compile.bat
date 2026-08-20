@ECHO OFF
SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
IF "%1"=="" (
    @ECHO Usage: compile.bat ^<path_to_vcpkg^> [--release]
    EXIT /B 1
)

SET "VCPKG_DIR=%~1"
SET "VCPKG_TOOLS_DIR=%VCPKG_DIR%\downloads\tools"
SET "BUILD_TYPE=Debug"

IF NOT "%~2"=="" (
    IF /I "%~2"=="--release" (
        SET "BUILD_TYPE=Release"
    ) ELSE (
        @ECHO Unknown option: %~2
        @ECHO Usage: compile.bat ^<path_to_vcpkg^> [--release]
        EXIT /B 1
    )
)

IF NOT "%~3"=="" (
    @ECHO Too many arguments.
    @ECHO Usage: compile.bat ^<path_to_vcpkg^> [--release]
    EXIT /B 1
)

REM SET CMAKE_EXE=%VCPKG_TOOLS_DIR%\cmake-3.10.2-windows\cmake-3.10.2-win32-x86\bin\cmake.exe
SET "CMAKE_EXE=cmake.exe"

RMDIR /s/q build
MKDIR build
PUSHD build
"%VCPKG_DIR%\vcpkg.exe" install sdl2:%VSCMD_ARG_TGT_ARCH%-windows
IF ERRORLEVEL 1 GOTO :BUILD_FAILED

"%CMAKE_EXE%" .. "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=!BUILD_TYPE! -DVCPKG_TARGET_TRIPLET=%VSCMD_ARG_TGT_ARCH%-windows
IF ERRORLEVEL 1 GOTO :BUILD_FAILED

nmake
IF ERRORLEVEL 1 GOTO :BUILD_FAILED

IF NOT EXIST "bin\gamelad_libretro.dll" (
    @ECHO Expected RetroArch core was not created: build\bin\gamelad_libretro.dll
    GOTO :BUILD_FAILED
)

IF NOT EXIST "bin\gamelad_libretro.info" (
    @ECHO Expected RetroArch core metadata was not created: build\bin\gamelad_libretro.info
    GOTO :BUILD_FAILED
)

POPD
@ECHO Built !BUILD_TYPE! RetroArch core and metadata in build\bin
ENDLOCAL
EXIT /B 0

:BUILD_FAILED
POPD
@ECHO Build failed.
ENDLOCAL
EXIT /B 1