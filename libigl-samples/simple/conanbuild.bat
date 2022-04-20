rd /s /q build
mkdir build
cd build
conan install .. -s compiler.version=16
cmake .. -G "Visual Studio 16"
pause