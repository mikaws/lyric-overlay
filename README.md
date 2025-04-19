# Lyric Overlay

**A simple C++ desktop application for displaying real-time lyrics over your screen.**

## About
LyricOverlay is a lightweight program that fetches the currently playing track from **Spotify** and displays the lyrics as an overlay. It runs in the background and stays on top of other applications. There's only support for Windows, but the project could be cross-plataform in the future.

## Features
- **Spotify Integration** – Fetches the current song in real-time (1s).
- **Lyrics Display** – Shows lyrics in a transparent overlay.

## Dependencies
- **C++** – Core language
- **SFML** – Handles window rendering and text display
- **Spotify Web API** – Retrieves current track information
- **Lyrics API** – Fetches lyrics for the current track (still undefined)

## Installation

### 1. Install Dependencies
- Install **SFML**
- Get **Spotify API credentials**
- (Optional) Get a Lyrics API key

### 2. Clone the Repository
```bash
git clone https://github.com/yourusername/LyricOverlay.git
cd LyricOverlay
```

### 3. Build & Run (Windows)

Clone vcpkg and install curl and openssl
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg && ./bootstrap-vcpkg.bat
./vcpkg.exe install curl:x64-windows
./vcpkg.exe openssl:x64-windows
```
Add your spotify credentials in the start.bat 

```bash
set CLIENT_ID=your-client-id
set SECRET_KEY=your-secret-key
```

Compile using CMake
```bash
cmake -B build -S . "-DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake"
.\start.bat
```

## Roadmap
-
- **Minimal UI** – Simple and non-intrusive.
- **Spotify Integration**

## Contributing
Contributions are welcome. Open an issue or submit a pull request.

## License
GPL 3.0 - A free software

---
