@echo off
rem Таблица «the whole machine» замера SPSC при одном размере блока: кольцо (ring)
rem против очереди (queue), лучший из 9 прогонов. Пары писатель/читатель занимают
rem все ядра и делят L3, поэтому запускать на свободной машине: любая фоновая
rem нагрузка — сборка, браузер, индексация — забирает у замера ядра и кэш.
rem Использование: bench-machine.bat [16|64|128|256], по умолчанию 256.
rem Собирается в build\bench, как у tools\bench.ps1.
setlocal
set "BLOCK=%~1"
if "%BLOCK%"=="" set "BLOCK=256"
set "ROOT=%~dp0"

pwsh -NoProfile -Command "& '%ROOT%tools\build.ps1' -Config Release -BuildDir build\bench -Target wxl.async.spsc-queue-benchmark -Define 'WXL_BUILD_BENCHMARKS=ON','WXL_BUILD_DOCS=OFF'"
if errorlevel 1 exit /b %errorlevel%

"%ROOT%build\bench\wxl.core\benchmarks\async\Release\wxl.async.spsc-queue-benchmark.exe" 9 --machine %BLOCK%
exit /b %errorlevel%
