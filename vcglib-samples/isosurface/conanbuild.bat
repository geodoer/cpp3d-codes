if "%~1"=="" (
    ECHO "Please Enter CMAKE GENERATOR"
)
else (
    @ECHO ON

    SET CMAKE_GENERATOR="%~1"
    ECHO "CMAKE GENERATOR = %CMAKE_GENERATOR%"

    RMDIR /Q /S build
    MKDIR build
    PUSHD build

    conan install ..
    cmake .. -G %CMAKE_GENERATOR%
    cmake --build . --config Release

    pause
)
