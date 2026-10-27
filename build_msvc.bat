@echo off
echo [1/3] Searching for Visual Studio installation...
set "VS_WHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VS_WHERE%" (
    echo [ERROR] Visual Studio installer not found at %VS_WHERE%
    echo Please install Visual Studio 2022 / 2019 with "Desktop development with C++".
    pause
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VS_WHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set "MSBUILD_PATH=%%i"
)

echo [2/3] Compiling DJMugumyaProSoundEnhancer.sln (Release | x64)...
"%MSBUILD_PATH%" DJMugumyaProSoundEnhancer.sln /p:Configuration=Release /p:Platform=x64 /m
if %ERRORLEVEL% neq 0 (
    echo [FAILED] Compilation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] BUILD SUCCESSFUL!
echo Output DLL: bin\x64\Release\DJMugumyaProSoundEnhancer64.dll
echo Copy this to: %USERPROFILE%\Documents\VirtualDJ\Plugins64\SoundEffect\
pause