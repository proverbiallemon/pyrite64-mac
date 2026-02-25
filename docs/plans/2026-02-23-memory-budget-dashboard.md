# Memory Budget Dashboard Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a dockable "Memory Budget" panel to the editor's bottom dock that shows ROM cartridge space usage by scanning compiled build outputs.

**Architecture:** New `MemoryDashboard` class following the existing panel pattern (like `LogWindow`). After a build, it scans the project's `filesystem/` directory and the final `.z64` ROM to calculate sizes per asset type. Displays a color-coded stacked bar and a sortable table. Cart size limit is configurable via dropdown.

**Tech Stack:** C++, ImGui (docking, tables, drawing), std::filesystem for file size scanning

---

### Task 1: Create the MemoryDashboard header

**Files:**
- Create: `src/editor/pages/parts/memoryDashboard.h`

**Step 1: Write the header file**

```cpp
/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace Editor
{
  class MemoryDashboard
  {
    public:
      void draw();
      void refresh();

    private:
      enum class AssetCategory : int {
        Textures = 0,
        Models,
        Audio,
        Fonts,
        Scenes,
        Code,
        Other,
        _COUNT
      };

      struct AssetEntry {
        std::string name{};
        AssetCategory category{};
        uint64_t sizeBytes{0};
        std::string compression{};
      };

      static constexpr int CART_SIZE_COUNT = 4;
      static constexpr uint64_t CART_SIZES[CART_SIZE_COUNT] = {
        8 * 1024 * 1024,
        16 * 1024 * 1024,
        32 * 1024 * 1024,
        64 * 1024 * 1024,
      };
      static constexpr const char* CART_LABELS[CART_SIZE_COUNT] = {
        "8 MB", "16 MB", "32 MB", "64 MB"
      };

      int selectedCartSize{3}; // default 64 MB
      uint64_t totalRomSize{0};
      uint64_t categoryTotals[static_cast<int>(AssetCategory::_COUNT)]{};
      std::vector<AssetEntry> entries{};
      int sortColumn{2}; // default sort by size
      bool sortAscending{false}; // default descending (largest first)
      bool hasData{false};

      void scanBuildOutputs();
      void sortEntries();
      void drawBudgetBar();
      void drawCategorySummary();
      void drawAssetTable();

      static const char* categoryName(AssetCategory cat);
      static ImVec4 categoryColor(AssetCategory cat);
  };
}
```

**Step 2: Commit**

```bash
git add src/editor/pages/parts/memoryDashboard.h
git commit -m "feat: add MemoryDashboard header with data structures"
```

---

### Task 2: Implement the scan and data collection

**Files:**
- Create: `src/editor/pages/parts/memoryDashboard.cpp`

**Step 1: Write the implementation with scan logic**

