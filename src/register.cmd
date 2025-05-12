@echo off
setlocal EnableDelayedExpansion

:: Initialize variables and logging
set "LOGFILE=%TEMP%\ApplyEffects_%DATE:~-4%%DATE:~4,2%%DATE:~7,2%.log"
echo [%DATE% %TIME%] Script started >> "%LOGFILE%"
set "SCRIPT_DIR=%~dp0"
set "DLL_NAME=ExplorerBlurMica.dll"
set "DLL_PATH=%SCRIPT_DIR%%DLL_NAME%"

:: Set console appearance
title Requesting Administrator Access
mode CON COLS=40 LINES=5
color F0
cls
echo :::::::::::::::::::::::::::::::::::::::
echo :: Requesting Administrator Privileges ::
echo :::::::::::::::::::::::::::::::::::::::

:: Check if DLL exists
if not exist "%DLL_PATH%" (
    echo [%DATE% %TIME%] ERROR: %DLL_NAME% not found >> "%LOGFILE%"
    echo ERROR: %DLL_NAME% not found in script directory
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

:: Request admin privileges
cd /d "%SCRIPT_DIR%" || (
    echo [%DATE% %TIME%] ERROR: Failed to change directory to %SCRIPT_DIR% >> "%LOGFILE%"
    exit /b 1
)
if exist "%TEMP%\getadmin.vbs" del "%TEMP%\getadmin.vbs"
fsutil dirty query %systemdrive% >nul 2>&1
if %errorlevel% neq 0 (
    echo [%DATE% %TIME%] Requesting admin privileges >> "%LOGFILE%"
    cmd /u /c echo Set UAC = CreateObject^("Shell.Application"^) : UAC.ShellExecute "cmd.exe", "/k cd ""%~sdp0"" && ""%~s0""", "", "runas", 1 >> "%TEMP%\getadmin.vbs"
    if exist "%TEMP%\getadmin.vbs" (
        "%TEMP%\getadmin.vbs"
        del "%TEMP%\getadmin.vbs"
        exit /b
    ) else (
        echo [%DATE% %TIME%] ERROR: Failed to create VBS script for elevation >> "%LOGFILE%"
        echo ERROR: Failed to create elevation script
        pause
        exit /b 1
    )
)

:: Main execution (admin privileges confirmed)
title Applying Effects
cls
echo Applying visual effects...
echo [%DATE% %TIME%] Running as administrator >> "%LOGFILE%"

:: Register DLL
echo Registering %DLL_NAME%...
regsvr32 /s "%DLL_PATH%"
if %errorlevel% neq 0 (
    echo [%DATE% %TIME%] ERROR: Failed to register %DLL_NAME% >> "%LOGFILE%"
    echo ERROR: Failed to register %DLL_NAME%
    echo Press any key to exit...
    pause >nul
    exit /b 1
)
echo [%DATE% %TIME%] Successfully registered %DLL_NAME% >> "%LOGFILE%"

:: Restart Explorer
echo Restarting Windows Explorer...
taskkill /F /IM explorer.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo [%DATE% %TIME%] WARNING: Failed to terminate explorer.exe >> "%LOGFILE%"
)
start "" explorer.exe
if %errorlevel% neq 0 (
    echo [%DATE% %TIME%] WARNING: Failed to restart explorer.exe >> "%LOGFILE%"
)
echo [%DATE% %TIME%] Explorer restarted >> "%LOGFILE%"

:: Final message
title Success
cls
color 2F
echo :::::::::::::::::::::::::::::::::::::::
echo ::      Changes Applied Successfully   ::
echo :::::::::::::::::::::::::::::::::::::::
echo Changes have been applied successfully
echo Log file: %LOGFILE%
echo Window will close in 5 seconds...
echo [%DATE% %TIME%] Script completed successfully >> "%LOGFILE%"
timeout /t 5 >nul 2>&1
exit /b 0
