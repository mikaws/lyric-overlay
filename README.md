# Lyric Overlay

**A simple C++ desktop application for displaying real-time lyrics over your screen.**

## About
LyricOverlay is a lightweight program that fetches the currently playing track from **Spotify** and displays the lyrics as an overlay. It runs in the background and stays on top of other applications.

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

### 3. Build & Run
Compile using your preferred C++ build system. Example with CMake:
```bash
cmake -B build
cmake --build build
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
