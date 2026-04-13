@echo off

:label
set CONFIG=

echo Choisis la configuration :
echo 1 - Debug
echo 2 - Release
echo 3 - Exit
set /p choice=Ton choix (1 ou 2) :

if "%choice%"=="1" set CONFIG=Debug
if "%choice%"=="2" set CONFIG=Release
if "%choice%" == "3" (
    IF EXIST build (
        echo Deleting the build folder...
        rmdir /s /q build
    )
    exit /b
)

if not defined CONFIG (
echo Choix invalide !
pause
cls
goto label
)

IF EXIST build (
echo Deleting the build folder...
rmdir /s /q build
)

echo Creating the build folder...
mkdir build

cd build

cmake -G "Visual Studio 18 2026" ..
cmake --build . --config %CONFIG%
ctest --output-on-failure -C %CONFIG%

pause

cd ..
cls
goto label
