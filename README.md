# Win32 Map Editor

A 2D map editor for Windows developed in **C++23** using the **Win32 API** and **SDL3**.

The editor is designed for creating and editing tile-based maps. It is made for in parallel with my own sdl based sdl-game-core library which is an SDL based rendering engine and game engine.


# Building

## Requirements

* **CMake 3.20 or newer**
* A C++ compiler with **C++23** support
* **SDL3**
* **SDL3_image**
* **sdl-game-core**
* **nlohmann/json** — downloaded automatically by CMake using `FetchContent`
* Windows, as the editor uses the Win32 API

The project is currently configured for building with **MSVC** and uses the following compiler options:

* `/W4`
* `/permissive-`

## Dependencies

The paths to SDL3, SDL3_image, and `sdl-game-core` are provided to CMake through environment variables:

```text
SGC_INCLUDE_PATH
SGC_LIB_PATH
SDL_INCLUDE_PATH
SDL_LIB_PATH
SDL_IMAGE_LIB_PATH
```

For example, on Windows:

```powershell
$env:SGC_INCLUDE_PATH="C:\Libraries\sdl-game-core\include"
$env:SGC_LIB_PATH="C:\Libraries\sdl-game-core\lib"
$env:SDL_INCLUDE_PATH="C:\Libraries\SDL3\include"
$env:SDL_LIB_PATH="C:\Libraries\SDL3\lib"
$env:SDL_IMAGE_LIB_PATH="C:\Libraries\SDL3_image\lib"
```

The exact paths will depend on where the dependencies are installed.

## Configure and Build

Clone the repository and create a build directory:

```powershell
git clone <repository-url>
cd sgc-editor

cmake -S . -B build
cmake --build build --config Release
```

The executable will be generated in the configuration-specific build directory, for example:

```text
build/Release/sgc-editor.exe
```

### Visual Studio

CMake can also generate a Visual Studio solution:

```powershell
cmake -S . -B build -G "Visual Studio 18 2026"
```

The generated solution can then be opened in Visual Studio and built normally.

## Runtime DLLs

Any `.dll` files placed in the project's `dlls` directory are automatically copied next to the executable during the build.

This can be used to provide the required SDL3 and other runtime DLLs without manually copying them after every build.
