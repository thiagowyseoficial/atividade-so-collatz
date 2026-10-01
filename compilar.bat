@echo off
setlocal
cd /d "%~dp0"
if not exist evidencias mkdir evidencias
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo Visual Studio ou Build Tools com C++ nao encontrado.
  exit /b 1
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT exit /b 1
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" > evidencias\compilacao.txt 2>&1
if errorlevel 1 goto falha
echo COMANDO: cl /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 produtor.cpp /Fe:produtor.exe >> evidencias\compilacao.txt
cl /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 produtor.cpp /Fe:produtor.exe >> evidencias\compilacao.txt 2>&1
if errorlevel 1 goto falha
echo COMANDO: cl /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 consumidor.cpp /Fe:consumidor.exe >> evidencias\compilacao.txt
cl /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 consumidor.cpp /Fe:consumidor.exe >> evidencias\compilacao.txt 2>&1
if errorlevel 1 goto falha
echo RESULTADO: ambos os programas compilados; codigo 0. >> evidencias\compilacao.txt
type evidencias\compilacao.txt
exit /b 0
:falha
type evidencias\compilacao.txt
exit /b 1
