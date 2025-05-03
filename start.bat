@echo off
set CLIENT_ID=
set SECRET_KEY=
set LYRICS_API_ENDPOINT=http://localhost:8000
cmake --build build
.\build\bin\Debug\lyric-overlay.exe