```cpp
/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#include "memoryDashboard.h"

#include "imgui.h"
#include "../../../context.h"
#include "../../../utils/string.h"
#include "../../imgui/theme.h"
#include <filesystem>

namespace fs = std::filesystem;

const char* Editor::MemoryDashboard::categoryName(AssetCategory cat)
{
  switch(cat) {
    case AssetCategory::Textures: return "Texture";
    case AssetCategory::Models:   return "Model";
    case AssetCategory::Audio:    return "Audio";
    case AssetCategory::Fonts:    return "Font";
    case AssetCategory::Scenes:   return "Scene";
    case AssetCategory::Code:     return "Code";
    case AssetCategory::Other:    return "Other";
    default: return "Unknown";
  }
}

ImVec4 Editor::MemoryDashboard::categoryColor(AssetCategory cat)
{
  switch(cat) {
    case AssetCategory::Textures: return {0.40f, 0.65f, 0.90f, 1.0f}; // blue
    case AssetCategory::Models:   return {0.45f, 0.78f, 0.45f, 1.0f}; // green
    case AssetCategory::Audio:    return {0.90f, 0.65f, 0.30f, 1.0f}; // orange
    case AssetCategory::Fonts:    return {0.70f, 0.50f, 0.85f, 1.0f}; // purple
    case AssetCategory::Scenes:   return {0.90f, 0.85f, 0.35f, 1.0f}; // yellow
    case AssetCategory::Code:     return {0.60f, 0.60f, 0.60f, 1.0f}; // gray
    case AssetCategory::Other:    return {0.50f, 0.50f, 0.50f, 1.0f}; // dark gray
    default: return {1.0f, 1.0f, 1.0f, 1.0f};
  }
}

void Editor::MemoryDashboard::scanBuildOutputs()
{
  entries.clear();
  totalRomSize = 0;
  for(int i = 0; i < static_cast<int>(AssetCategory::_COUNT); ++i) {
    categoryTotals[i] = 0;
  }

  if(!ctx.project) return;

  auto projectPath = fs::path{ctx.project->getPath()};
  auto fsPath = projectPath / "filesystem";

  // Check for the final .z64 ROM to get total size
  auto romPath = projectPath / (ctx.project->conf.romName + ".z64");
  if(fs::exists(romPath)) {
    totalRomSize = fs::file_size(romPath);
  }

  if(!fs::exists(fsPath)) return;

  // Cross-reference with asset manager for metadata
  auto &assets = ctx.project->getAssets();

  // Scan all files in filesystem/ recursively
  for(auto &dirEntry : fs::recursive_directory_iterator(fsPath)) {
    if(!dirEntry.is_regular_file()) continue;

    auto relPath = fs::relative(dirEntry.path(), fsPath).string();
    auto fileSize = dirEntry.file_size();

    // Determine category by cross-referencing with asset manager
    AssetCategory cat = AssetCategory::Other;
    std::string displayName = relPath;
    std::string comprStr = "-";

    // Try to match against known assets
    auto &allEntries = assets.getEntries();
    bool matched = false;
    for(int typeIdx = 0; typeIdx < static_cast<int>(Project::FileType::_SIZE); ++typeIdx) {
      for(auto &asset : allEntries[typeIdx]) {
        // Check if this filesystem file matches the asset's output path
        if(!asset.outPath.empty() && relPath.find(asset.outPath.substr(asset.outPath.find('/') + 1)) != std::string::npos) {
          displayName = asset.name;
          matched = true;

          switch(asset.type) {
            case Project::FileType::IMAGE:       cat = AssetCategory::Textures; break;
            case Project::FileType::MODEL_3D:    cat = AssetCategory::Models; break;
            case Project::FileType::AUDIO:       cat = AssetCategory::Audio; break;
            case Project::FileType::FONT:        cat = AssetCategory::Fonts; break;
            case Project::FileType::CODE_OBJ:
            case Project::FileType::CODE_GLOBAL:
            case Project::FileType::NODE_GRAPH:  cat = AssetCategory::Code; break;
            case Project::FileType::PREFAB:      cat = AssetCategory::Other; break;
            default: break;
          }

          // Compression label
          switch(asset.conf.compression) {
            case Project::ComprTypes::DEFAULT: comprStr = "Default"; break;
            case Project::ComprTypes::LEVEL_0: comprStr = "None"; break;
            case Project::ComprTypes::LEVEL_1: comprStr = "Level 1"; break;
            case Project::ComprTypes::LEVEL_2: comprStr = "Level 2"; break;
            case Project::ComprTypes::LEVEL_3: comprStr = "Level 3"; break;
          }
          break;
        }
      }
      if(matched) break;
    }

    // Heuristic fallback for unmatched files
    if(!matched) {
      if(relPath.find("p64/scenes/") != std::string::npos) {
        cat = AssetCategory::Scenes;
      } else if(relPath.find("p64/a") != std::string::npos || relPath.find("p64/conf") != std::string::npos) {
        cat = AssetCategory::Other;
      }
    }

    categoryTotals[static_cast<int>(cat)] += fileSize;
    entries.push_back({displayName, cat, fileSize, comprStr});
  }

  hasData = true;
  sortEntries();
}

void Editor::MemoryDashboard::sortEntries()
{
  std::sort(entries.begin(), entries.end(), [this](const AssetEntry &a, const AssetEntry &b) {
    int cmp = 0;
    switch(sortColumn) {
      case 0: // Type
        cmp = strcmp(categoryName(a.category), categoryName(b.category));
        break;
      case 1: // Name
        cmp = a.name.compare(b.name);
        break;
      case 2: // Size
        cmp = (a.sizeBytes < b.sizeBytes) ? -1 : (a.sizeBytes > b.sizeBytes) ? 1 : 0;
        break;
      case 3: // Compression
        cmp = a.compression.compare(b.compression);
        break;
    }
    return sortAscending ? (cmp < 0) : (cmp > 0);
  });
}

void Editor::MemoryDashboard::refresh()
{
  scanBuildOutputs();
}
```

