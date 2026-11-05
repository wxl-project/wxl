pwsh -NoProfile -Command "& '%~dp0tools\build.ps1' -Config Release -Define WXL_BUILD_BENCHMARKS=ON,WXL_BUILD_DOCS=OFF; exit $LASTEXITCODE"
