/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene/object.h"
#include "scene/components/audio2d.h"

#include "audio/audioManager.h"
#include "scene/sceneManager.h"

namespace
{
  struct InitData
  {
    uint16_t assetIdx;
    uint16_t volume;
    uint8_t flags;
    uint8_t padding;
  };
}

namespace P64::Comp
{
  void Audio2D::initDelete(Object &obj, Audio2D* data, uint16_t* initData_)
  {
    auto initData = (InitData*)initData_;
    if (initData == nullptr) {
      data->~Audio2D();
      return;
    }

    new(data) Audio2D();

    data->audio = (wav64_t*)AssetManager::getByIndex(initData->assetIdx);
    assert(data->audio);

    data->volume = (float)initData->volume * (1.0f / 0xFFFF);
    data->flags = initData->flags;
    wav64_set_loop(data->audio, (data->flags & FLAG_LOOP) != 0);

    if(data->flags & FLAG_AUTO_PLAY) {
      data->handle = AudioManager::play2D(data->audio);
      data->handle.setVolume(data->volume);
    }
  }
}
