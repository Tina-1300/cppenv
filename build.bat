@echo off
IF EXIST build (
    echo Deleting the build folder...
    rmdir /s /q build
    exit
)


echo Creating the build folder...
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . 
ctest --output-on-failure

