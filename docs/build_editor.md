# Building the Editor

Below are instructions to build the editor on linux, windows, or macOS.<br/>
Note that due to a small dependency on libdragon, GCC is required on Windows/Linux.<br/>
On Windows, that means building via MSYS2. On macOS, Clang (from Xcode) works natively.

## Prerequisites

Before building the project, make sure you have the following tools installed:
- CMake
- Ninja
- GCC with at least C++23 support (Windows/Linux) or Clang via Xcode Command Line Tools (macOS)

Linux users should follow the conventions of their distribution and package manager.</br>

Windows users need to make sure a recent version of MSYS2 is installed (https://www.msys2.org/).<br>
Open an MSYS2 terminal in the `UCRT64` environment, and install the UCRT-specific packages for the dependencies:
```sh
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-ninja
```

macOS users (Apple Silicon only) need Homebrew and Xcode Command Line Tools:
```sh
xcode-select --install
brew install cmake ninja
```

## Build Instructions

After cloning the `pyrite64` repo, make sure to fetch all the submodules:
```sh
git submodule update --init --recursive
```

To configure the project, run:
```sh
cmake --preset <preset>
```

After that, and for every subsequent build, run:
```sh
cmake --build --preset <preset>
```

Where `<preset>` is replaced with the CMake preset name corresponding to your system:
- `linux` for Linux systems
- `windows-gcc` for Windows systems with MSYS2
- `macos` for macOS (Apple Silicon)

Once the build is finished, a program called `pyrite64` (or `pyrite64.exe`) should be placed in the root directory of the repo.<br>
The program itself can be placed anywhere on the system, however the `./data` and `./n64` directories must stay next to it.

To open the editor, simply execute `./pyrite64` (or `.\pyrite64.exe`).

### macOS .app Bundle

On macOS, you can package the build into a `.app` bundle using the included script:
```sh
./build_app.sh
```
This builds the project, copies the binary and data into `Pyrite64.app`, and ad-hoc signs it.<br>
The signing step is required — macOS will refuse to open the `.app` if the binary is changed without re-signing.

If you built from source, that's all you need. The Gatekeeper quarantine warning (right-click > Open / `xattr -cr`) only applies to pre-built `.app` bundles downloaded from the internet.
