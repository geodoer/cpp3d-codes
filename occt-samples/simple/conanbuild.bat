rd /s /q build
mkdir build
cd build
conan install ..
cmake .. -G "Visual Studio 15 Win64"
pause