#pragma once

#include <aw/runtime/managers/MeshAnimationManager.hpp>
#include <aw/runtime/managers/MeshManager.hpp>
#include <aw/runtime/managers/TextureManager.hpp>

class ResourceManager
{
public:
  ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager operator=(const ResourceManager&) = delete;

  aw::MeshManager meshes;
  aw::MeshAnimationManager meshAnimations;
  aw::TextureManager textures;
};