**Step 2: Commit**

```bash
git add src/editor/pages/parts/memoryDashboard.cpp
git commit -m "feat: implement MemoryDashboard scan and data collection"
```

---

### Task 3: Implement the draw methods (bar, summary, table)

**Files:**
- Modify: `src/editor/pages/parts/memoryDashboard.cpp` (append draw methods)

**Step 1: Add the draw methods to memoryDashboard.cpp**

Append these methods after `refresh()`:

```cpp
void Editor::MemoryDashboard::drawBudgetBar()
{
  uint64_t cartLimit = CART_SIZES[selectedCartSize];
  uint64_t usedSize = totalRomSize > 0 ? totalRomSize : 0;

  // Calculate totals from scanned files if no ROM exists
  if(usedSize == 0) {
    for(int i = 0; i < static_cast<int>(AssetCategory::_COUNT); ++i) {
      usedSize += categoryTotals[i];
    }
  }

  float ratio = cartLimit > 0 ? static_cast<float>(usedSize) / static_cast<float>(cartLimit) : 0.0f;
  float percent = ratio * 100.0f;

  // Header text
  ImGui::Text("ROM: %s / %s  [%.1f%%]",
    Utils::byteSize(usedSize).c_str(),
    CART_LABELS[selectedCartSize],
    percent);

  // Stacked bar
  ImVec2 barStart = ImGui::GetCursorScreenPos();
  float barWidth = ImGui::GetContentRegionAvail().x;
  float barHeight = 20.0f;

  auto *drawList = ImGui::GetWindowDrawList();

  // Background
  drawList->AddRectFilled(barStart,
    {barStart.x + barWidth, barStart.y + barHeight},
    IM_COL32(40, 40, 45, 255), 3.0f);

  // Draw each category segment
  float xOffset = 0.0f;
  for(int i = 0; i < static_cast<int>(AssetCategory::_COUNT); ++i) {
    if(categoryTotals[i] == 0) continue;
    float segWidth = (static_cast<float>(categoryTotals[i]) / static_cast<float>(cartLimit)) * barWidth;
    if(segWidth < 1.0f) segWidth = 1.0f;

    ImVec4 col = categoryColor(static_cast<AssetCategory>(i));
    drawList->AddRectFilled(
      {barStart.x + xOffset, barStart.y},
      {barStart.x + xOffset + segWidth, barStart.y + barHeight},
      ImGui::ColorConvertFloat4ToU32(col),
      (i == 0) ? 3.0f : 0.0f  // round left edge only for first segment
    );
    xOffset += segWidth;
  }

  // Over-budget warning line at 100%
  if(ratio > 1.0f) {
    float lineX = barStart.x + barWidth;
    drawList->AddLine({lineX, barStart.y}, {lineX, barStart.y + barHeight}, IM_COL32(255, 80, 80, 255), 2.0f);
  }

  ImGui::Dummy({barWidth, barHeight + 4.0f});
}

void Editor::MemoryDashboard::drawCategorySummary()
{
  // Color legend inline
  for(int i = 0; i < static_cast<int>(AssetCategory::_COUNT); ++i) {
    if(categoryTotals[i] == 0) continue;

    ImVec4 col = categoryColor(static_cast<AssetCategory>(i));
    ImGui::SameLine();
    ImGui::TextColored(col, "%s", "\xe2\x96\xa0"); // Unicode filled square
    ImGui::SameLine();
    ImGui::Text("%s %s", categoryName(static_cast<AssetCategory>(i)), Utils::byteSize(categoryTotals[i]).c_str());
  }
  ImGui::NewLine();
}

void Editor::MemoryDashboard::drawAssetTable()
{
  ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg
    | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY
    | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

  if(!ImGui::BeginTable("##AssetTable", 4, flags)) return;

  ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultSort, 0.15f);
  ImGui::TableSetupColumn("Name", 0, 0.45f);
  ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortDescending, 0.2f);
  ImGui::TableSetupColumn("Compression", 0, 0.2f);
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableHeadersRow();

  // Handle sorting
  if(auto *sortSpecs = ImGui::TableGetSortSpecs()) {
    if(sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
      sortColumn = sortSpecs->Specs[0].ColumnIndex;
      sortAscending = (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending);
      sortEntries();
      sortSpecs->SpecsDirty = false;
    }
  }

  for(auto &entry : entries) {
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::TextColored(categoryColor(entry.category), "%s", categoryName(entry.category));

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(entry.name.c_str());

    ImGui::TableNextColumn();
    ImGui::Text("%s", Utils::byteSize(entry.sizeBytes).c_str());

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(entry.compression.c_str());
  }

  ImGui::EndTable();
}

void Editor::MemoryDashboard::draw()
{
  // Top toolbar: cart size selector + refresh button
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

  ImGui::BeginChild("MEM_TOP", ImVec2(0, 22),
    ImGuiChildFlags_AlwaysUseWindowPadding,
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
  );
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

    ImGui::Text("Cart:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::Combo("##CartSize", &selectedCartSize, CART_LABELS, CART_SIZE_COUNT);

    ImGui::SameLine();
    if(ImGui::Button("Refresh", {64, 0})) {
      refresh();
    }

    ImGui::PopStyleVar(2);
  ImGui::EndChild();

  ImGui::PopStyleVar(2);

  if(!hasData) {
    ImGui::TextDisabled("No build data. Click Refresh after building your project.");
    return;
  }

  ImGui::Spacing();
  drawBudgetBar();
  drawCategorySummary();
  ImGui::Separator();
  drawAssetTable();
}
```

