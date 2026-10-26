@echo off
rem Таблица «many queues per thread» замера SPSC: у каждой пары писатель/читатель по 8
rem своих очередей, spsc_queue против кольца слотов, лучший раунд и медиана из 9.
rem Пары занимают все ядра, а вопрос таблицы — чьё рабочее множество удержится в общем
rem L3, поэтому запускать на свободной машине: любая фоновая нагрузка — сборка, браузер,
rem индексация — забирает у замера ядра и вымывает тот самый кэш.
rem Собирается в build\bench, как у tools\bench.ps1.
setlocal
set "ROOT=%~dp0"

pwsh -NoProfile -Command "& '%ROOT%tools\build.ps1' -Config Release -BuildDir build\bench -Target wxl.async.spsc-queue-benchmark -Define 'WXL_BUILD_BENCHMARKS=ON','WXL_BUILD_DOCS=OFF'"
if errorlevel 1 exit /b %errorlevel%

"%ROOT%build\bench\wxl.async\benchmarks\Release\wxl.async.spsc-queue-benchmark.exe" 9 --many-queues
exit /b %errorlevel%
