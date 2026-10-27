# ============================================================================
# DJ Mugumya Pro Sound Enhancer - PowerShell 1-Click Build Script
# ============================================================================

Write-Host "Compiling DJ Mugumya Pro Sound Enhancer..." -ForegroundColor Cyan
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Host "[ERROR] Visual Studio installer not found at: $vswhere" -ForegroundColor Red
    Pause; Exit 1
}

$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if (-not $msbuild) {
    Write-Host "[ERROR] MSBuild was not found." -ForegroundColor Red
    Pause; Exit 1
}

$slnPath = Join-Path $PSScriptRoot "DJMugumyaProSoundEnhancer.sln"
& $msbuild $slnPath /p:Configuration=Release /p:Platform=x64 /m

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nBUILD SUCCESSFUL! Output: bin\x64\Release\DJMugumyaProSoundEnhancer64.dll" -ForegroundColor Green
} else {
    Write-Host "`nBUILD FAILED." -ForegroundColor Red
}
Pause