**Step 2: Commit**

```bash
git add src/editor/pages/parts/memoryDashboard.cpp
git commit -m "feat: implement MemoryDashboard draw (bar, summary, table)"
```

---

### Task 4: Wire up the panel in the editor

**Files:**
- Modify: `src/editor/pages/editorScene.h:6-33` — add include and member
- Modify: `src/editor/pages/editorScene.cpp:107-108` — add dock window
- Modify: `src/editor/pages/editorScene.cpp:161-165` — add draw call

**Step 1: Add include to editorScene.h**

After line 9 (`#include "parts/logWindow.h"`), add:
```cpp
#include "parts/memoryDashboard.h"
```

**Step 2: Add member to editorScene.h**

After line 32 (`LogWindow logWindow{};`), add:
```cpp
      MemoryDashboard memoryDashboard{};
```

**Step 3: Register dock window in editorScene.cpp**

After line 108 (`ImGui::DockBuilderDockWindow("Log", dockBottomID);`), add:
```cpp
    ImGui::DockBuilderDockWindow("Memory", dockBottomID);
```

**Step 4: Add draw call in editorScene.cpp**

After the Log window draw block (after line 165 `ImGui::End();` that closes Log), add:
```cpp

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
  ImGui::Begin("Memory");
  ImGui::PopStyleVar();
    memoryDashboard.draw();
  ImGui::End();
```

**Step 5: Commit**

```bash
git add src/editor/pages/editorScene.h src/editor/pages/editorScene.cpp
git commit -m "feat: wire MemoryDashboard into editor dock layout"
```

---

### Task 5: Build and test

**Step 1: Build the project**

```bash
cmake --build --preset macos
```

Expected: Clean compile, no errors.

**Step 2: Test manually**

1. Launch `./pyrite64` and open an existing project
2. Verify "Memory" tab appears in the bottom dock alongside Files and Log
3. Click "Refresh" — should show "No build data" if project hasn't been built
4. Build the project (Build menu → Build)
5. Click "Refresh" in the Memory tab — should populate with asset data
6. Verify the stacked bar shows colored segments
7. Verify the table lists individual assets with sizes
8. Verify cart size dropdown changes the budget limit
9. Verify column sorting works (click headers)

**Step 3: Fix any issues found during testing**

**Step 4: Final commit if any fixes were needed**

```bash
git add -A
git commit -m "fix: memory dashboard polish from testing"
```

---

### Task 6: Build .app bundle and verify

**Step 1: Build the .app bundle**

```bash
bash build_app.sh
```

**Step 2: Test the .app bundle**

1. Open `Pyrite64.app`
2. Load a project, build it, verify Memory tab works in .app context
3. Verify no path issues (since we only use `ctx.project->getPath()` which is absolute)

**Step 3: Commit if any .app-specific fixes were needed**
