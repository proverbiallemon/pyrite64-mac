/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "assets/assetManager.h"
#include "scene/object.h"
#include "assets/assetManager.h"
#include "scene/components/collMesh.h"
#include <t3d/t3dmodel.h>

#include "scene/scene.h"
#include "scene/sceneManager.h"

namespace P64::Comp
{
  void CollMesh::initDelete([[maybe_unused]] Object& obj, CollMesh* data, uint16_t* initData)
  {
    if (initData == nullptr) {
      if(data->meshInstance.mesh) {
        obj.getScene().getCollision().unregisterMesh(&data->meshInstance);
      }

      data->~CollMesh();
      return;
    }

    uint16_t assetIdx = initData[0];
    new(data) CollMesh();
    //debugf("Mesh: %d | id: %d\n", assetIdx, obj.id);
    auto asset = (T3DModel*)AssetManager::getByIndex(assetIdx);
    auto it = t3d_model_iter_create(asset, (T3DModelChunkType)'0');
    if(t3d_model_iter_next(&it)) {
      data->meshInstance.object = &obj;
      // @TODO: reuse mesh
      data->meshInstance.mesh = Coll::Mesh::load(it.chunk);
      obj.getScene().getCollision().registerMesh(&data->meshInstance);
    }
  }

  void CollMesh::onEvent(Object &obj, CollMesh* data, const ObjectEvent &event)
  {
    if(event.type == EVENT_TYPE_DISABLE) {
      return obj.getScene().getCollision().unregisterMesh(&data->meshInstance);
    }
    if(event.type == EVENT_TYPE_ENABLE) {
      obj.getScene().getCollision().registerMesh(&data->meshInstance);
    }
  }

  void CollMesh::update(Object &obj, CollMesh* data, float deltaTime)
  {
  }
}
