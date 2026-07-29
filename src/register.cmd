@echo off
title Applying
mode con: cols=80 lines=25

REM Переходим в директорию скрипта
cd /d "%~dp0"

REM Проверяем права администратора
call :isAdmin
if %errorlevel% == 0 (
    echo Applying effects...
    regsvr32 "%~dp0ExplorerBlurMica.dll"
    echo.
) else (
    echo Please run as Administrator
    pause
    exit /b
)

echo Restarting explorer...
taskkill /F /IM explorer.exe >nul
start explorer.exe

echo.
echo Changes applied successfully
echo Window will close automatically in 5 seconds
timeout /t 5 >nul
exit /b 0

:isAdmin
fsutil dirty query %systemdrive% >nul
exit /b %errorlevel%
