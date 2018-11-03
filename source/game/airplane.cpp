#include "airplane.hpp"

#include "resourceManager.hpp"

#include <aw/runtime/renderers/meshRenderer.hpp>
#include <aw/runtime/scene/meshNode.hpp>
#include <aw/runtime/scene/sceneLoader.hpp>
#include <aw/utils/file/assetInputStream.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>
using namespace aw::constantsF;

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Touch.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

DEFINE_LOG_CATEGORY(AirplaneE, aw::log::Error, "Airplane")

Airplane::Airplane() {}

bool Airplane::loadFromAssetFile(const std::string& assetPath, aw::Scene& scene, ResourceManager& resManager,
                                 aw::MeshRenderer& meshRenderer)
{
  aw::Scene* container = new aw::SceneNode();

  bool result = aw::SceneLoader::loadFromAssetFile(assetPath, *container, resManager.textures, resManager.meshes,
                                                   resManager.meshAnimations);
  if (!result)
    return false;

  scene.addChild(container);

  mPlaneNode = dynamic_cast<aw::MeshNode*>(container->findNodeByName("airplane"));
  if (!mPlaneNode)
  {
    LogAirplaneE() << "The airplane file did not contain a airplane mesh node: " << assetPath;
    return false;
  }

  // Register nodes for rendering
  auto lambda = [&](aw::SceneNode* node) {
    auto* t = dynamic_cast<aw::MeshNode*>(node);
    if (t)
      meshRenderer.registerMesh(t);
  };
  auto func = std::function<void(aw::SceneNode*)>(lambda);
  container->traverseChilds(func);

  return true;
}

void Airplane::update(float delta)
{
  updateControls(delta);

  if (!mUpwaysChanged)
  {
    const float tolerance = 0.001f;
    const float constant = 1.4f;
    const float quadratic = 1.f;
    const float cubic = 1.f;
    auto absValue = glm::abs(mUpWaysForce);
    if (absValue > tolerance)
    {
      auto sign = absValue / mUpWaysForce;
      auto slow = absValue * constant + quadratic * glm::pow(absValue, 2) + cubic * glm::pow(absValue, 3);
      mUpWaysForce -= sign * slow * delta;
    }
    else
    {
      mUpWaysForce = 0.f;
    }
  }
  mUpwaysChanged = false;

  if (!mSidewaysChanged)
  {
    const float tolerance = 0.001f;
    const float constant = 1.4f;
    const float quadratic = 1.f;
    const float cubic = 1.f;

    auto absValue = glm::abs(mSideWaysForce);
    if (absValue > tolerance)
    {
      auto sign = absValue / mSideWaysForce;
      auto slow = absValue * constant + quadratic * glm::pow(absValue, 2) + cubic * glm::pow(absValue, 3);
      mSideWaysForce -= sign * slow * delta;
    }
    else
    {
      mSideWaysForce = 0.f;
    }
  }
  mSidewaysChanged = false;

  const float maxUpForce = PI_4;
  mUpWaysForce = std::max(std::min(mUpWaysForce, maxUpForce), -maxUpForce);
  mFlightOrientation *= aw::Quaternion(aw::Vec3(delta * mUpWaysForce, 0.f, 0.f));

  const float maxSideForce = PI_4;
  mSideWaysForce = std::max(std::min(mSideWaysForce, maxSideForce), -maxSideForce);
  mFlightOrientation = aw::Quaternion(aw::Vec3(0.f, delta * mSideWaysForce, 0.f)) * mFlightOrientation;
  // setPosition(getPosition() + glm::normalize(mFlightDirection) * mVelocity * delta);
  // mFlightOrientation = aw::Quaternion(aw::Vec3(0.f, 0.f, 0.f));
  auto flightDir = glm::rotate(mFlightOrientation, aw::Vec3(0.f, 0.f, 1.f));
  setPosition(getPosition() + glm::normalize(flightDir) * mVelocity * delta);
  // Airplane points torwards viewer, so rotate it by 180° in Y axis
  auto upWaysRotation = aw::Quaternion(aw::Vec3(0.75 * mUpWaysForce, 0.f, 0.f));
  auto sideWaysRotation = aw::Quaternion(aw::Vec3(-glm::abs(mSideWaysForce) * 0.4f, 0.f, -mSideWaysForce));
  mPlaneNode->localTransform().setRotation(mFlightOrientation * upWaysRotation * sideWaysRotation);
}

