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
  const aw::Quaternion& getFlightOrientation() const { return mFlightOrientation; }
  float getSidewaysForce() const { return mSideWaysForce; }
  float getUpwaysForce() const { return mUpWaysForce; }

  aw::MeshNode* getPlaneNode() const;

private:
  void updateControls(float delta);
  void updateKeyboard(float delta);
  void updateMouse(float delta);
  void updateTouch(float delta);

private:
  float mtest2;
  aw::MeshNode* test2;
  aw::MeshNode* mPlaneNode{nullptr};

  float mVelocity{0.f};
  aw::Quaternion mFlightOrientation{1.f, 0.f, 0.f, 0.f};

  float mUpwaysSensitivity{1.25f};
  float mUpWaysForce{0.f};
  bool mUpwaysChanged{false};

  float mSidewaysSensitivity{1.25f};
  float mSideWaysForce{0.f};
  bool mSidewaysChanged{false};

  bool mDragginPlaneMouse{false};
  bool mDragginPlaneTouch{false};
  aw::Vec2 mDragStart;
};
