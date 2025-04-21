@echo off
set CLIENT_ID=
set SECRET_KEY=
cmake -B build -S . "-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
.\build\bin\Debug\lyric-overlay.exe