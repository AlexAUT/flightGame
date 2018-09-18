#pragma once

#include <string>

#include <aw/runtime/scene/scene.hpp>

#include <aw/utils/math/quaternion.hpp>
#include <aw/utils/math/vector.hpp>

class ResourceManager;

namespace aw
{
class MeshRenderer;
class MeshNode;
} // namespace aw

class Airplane
{
public:
  Airplane();

  bool loadFromAssetFile(const std::string& path, aw::Scene& scene, ResourceManager& resManager,
                         aw::MeshRenderer& meshRenderer);

  void update(float delta);

  void setPosition(aw::Vec3 pos);
  void setVelocity(float velocity);

  aw::Vec3 getPosition() const;
  float getVelocity() const;

private:
  float test;

private:
  float mtest2;
  aw::MeshNode* test2;
  aw::MeshNode* mPlaneNode{nullptr};

  float mVelocity{0.f};
  aw::Quaternion mFlightOrientation{1.f, 0.f, 0.f, 0.f};
};
