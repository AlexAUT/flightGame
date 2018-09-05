#pragma once

#include <aw/engine/window.hpp>
#include <aw/graphics/3d/orbitCameraController.hpp>

#include <aw/utils/math/vector.hpp>

namespace aw
{
class Camera;
} // namespace aw

class CameraController
{
public:
  CameraController(aw::Camera* camera = nullptr);

  void setCamera(aw::Camera* camera);

  void update(float delta);
  void processEvent(const aw::WindowEvent& event);

private:
private:
  aw::OrbitCameraController mOrbitalController;

  bool mMouseInit;
  aw::Vec2i mOldMousePos;
};
