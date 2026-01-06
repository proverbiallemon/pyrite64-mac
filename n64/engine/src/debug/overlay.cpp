/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "overlay.h"
#include "debug/debugDraw.h"
#include "scene/scene.h"
#include "vi/swapChain.h"
#include "lib/memory.h"
#include <vector>
#include <string>

#include "audio/audioManager.h"
#include "lib/matrixManager.h"

namespace {
  constexpr uint32_t SCREEN_HEIGHT = 240;
  constexpr uint32_t SCREEN_WIDTH = 320;

  constexpr float barWidth = 280.0f;
  constexpr float barHeight = 3.0f;
  constexpr float barRefTimeMs = 1000.0f / 30.0f; // FPS

  constexpr color_t COLOR_BVH{0,0xAA,0x22, 0xFF};
  constexpr color_t COLOR_COLL{0x22,0xFF,0, 0xFF};
  constexpr color_t COLOR_ACTOR_UPDATE{0xAA,0,0, 0xFF};
  constexpr color_t COLOR_CULL{0xFF,0x11,0x99, 0xFF};

  uint64_t ticksSelf = 0;

  constexpr float usToWidth(long timeUs) {
    double timeMs = (double)timeUs / 1000.0;
    return (float)(timeMs / (double)barRefTimeMs) * barWidth;
  }

  float frameTimeScale = 2;

  std::vector<std::string> sceneNames{"tst0", "tst1", "tst2"};

  float ticksToOffset(uint32_t ticks) {
    float timeOffX = TICKS_TO_US((uint64_t)ticks) / 1000.0f;
    return timeOffX * frameTimeScale;
  };

  void addBoolItem(Debug::Overlay::Menu &menu, const char* name, bool &value) {
    menu.items.push_back({name, value, Debug::Overlay::MenuItemType::BOOL, [&value](auto &item) {
      value = item.value;
    }});
  }
  void addActionItem(Debug::Overlay::Menu &menu, const char* name, std::function<void(Debug::Overlay::MenuItem&)> action) {
    menu.items.push_back({name, 0, Debug::Overlay::MenuItemType::ACTION, action});
  }

  bool showCollMesh = false;
  bool showCollSpheres = false;
  bool matrixDebug = false;
  bool showMenuScene = false;
  bool showFrameTime = false;
}

