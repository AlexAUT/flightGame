#pragma once

#include <aw/runtime/scene/scene.hpp>

namespace aw
{
class MeshRenderer;
}

class ResourceManager;

class Level
{
public:
  Level();

  bool load(const std::string& name, aw::Scene& scene, ResourceManager& resManager, aw::MeshRenderer& meshRenderer);

private:
private:
  aw::Scene* mScene{nullptr};
};
