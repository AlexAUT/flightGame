#include "cameraController.hpp"

#include "airplane.hpp"

#include <aw/graphics/core/camera.hpp>

#include <aw/runtime/scene/meshNode.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>
using namespace aw::constantsF;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

CameraController::CameraController(aw::Camera* camera) : mCamera(camera), mOrbitalController(camera)
{
  mOrbitalController.setViewAtPoint({0.f, 0.5f, 10.f});
  mOrbitalController.setDistanceToViewPoint(2.f);
  mOrbitalController.setRotationHorizontal(0.f);
  mOrbitalController.setRotationVertical(PI);
}

void CameraController::setCamera(aw::Camera* camera)
{
  mCamera = camera;
  mOrbitalController.setCamera(camera);
}

void CameraController::update(float delta, const Airplane& airplane)
{
  //  mOrbitalController.setViewAtPoint(airplane.getPosition());
  // mOrbitalController.setRotationHorizontal(-airplane.getFlightDirection().y);
  // mOrbitalController.setRotationVertical(airplane.getFlightDirection().z);
  // mOrbitalController.update(delta);
  const float distanceToPlane = 0.75f;
  const float cameraHeight = 0.20f;
  auto* p = airplane.getPlaneNode();
  auto pos = glm::rotate(airplane.getFlightOrientation(),
                         aw::Vec3(airplane.getSidewaysForce() * -0.1f, cameraHeight, -distanceToPlane));
  mCamera->setPosition(pos + p->localTransform().getPosition());
  LogTemp() << "Plane rot: " << pos;
  mCamera->setRotation(airplane.getFlightOrientation() *
                       aw::Quaternion(aw::Vec3(0.f, 0.25f * airplane.getSidewaysForce(), 0.f)) *
                       aw::Quaternion(aw::Vec3(0.f, PI, 0.f)));
}

void CameraController::processEvent(const aw::WindowEvent& event)
{
  if (event.type == aw::WindowEvent::MouseLeft)
    mMouseInit = false;
  if (event.type == aw::WindowEvent::MouseMoved)
  {
    if (!mMouseInit)
    {
      mOldMousePos = {event.mouseMove.x, event.mouseMove.y};
      mMouseInit = true;
    }
    else
    {
      aw::Vec2 currentPos{event.mouseMove.x, event.mouseMove.y};
      mOrbitalController.rotateVertical(mOldMousePos.x - currentPos.x);
      mOrbitalController.rotateHorizontal(currentPos.y - mOldMousePos.y);
      mOldMousePos = currentPos;
    }
  }

  if (event.type == aw::WindowEvent::TouchEnded)
    mMouseInit = false;
  if (event.type == aw::WindowEvent::TouchMoved)
  {
    if (!mMouseInit)
    {
      mOldMousePos = {event.touch.x, event.touch.y};
      mMouseInit = true;
    }
    else
    {
      aw::Vec2 currentPos{event.touch.x, event.touch.y};
      mOrbitalController.rotateVertical(mOldMousePos.x - currentPos.x);
      mOrbitalController.rotateHorizontal(currentPos.y - mOldMousePos.y);
      mOldMousePos = currentPos;
    }
  }
  if (event.type == aw::WindowEvent::MouseWheelScrolled)
  {
    mOrbitalController.zoom(event.mouseWheelScroll.delta);
  }
}
