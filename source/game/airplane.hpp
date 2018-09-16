#pragma once

#include <string>

#include <aw/runtime/scene/scene.hpp>

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
  void setFlightDirection(aw::Vec3 flightDirection);

  aw::Vec3 getPosition() const;
  float getVelocity() const;
  aw::Vec3 getFlightDirection() const;

private:
  float test;

private:
  float mtest2;
  aw::MeshNode* test2;
  aw::MeshNode* mPlaneNode{nullptr};

  float mVelocity{0.f};
  aw::Vec3 mFlightDirection{0.f};
};
