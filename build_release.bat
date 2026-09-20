@echo off
setlocal

set "ROOT=%~dp0"
set "MSBUILD=C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"
set "SDL_PROJECT=%ROOT%third_party\SDL-release-3.4.16\VisualC\SDL\SDL.vcxproj"
set "SDL_OUTPUT=%ROOT%third_party\SDL-static"
set "DIST=%ROOT%dist"

if not exist "%MSBUILD%" (
    echo MSBuild not found.
    exit /b 1
)

echo [1/2] Building static SDL3...
"%MSBUILD%" "%SDL_PROJECT%" /t:Rebuild /m /p:Configuration=Release /p:Platform=x64 /p:ConfigurationType=StaticLibrary /p:TargetName=SDL3-static /verbosity:minimal
if errorlevel 1 exit /b 1
if not exist "%SDL_OUTPUT%" mkdir "%SDL_OUTPUT%"
copy /Y "%ROOT%third_party\SDL-release-3.4.16\VisualC\SDL\x64\Release\SDL3-static.lib" "%SDL_OUTPUT%\SDL3-static.lib" >nul

echo [2/2] Building LEAK...
"%MSBUILD%" "%ROOT%Injector.vcxproj" /t:Rebuild /m /p:Configuration=Release /p:Platform=x64 /verbosity:minimal
if errorlevel 1 exit /b 1

if not exist "%DIST%" mkdir "%DIST%"
copy /Y "%ROOT%x64\Release\Injector.exe" "%DIST%\LEAK.exe" >nul

echo.
echo Single-file build ready:
echo %DIST%\LEAK.exe
pause
