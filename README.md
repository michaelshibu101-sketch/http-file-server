# HTTP File Server

A cross-platform C++17 multi-threaded static file server. It serves files from a `public/` directory and handles multiple client requests using a thread pool.

## Why this is a strong resume project
- networking with raw sockets
- concurrent request handling with a thread pool
- cross-platform engineering for Windows and Linux
- file serving and HTTP response parsing

## Features
- serves HTML, CSS, JS, text, JSON, PNG, JPG, and SVG files
- thread pool for concurrent clients
- path sanitization to block simple directory traversal attempts
- console logging for each request
- works on Windows with Winsock and on Linux with BSD sockets

## Build on Windows (MinGW g++)
Open PowerShell inside this folder and run:

```powershell
g++ -std=c++17 -O2 -pthread server.cpp -o http_server.exe -lws2_32
.\http_server.exe
```

Or use:

```powershell
build.bat
.\build\http_server.exe
```

Then open this in your browser:

```text
http://127.0.0.1:8080
```

## Build on Linux
```bash
g++ -std=c++17 -O2 -pthread server.cpp -o http_server
./http_server
```

## Custom port and folder
```powershell
.\http_server.exe 9000 public
```

## Suggested resume bullets
- Built a cross-platform HTTP file server in C++17 using raw sockets, thread pooling, and file-system based routing.
- Implemented concurrent request handling, response serialization, and basic path sanitization for safer file serving.
- Designed the project to compile on both Windows and Linux by abstracting platform-specific socket APIs.
