#include "level.hpp"

#include "resourceManager.hpp"

#include <aw/runtime/renderers/meshRenderer.hpp>
#include <aw/runtime/scene/meshNode.hpp>
#include <aw/runtime/scene/sceneLoader.hpp>
#include <aw/utils/log.hpp>

DEFINE_LOG_CATEGORY(LevelE, aw::log::Error, "Level")

Level::Level()
{
}

bool Level::load(const std::string& path, aw::Scene& scene, ResourceManager& resManager, aw::MeshRenderer& meshRenderer)
{
  bool result = aw::SceneLoader::loadFromAssetFile(path, scene, resManager.textures, resManager.meshes,
                                                   resManager.meshAnimations);
  if (!result)
    return false;

  auto mapNode = dynamic_cast<aw::MeshNode*>(scene.findNodeByName("map"));
  LogLevelE() << mapNode->meshInstance().getMesh().getBounds();
  if (!mapNode)
  {
    LogLevelE() << "The level file did not contain a map mesh node: " << path;
    return false;
  }

  // Register nodes for rendering
  auto lambda = [&](aw::SceneNode* node) {
    auto* t = dynamic_cast<aw::MeshNode*>(node);
    if (t)
      meshRenderer.registerMesh(t);
  };
  auto func = std::function<void(aw::SceneNode*)>(lambda);
  mapNode->traverseChilds(func);

  return true;
}
