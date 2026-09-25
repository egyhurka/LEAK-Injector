@echo off
setlocal

set "ROOT=%~dp0"
set "MSBUILD=C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"
set "SDL_PROJECT=%ROOT%third_party\SDL-release-3.4.16\VisualC\SDL\SDL.vcxproj"
set "SDL_OUTPUT_X64=%ROOT%third_party\SDL-static"
set "SDL_OUTPUT_X86=%ROOT%third_party\SDL-static-x86"
set "DIST=%ROOT%dist"

if not exist "%MSBUILD%" (
    echo MSBuild not found.
    exit /b 1
)

echo [1/4] Building static SDL3 x64...
"%MSBUILD%" "%SDL_PROJECT%" /t:Rebuild /m /p:Configuration=Release /p:Platform=x64 /p:ConfigurationType=StaticLibrary /p:TargetName=SDL3-static /verbosity:minimal
if errorlevel 1 exit /b 1
if not exist "%SDL_OUTPUT_X64%" mkdir "%SDL_OUTPUT_X64%"
copy /Y "%ROOT%third_party\SDL-release-3.4.16\VisualC\SDL\x64\Release\SDL3-static.lib" "%SDL_OUTPUT_X64%\SDL3-static.lib" >nul

echo [2/4] Building static SDL3 x86...
"%MSBUILD%" "%SDL_PROJECT%" /t:Rebuild /m /p:Configuration=Release /p:Platform=Win32 /p:ConfigurationType=StaticLibrary /p:TargetName=SDL3-static /verbosity:minimal
if errorlevel 1 exit /b 1
if not exist "%SDL_OUTPUT_X86%" mkdir "%SDL_OUTPUT_X86%"
copy /Y "%ROOT%third_party\SDL-release-3.4.16\VisualC\SDL\Win32\Release\SDL3-static.lib" "%SDL_OUTPUT_X86%\SDL3-static.lib" >nul

echo [3/4] Building LEAK x64...
"%MSBUILD%" "%ROOT%Injector.vcxproj" /t:Rebuild /m /p:Configuration=Release /p:Platform=x64 /verbosity:minimal
if errorlevel 1 exit /b 1
if not exist "%DIST%" mkdir "%DIST%"
copy /Y "%ROOT%x64\Release\Injector.exe" "%DIST%\LEAK-x64.exe" >nul

echo [4/4] Building LEAK x86...
"%MSBUILD%" "%ROOT%Injector.vcxproj" /t:Rebuild /m /p:Configuration=Release /p:Platform=Win32 /verbosity:minimal
if errorlevel 1 exit /b 1
copy /Y "%ROOT%Release\Injector.exe" "%DIST%\LEAK-x86.exe" >nul

echo.
echo Builds ready:
echo %DIST%\LEAK-x64.exe
echo %DIST%\LEAK-x86.exe
pause