void Airplane::setPosition(aw::Vec3 pos)
{
  mPlaneNode->localTransform().setPosition(pos);
}

void Airplane::setVelocity(float velocity)
{
  mVelocity = velocity;
}

aw::Vec3 Airplane::getPosition() const
{
  return mPlaneNode->localTransform().getPosition();
}

float Airplane::getVelocity() const
{
  return mVelocity;
}

aw::MeshNode* Airplane::getPlaneNode() const
{
  return mPlaneNode;
}

void Airplane::updateControls(float delta)
{
  updateKeyboard(delta);
  updateMouse(delta);
  updateTouch(delta);
}

void Airplane::updateKeyboard(float delta)
{
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
  {
    mSideWaysForce += mSidewaysSensitivity * delta;
    mSidewaysChanged = mSideWaysForce > 0;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
  {
    mSideWaysForce -= mSidewaysSensitivity * delta;
    mSidewaysChanged = mSideWaysForce < 0;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
  {
    mUpWaysForce += mUpwaysSensitivity * delta;
    mUpwaysChanged = true;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
  {
    mUpWaysForce -= mUpwaysSensitivity * delta;
    mUpwaysChanged = true;
  }
}

void Airplane::updateMouse(float delta)
{
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
  {
    if (!mDragginPlaneMouse)
    {
      mDragginPlaneMouse = true;
      auto mousePos = sf::Mouse::getPosition();
      mDragStart = {mousePos.x, mousePos.y};
    }
    else
    {
      auto mousePos = sf::Mouse::getPosition();
      auto direction = aw::Vec2{mousePos.x, mousePos.y} - mDragStart;
      const float maxRadius = 20 * 20;
      direction.x = glm::clamp(-direction.x / maxRadius, -1.f, 1.f);
      direction.y = glm::clamp(direction.y / maxRadius, -1.f, 1.f);

      mSideWaysForce += direction.x * mSidewaysSensitivity * delta;
      mSidewaysChanged = (direction.x * mSideWaysForce) > 0;
      mUpWaysForce += direction.y * mUpwaysSensitivity * delta;
      mUpwaysChanged = (direction.y * mUpWaysForce) > 0;
    }
  }
  else
  {
    mDragginPlaneMouse = false;
  }
}

void Airplane::updateTouch(float delta)
{
  if (sf::Touch::isDown(0))
  {
    if (!mDragginPlaneTouch)
    {
      mDragginPlaneTouch = true;
      auto mousePos = sf::Touch::getPosition(0);
      mDragStart = {mousePos.x, mousePos.y};
      LogTemp() << "Drag Start: " << mDragStart;
    }
    else
    {
      auto mousePos = sf::Touch::getPosition(0);
      auto direction = aw::Vec2{mousePos.x, mousePos.y} - mDragStart;
      LogTemp() << "DIRECTION: " << direction;
      const float maxRadius = 100;
      direction.x = glm::clamp(-direction.x / maxRadius, -1.f, 1.f);
      direction.y = glm::clamp(direction.y / maxRadius, -1.f, 1.f);

      mSideWaysForce += direction.x * mSidewaysSensitivity * delta;
      mSidewaysChanged = (direction.x * mSideWaysForce) > 0;
      mUpWaysForce += direction.y * mUpwaysSensitivity * delta;
      mUpwaysChanged = (direction.y * mUpWaysForce) > 0;
    }
  }
  else
  {
    LogTemp() << "TOUCH UP?";
    mDragginPlaneTouch = false;
  }
}
