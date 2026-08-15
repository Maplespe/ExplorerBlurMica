# BuildAndRegister.ps1
# Build the companion fix DLL with mingw-w64 g++ and register it for the current user.
# No admin required. Restarts explorer.exe to load the DLL.
#
# Usage:  powershell -ExecutionPolicy Bypass -File BuildAndRegister.ps1

$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host '== build =='
$gxx = (Get-Command g++ -ErrorAction SilentlyContinue)
if (-not $gxx) { throw 'g++ not found. Install mingw-w64 first.' }
& g++ -shared -O2 -DUNICODE -D_UNICODE -static-libgcc `
    (Join-Path $here 'FixExplorerBlurMica.c') `
    -o (Join-Path $here 'FixExplorerBlurMica.dll') `
    -lole32 -lshlwapi -ldwmapi -luuid
if ($LASTEXITCODE -ne 0) { throw "g++ failed with exit code $LASTEXITCODE" }
$dll = Join-Path $here 'FixExplorerBlurMica.dll'
Write-Host "built: $dll"

# Optional: copy EBM config so effect=... is honored. Default effect=1 if absent.
$ebmConfig = "$env:USERPROFILE\Downloads\Release_x64\Release\config.ini"
if (-not (Test-Path (Join-Path $here 'config.ini')) -and (Test-Path $ebmConfig)) {
    Copy-Item $ebmConfig (Join-Path $here 'config.ini') -Force
    Write-Host 'config.ini copied'
}

Write-Host '== register (HKCU, no admin) =='
$guid = '{48E13E56-7F01-4B8A-9C32-5A0E3F0D2B71}'
$base = 'HKCU:\SOFTWARE\Classes'
New-Item -Path "$base\CLSID\$guid" -Force | Out-Null
New-Item -Path "$base\CLSID\$guid\InprocServer32" -Force | Out-Null
Set-ItemProperty -Path "$base\CLSID\$guid\InprocServer32" -Name '(default)' -Value $dll
Set-ItemProperty -Path "$base\CLSID\$guid\InprocServer32" -Name 'ThreadingModel' -Value 'Apartment'
New-Item -Path "$base\Drive\shellex\FolderExtensions\$guid" -Force | Out-Null
Set-ItemProperty -Path "$base\Drive\shellex\FolderExtensions\$guid" -Name 'DriveMask' -Value ([int]255) -Type DWord
Write-Host 'registered'

Write-Host '== restart explorer =='
Stop-Process -Name explorer -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1
Start-Process explorer.exe
Start-Sleep -Seconds 6
$proc = Get-Process explorer | Select-Object -First 1
$mod = $proc.Modules | Where-Object ModuleName -eq 'FixExplorerBlurMica.dll'
if ($mod) { Write-Host "loaded: $($mod.FileName)" } else { Write-Host 'not loaded yet - open a folder window' }
