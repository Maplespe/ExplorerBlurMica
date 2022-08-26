@echo off

call :isAdmin
if %errorlevel% == 0 (
	regsvr32 /u "%~dp0ExplorerBlurMica.dll"
	taskkill /f /im explorer.exe & explorer.exe
) else (
	echo 请以管理员身份运行! 
	echo Please run as Administrator
)

pause >nul
exit /b

:isAdmin
fsutil dirty query %systemdrive% >nul
exit /b