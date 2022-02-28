# NukeFocusStealer - Halt programs demanding Window Focus!
[![License: GPL v3 or later](https://img.shields.io/badge/License-GPL%20v3%20or%20later-blue.svg)](https://github.com/RadWolfie/NukeFocusStealer/blob/main/LICENSE)
[![GitHub Actions](https://github.com/RadWolfie/NukeFocusStealer/workflows/GitHub%20CI/badge.svg?event=push)](https://github.com/RadWolfie/NukeFocusStealer/actions?query=event%3Apush+workflow%3A%22GitHub+CI%22)

NukeFocusStealer is a software to prevent programs request window focus whenever user doesn't want it to.

## System Requirements
### Minimum
  * OS: Windows 7+ x64. 32-bit can be built locally.
    * MacOS and Linux are not supported at this time.
  * GPU: OpenGL 3.0

## Contributing
We welcome contributions, large and small.

### Main Prerequisites
1. [Git for Windows](https://git-scm.com)
2. [CMake](https://cmake.org)
    * Some IDEs already have CMake support, this is optional.

### Fetching the code
1. Run the following command in the command line:
<br>`git clone --recurse-submodules https://github.com/RadWolfie/NukeFocusStealer.git`
    * Please note the `--recurse-submodules` parameter. This is required to fetch submodules.
      * If NukeFocusStealer was checked out without submodules, they can be updated/fetched with the following command:

        `git submodule update --init --recursive`

### Compiling

#### Windows
Don't open `CMakeLists.txt` from Visual Studio, as it won't generate files in the `build` directory.

##### Prerequisites
1. [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2022
    * C++ development
    * Windows Universal CRT SDK
    * C++ CMake tools for Windows
      * *Optional if CMake is installed*

##### Generate Visual Studio files
1. If you don't have CMake installed, open `___ Native Tools Command Prompt for VS 20##`.
2. `cd` to the NukeFocusStealer directory.
3. Run these commands.
    1. `mkdir build & cd build`
    2. `cmake .. -G "Visual Studio 17 2022" -A Win32`
4. Open `NukeFocusStealer.sln` from the `build` directory.
5. Select any configuration, then click Build.

#### Linux / macOS
Currently not supported.

## Special Thanks
* [SDL2](https://github.com/libsdl-org/SDL) - Cross platform development library
* [Dear ImGui](https://github.com/ocornut/imgui) - bloat-free graphical user interface library
* [Noto Sans Fonts](https://fonts.google.com/noto/specimen/Noto+Sans) - multi-languages support
