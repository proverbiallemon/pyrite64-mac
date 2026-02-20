#!/bin/bash
# macOS toolchain install script for Pyrite64
# Builds mips64-elf-gcc from source via libdragon's build-toolchain.sh,
# then builds libdragon (preview branch) and tiny3d.

set -euo pipefail
IFS=$'\n\t'

sdkpath="${N64_INST:-$HOME/pyrite64-sdk}"
workpath="$HOME/pyrite64-tmp"

export N64_INST="$sdkpath"

# Check for Homebrew
if ! command -v brew &>/dev/null; then
    echo "ERROR: Homebrew is required but not installed."
    echo "Install it from https://brew.sh and try again."
    exit 1
fi

echo "=== Pyrite64 macOS Toolchain Installer ==="
echo "Install path: $sdkpath"
echo "Work path:    $workpath"
echo ""

# Install build dependencies
echo "Installing build dependencies via Homebrew..."
brew install -q gmp mpfr libmpc gsed isl make python3 texinfo ninja

mkdir -p "$workpath"
cd "$workpath"

# Get build-toolchain.sh from libdragon (use local clone if available, otherwise download)
BUILD_SCRIPT="$workpath/build-toolchain.sh"
if [ -e "$workpath/libdragon/tools/build-toolchain.sh" ]; then
    cp "$workpath/libdragon/tools/build-toolchain.sh" "$BUILD_SCRIPT"
else
    echo "Downloading build-toolchain.sh from libdragon preview branch..."
    curl -fsSL "https://raw.githubusercontent.com/DragonMinded/libdragon/preview/tools/build-toolchain.sh" -o "$BUILD_SCRIPT"
fi
chmod +x "$BUILD_SCRIPT"

# Build the cross-compiler toolchain from source
echo ""
echo "=== Building mips64-elf toolchain from source ==="
echo "This will take 30-60 minutes..."
echo ""

export BUILD_PATH="$workpath/toolchain"
export DOWNLOAD_PATH="$workpath/toolchain"
mkdir -p "$BUILD_PATH"

bash "$BUILD_SCRIPT"

# Verify toolchain was built
if [ ! -x "$sdkpath/bin/mips64-elf-gcc" ]; then
    echo "ERROR: Toolchain build failed. mips64-elf-gcc not found."
    exit 1
fi

echo ""
echo "=== Toolchain build complete ==="
echo ""

export PATH="$sdkpath/bin:$PATH"

# Build libdragon
echo "=== Building libdragon (preview branch) ==="

if [ -e "libdragon" ]; then
    echo "Libdragon directory exists, updating..."
    cd libdragon
    git checkout preview
    git pull
else
    git clone -b preview https://github.com/DragonMinded/libdragon.git
    cd libdragon
fi

if [[ ! -f "$sdkpath/bin/n64tool" || "${FORCE_UPDATE:-}" == "true" ]]; then
    make clean && make -C tools clean
    make -j"$(sysctl -n hw.ncpu)" libdragon && make -j"$(sysctl -n hw.ncpu)" tools
    make install
    make -C tools install
else
    echo "Libdragon already installed"
fi

cd ..

# Build Tiny3D
echo ""
echo "=== Building Tiny3D ==="

if [ -e "tiny3d" ]; then
    echo "Tiny3D directory exists, updating..."
    cd tiny3d
    git pull
else
    git clone https://github.com/HailToDodongo/tiny3d.git
    cd tiny3d
fi

if [[ ! -f "$sdkpath/bin/gltf_to_t3d" || "${FORCE_UPDATE:-}" == "true" ]]; then
    make clean
    make -j"$(sysctl -n hw.ncpu)"
    make install
else
    echo "Tiny3D already installed"
fi

# Build gltf_importer tool
echo "Building gltf_importer..."
make -C tools/gltf_importer -j"$(sysctl -n hw.ncpu)"
make -C tools/gltf_importer install

cd ..

# Install ares N64 emulator
echo ""
echo "=== Installing ares emulator ==="
if brew list --cask ares-emulator &>/dev/null; then
    echo "ares is already installed"
else
    brew install --cask ares-emulator
fi

echo ""
echo "==========================================="
echo "  Pyrite64 toolchain installation complete!"
echo "  Installed to: $sdkpath"
echo "==========================================="
echo ""
echo "You can close this window and restart Pyrite64."
