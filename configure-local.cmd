@echo off
rem Configures build\x64-dynamic with the dependencies taken from local copies in the
rem given directory (see WXL_LOCAL_SOURCE_DIR in docs\building.md), without building.
if "%~1"=="" (
    echo Usage: %~nx0 ^<directory with winmd, fmt, googletest, wxl_doxygen, wxl_doxygen_awesome^>
    exit /b 2
)
pwsh -NoProfile -Command "& '%~dp0tools\build.ps1' -Configure -Define WXL_BUILD_BENCHMARKS=ON,WXL_BUILD_DOCS=OFF,'WXL_LOCAL_SOURCE_DIR=%~f1'; exit $LASTEXITCODE"
