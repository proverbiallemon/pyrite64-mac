# Memory Budget Dashboard — Design

**Date:** 2026-02-23
**Status:** Approved

## Summary

A dockable ImGui panel in the bottom dock (alongside Files and Log) that shows ROM cartridge space usage. Scans compiled build outputs in `filesystem/` after each build to provide accurate size data.

## Decisions

- **Data source:** Post-build scan of `filesystem/` directory (accurate, not estimated)
- **Budget scope:** ROM/cartridge size only (no runtime RAM estimation for now)
- **Panel location:** Bottom dock tab alongside Files and Log
- **Visual style:** Stacked color-coded bar + sortable asset table
- **Cart size limit:** Configurable dropdown (8, 16, 32, 64 MB)

## UI Layout

```
┌─────────────────────────────────────────────────┐
│ Memory Budget          Cart: [32 MB ▼] │ Refresh │
├─────────────────────────────────────────────────┤
│ ROM: 2.4 MB / 32 MB  [7.5%]                    │
│ ██████▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ │
│  ■ Textures 1.2MB  ■ Models 800KB  ■ Audio 300K│
├─────────────────────────────────────────────────┤
│ Type    │ Name           │ Size   │ Compression  │
│─────────┼────────────────┼────────┼──────────────│
│ Texture │ player.png     │ 128 KB │ Level 2      │
│ Texture │ tileset.png    │  96 KB │ Level 1      │
│ Model   │ level1.glb     │ 256 KB │ Level 1      │
│ Audio   │ bgm_main.wav   │  96 KB │ Level 3      │
│ Font    │ default.ttf    │  48 KB │ None         │
│ Scene   │ scene_main     │  12 KB │ —            │
│ Code    │ (binary)       │ 180 KB │ —            │
└─────────────────────────────────────────────────┘
```

## Architecture

- **New files:** `src/editor/pages/parts/memoryDashboard.h` and `.cpp`
- **Class:** `MemoryDashboard` with `draw()` and `refresh()` methods
- **Registration:** Added as member of `Editor::Scene`, drawn in bottom dock layout
- **Data flow:** Scan `filesystem/` recursively for file sizes, cross-reference with `AssetManager` entries for type/name/compression metadata
- **ROM file:** Also check final `.z64` file size for the total ROM measurement
- **Color coding:** Textures=blue, Models=green, Audio=orange, Fonts=purple, Scenes=yellow, Code=gray
- **Table:** Sortable by clicking column headers (type, name, size, compression)
- **Refresh:** Manual button + auto-refresh after build completes

## Out of Scope

- Runtime RAM estimation
- Per-scene memory breakdown
- Real-time estimation before builds
- TMEM (4KB texture cache) analysis
- Expansion pak detection

## Future Enhancements

- RAM budget estimation per scene
- TMEM usage analysis per material
- Warnings when approaching budget limits
- Compression ratio comparison (original vs compiled)
