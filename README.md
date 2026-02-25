# Pyrite64 - macOS Edition

<p align="center">
<img src="./data/img/titleLogo.png" width="400">
</p>

<p align="center">
N64 game-engine and editor using <a href="https://github.com/DragonMinded/libdragon">Libdragon</a> and <a href="https://github.com/HailToDodongo/tiny3d">tiny3d</a>.<br/>
<strong>macOS (Apple Silicon) port</strong> of <a href="https://github.com/HailToDodongo/pyrite64">HailToDodongo/pyrite64</a>.
</p>
<br/>

<p align="center">
  <img src="./docs/img/editor00.png" width="350">&nbsp;&nbsp;&nbsp;&nbsp;
  <img src="./docs/img/editor02.png" width="350">
</p>
<p align="center">
  <img src="./docs/img/editor01.png" width="350">&nbsp;&nbsp;&nbsp;&nbsp;
  <img src="./docs/img/editor03.jpg" width="350">
</p>

> Note: This project does NOT use any proprietary N64 SDKs or libraries.

> [!IMPORTANT]
> **This fork targets Apple Silicon (M1/M2/M3/M4) Macs only.** Intel Macs are untested and unlikely to work without modifications.

Pyrite64 is a visual editor + runtime-engine to create 3D games that can run on a real N64 console or accurate emulators.

### Features

- Automatic toolchain installation on **Windows and macOS**
- 3D-Model import (GLTF) from Blender with [fast64](https://github.com/Fast-64/fast64) material support
- Support for HDR+Bloom rendering ([demo](https://www.youtube.com/watch?v=XP8g2ngHftY))
- Support for big-texture rendering 256x256 ([demo](https://www.youtube.com/watch?v=rNEo0aQkGnU))
- Runtime engine handling scene-management, rendering, collision, audio and more
- Global asset management with automatic memory cleanup
- Node-Graph editor to script basic control flow

Note that this project focuses on real hardware, so accurate emulation is required to run/test games on PC.
Emulators that are accurate enough include [Ares (v147 or newer)](https://ares-emu.net/) and [gopher64](https://github.com/gopher64/gopher64).

> [!WARNING]
> This project is still in early development, so features are going to be missing.<br>
> Documentation is also still a work in progress, and breaking API changes are to be expected.

---

### macOS-Specific Additions

This fork adds the following on top of the upstream project:

| Feature | Details |
|---------|---------|
| Toolchain install | One-click Install via Homebrew + Terminal.app (~30-60 min build from source) |
| Toolchain Manager | Detects Homebrew, toolchain, libdragon, tiny3d, and ares across 5 steps |
| Emulator | Auto-installs [ares](https://ares-emu.net/) via Homebrew, launches with `open -a ares` |
| SDK path | Defaults to `$HOME/pyrite64-sdk` (no sudo, auto-discovered without `N64_INST`) |
| App bundle | Pre-built `.app` with ad-hoc code signing via `build_app.sh` |

---

## Requirements

- **macOS on Apple Silicon** (M1 or later)
- **Homebrew** ([brew.sh](https://brew.sh))
- **Full Xcode** (from the App Store) — Command Line Tools alone are not sufficient, as the CLT ships with incomplete C++23/libc++ headers needed for building

## Quick Start

### Option A: Use the pre-built .app

1. Download or clone this repo
2. Right-click `Pyrite64.app` > **Open** (first launch only — see Gatekeeper note below)
3. Open the **Toolchain Manager** and click **Install**
4. Wait for the Terminal window to finish (~30-60 minutes)
5. Restart Pyrite64 — all 5 steps should be green

### Option B: Build from source

```bash
brew install cmake ninja
cmake --preset macos
./build_app.sh
```

Then launch `Pyrite64.app`. No Gatekeeper issues when building locally.

### Gatekeeper Note

If you **downloaded** the `.app` from GitHub, macOS will block it on first launch since it isn't notarized.

**On macOS Sequoia (15) and newer:**
1. Try to open the app — macOS will block it
2. Go to **System Settings** > **Privacy & Security**, scroll down and click **Open Anyway**
3. Click **Open Anyway** again in the confirmation dialog

**On older macOS versions:**
- **Right-click** the .app > **Open** > click **Open** in the dialog

**Alternatively**, run this in Terminal to skip Gatekeeper entirely:
```bash
xattr -cr Pyrite64.app
```

This only applies to downloaded builds, not when building from source. Only needed once.

## Documentation

Before starting, please read the [FAQ](./docs/faq.md)!

- [Pyrite64 Installation](./docs/setup_editor.md)
- [Using the Editor](./docs/usage_editor.md)
- [Using the CLI](./docs/usage_editor.md)
- [Building the Editor](./docs/build_editor.md)

## Showcase

<p align="center">
<a href="https://www.youtube.com/watch?v=zz_wByA_k6E" target="_blank">
    <img src="https://img.youtube.com/vi/zz_wByA_k6E/0.jpg" width="250">
</a>
<a href="https://www.youtube.com/watch?v=4BCmKnN5eGA" target="_blank">
    <img src="https://img.youtube.com/vi/4BCmKnN5eGA/0.jpg" width="250">
</a>
<br/>
Cathode Quest 64 (YouTube) &nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp; Pyrite64 Release Video
</p>

## Links

- N64Brew Discord: https://discord.gg/WqFgNWf
- Upstream project: [HailToDodongo/pyrite64](https://github.com/HailToDodongo/pyrite64)

## Credits & License

Original project (c) 2025-2026 - Max Bebök ([HailToDodongo](https://github.com/HailToDodongo))<br>
macOS port by [proverbiallemon](https://github.com/proverbiallemon)

Pyrite64 is licensed under the MIT License, see the [LICENSE](LICENSE) file for more information.<br>
Licenses for external libraries used in the editor can be found in their respective directory under `/vendored`.

Pyrite64 does NOT force any restrictions or licenses on games made with it.<br>
Pyrite64 does NOT claim any copyright or force licenses for assets / source-code generated by the editor.

While not required, please consider crediting Pyrite64 with a logo and/or name in your credits and/or boot logo sequence.
