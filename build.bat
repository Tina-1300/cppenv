@echo off

:label

IF EXIST build (
    echo Deleting the build folder...
    rmdir /s /q build
    pause
)


echo Creating the build folder...
mkdir build

cd build
cmake -G "Visual Studio 18 2026" ..
cmake --build . --config Debug
ctest --output-on-failure -C Debug

pause

cd ../
cls

goto label

