@echo off
setlocal
if not exist build mkdir build
g++ -std=c++17 -O2 -pthread server.cpp -o build\http_server.exe -lws2_32
if %errorlevel% neq 0 exit /b %errorlevel%
echo Build complete: build\http_server.exe