void Debug::Overlay::draw(P64::Scene &scene, int triCount, float deltaTime) {
  auto &collScene = scene.getCollision();
  uint64_t newTicksSelf = get_ticks();

  auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
  auto held = joypad_get_buttons_held(JOYPAD_PORT_1);

  if(menu.items.empty()) {
    addActionItem(menu, "Scenes", []([[maybe_unused]] auto &item) { showMenuScene = true; });

    addBoolItem(menu, "Spheres", showCollSpheres);
    addBoolItem(menu, "Coll-Tri", showCollMesh);
    addBoolItem(menu, "Memory", matrixDebug);
    addBoolItem(menu, "Frames", showFrameTime);

    addActionItem(menuScenes, "< Back >", []([[maybe_unused]] auto &item) {
      showMenuScene = false;
    });

    for(auto &sceneName : sceneNames)
    {
      addActionItem(menuScenes, sceneName.c_str(), [&scene, sceneName]([[maybe_unused]] auto &item) {
        uint32_t sceneId = 0;
        for(int i=0; i<4; ++i) {
          sceneId |= sceneName[i] << (24 - i * 8);
        }
        P64::SceneManager::load(sceneId);
      });
    }
  }

  Menu *currMenu = showMenuScene ? &menuScenes : &menu;

  if(btn.d_up)--currMenu->currIndex;
  if(btn.d_down)++currMenu->currIndex;
  if(currMenu->currIndex > currMenu->items.size() - 1)currMenu->currIndex = 0;

  if(btn.d_left)currMenu->items[currMenu->currIndex].value--;
  if(btn.d_right)currMenu->items[currMenu->currIndex].value++;
  if(btn.d_left || btn.d_right) {
    auto &item = currMenu->items[currMenu->currIndex];
    if(item.type == Overlay::MenuItemType::BOOL)item.value = (item.value < 0) ? 1 : (item.value % 2);
    item.onChange(item);
  }

  collScene.debugDraw(showCollMesh, showCollSpheres);

  float posX = 16;
  float posY = 130;

  if(showFrameTime) {
    rdpq_sync_pipe();

    constexpr uint32_t fbCount = 3;
    float viBarWidth = 300;

    color_t fbStateCol[6];
    fbStateCol[0] = {0x00, 0x00, 0x00, 0xFF};
    fbStateCol[1] = {0x33, 0xFF, 0x33, 0xFF};
    fbStateCol[2] = {0x22, 0x77, 0x77, 0xFF};
    fbStateCol[3] = {0x22, 0xAA, 0xAA, 0xFF};
    fbStateCol[4] = {0xAA, 0xAA, 0xAA, 0xFF};
    fbStateCol[5] = {0xAA, 0x22, 0xAA, 0xFF};

    rdpq_mode_push();
    rdpq_set_mode_fill({0,0,0, 0xFF});
    rdpq_fill_rectangle(posX, posY-2, posX + viBarWidth, posY + 7 * fbCount+1);
    rdpq_fill_rectangle(posX, posY-10, posX + viBarWidth, posY - 6);

    rdpq_mode_pop();

    Debug::printStart();

    posY = 64;
    Debug::printf(posX + 200, posY-8, "FPS: %.2f", (double)P64::VI::SwapChain::getFPS());

    return;
  }
  Debug::printStart();

  posY = 24;

  heap_stats_t heap_stats;
  sys_get_heap_stats(&heap_stats);

  rdpq_set_prim_color(COLOR_BVH);
  posX = Debug::printf(posX, posY, "%.2f", (double)TICKS_TO_US(collScene.ticksBVH) / 1000.0) + 8;
  rdpq_set_prim_color(COLOR_COLL);
  posX = Debug::printf(posX, posY, "%.2f", (double)TICKS_TO_US(collScene.ticks - collScene.ticksBVH) / 1000.0) + 2;
  posX = Debug::printf(posX, posY, ":%d", collScene.raycastCount) + 8;
  rdpq_set_prim_color(COLOR_ACTOR_UPDATE);
  //posX = Debug::printf(posX, posY, "%.2f", (double)TICKS_TO_US(scene.ticksActorUpdate) / 1000.0) + 8;
  rdpq_set_prim_color(COLOR_CULL);
  //Debug::printf(posX, posY + 9, "%.2f", (double)TICKS_TO_US(scene.ticksCull) / 1000.0);
  //posX = Debug::printf(posX, posY, "%.2f", (double)TICKS_TO_US(scene.getAudio().ticks) / 1000.0) + 8;


  rdpq_set_prim_color({0xFF,0xFF,0xFF, 0xFF});

  posX = 24 + barWidth - 120;
  //posX = Debug::printf(posX, posY, "A:%d/%d", scene.activeActorCount, scene.drawActorCount) + 8;
  posX = Debug::printf(posX, posY, "T:%d", triCount) + 8;
  Debug::printf(posX, posY, "H:%d", heap_stats.used / 1024);

  posX = 24;

  // Menu
  posY = 38;
  for(auto &item : currMenu->items) {
    bool isSel = currMenu->currIndex == (uint32_t)(&item - &currMenu->items[0]);
    switch(item.type) {
      case MenuItemType::INT:
        Debug::printf(posX, posY, "%c %s: %d", isSel ? '>' : ' ', item.text, item.value);
        break;
      case MenuItemType::BOOL:
        Debug::printf(posX, posY, "%c %s: %c", isSel ? '>' : ' ', item.text, item.value ? '1' : '0');
        break;
      case MenuItemType::ACTION:
        Debug::printf(posX, posY, "%c %s", isSel ? '>' : ' ', item.text);
        break;
    }
//    Debug::printf(posX, posY, "%c %s: %d", isSel ? '>' : ' ', item.text, item.value);
    posY += 8;
  }

  // audio channels
  posX = 24;
  posY = SCREEN_HEIGHT - 24;

  posX = Debug::printf(posX, posY, "CH (TODO)");
  /*uint32_t audioMask = scene.getAudio().getActiveChannelMask();
  for(int i=0; i<16; ++i) {
    bool isActive = audioMask & (1 << i);
    posX = Debug::printf(posX, posY, isActive ? "%d" : "-", i);
  }
*/

  // Matrix slots
  if(matrixDebug)
  {
    posX = 100;
    posY = 50;

    for(uint32_t f=0; f<3; ++f) {
      Debug::printf(posX, posY, "Color[%ld]: %p\n", f, P64::VI::SwapChain::getFrameBuffer(f)->buffer);
      posY += 8;
    }

    posY = 90;
    uint32_t matCount = P64::MatrixManager::getTotalCapacity();
    for(uint32_t i=0; i<matCount; ++i) {
      bool isUsed = P64::MatrixManager::isUsed(i);
      Debug::printf(posX, posY, "%c", isUsed ? '+' : '.');
      posX += 6;
      if(i % 32 == 31) {
        posX = 100;
        posY += 8;
      }
    }
  }


  posX = 24;
  posY = 16;

  // Performance graph
  float timeCollBVH = usToWidth(TICKS_TO_US(collScene.ticksBVH));
  float timeColl = usToWidth(TICKS_TO_US(collScene.ticks - collScene.ticksBVH));
  //float timeActorUpdate = usToWidth(TICKS_TO_US(scene.ticksActorUpdate));
  //float timeCull = usToWidth(TICKS_TO_US(scene.getAudio().ticks));
  float timeSelf = usToWidth(TICKS_TO_US(ticksSelf));

  rdpq_set_mode_fill({0,0,0, 0xFF});
  rdpq_fill_rectangle(posX-1, posY-1, posX + (barWidth/2), posY + barHeight+1);
  rdpq_set_mode_fill({0x33,0x33,0x33, 0xFF});
  rdpq_fill_rectangle(posX-1 + (barWidth/2), posY-1, posX + barWidth+1, posY + barHeight+1);

  rdpq_set_fill_color(COLOR_BVH);
  rdpq_fill_rectangle(posX, posY, posX + timeCollBVH, posY + barHeight); posX += timeCollBVH;
  rdpq_set_fill_color(COLOR_COLL);
  rdpq_fill_rectangle(posX, posY, posX + timeColl, posY + barHeight); posX += timeColl;
  rdpq_set_fill_color(COLOR_ACTOR_UPDATE);
  //rdpq_fill_rectangle(posX, posY, posX + timeActorUpdate, posY + barHeight); posX += timeActorUpdate;
  rdpq_set_fill_color(COLOR_CULL);
  //rdpq_fill_rectangle(posX, posY, posX + timeCull, posY + barHeight); posX += timeCull;
  rdpq_set_fill_color({0xFF,0xFF,0xFF, 0xFF});
  rdpq_fill_rectangle(24 + barWidth - timeSelf, posY, 24 + barWidth, posY + barHeight);

  newTicksSelf = get_ticks() - newTicksSelf;
  if(newTicksSelf < TICKS_FROM_MS(2))
  {
    ticksSelf = newTicksSelf;
  }
}
