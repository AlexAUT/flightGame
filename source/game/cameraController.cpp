#include "cameraController.hpp"

CameraController::CameraController(aw::Camera* camera) : mOrbitalController(camera)
{
  mOrbitalController.setViewAtPoint({0.f, 0.5f, 10.f});
  mOrbitalController.setDistanceToViewPoint(10.f);
  mOrbitalController.setRotationHorizontal(0.61f);
  mOrbitalController.setRotationVertical(3.19f);
}

void CameraController::setCamera(aw::Camera* camera)
{
  mOrbitalController.setCamera(camera);
}

void CameraController::update(float delta)
{
  mOrbitalController.update(delta);
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
  if (event.type == aw::WindowEvent::KeyPressed)
  {
    const float moveFactor = 10.f;
    if (event.key.code == sf::Keyboard::W)
      mOrbitalController.rotateHorizontal(moveFactor);
    if (event.key.code == sf::Keyboard::S)
      mOrbitalController.rotateHorizontal(-moveFactor);
    if (event.key.code == sf::Keyboard::A)
      mOrbitalController.rotateVertical(moveFactor);
    if (event.key.code == sf::Keyboard::D)
      mOrbitalController.rotateVertical(-moveFactor);
  }
}
