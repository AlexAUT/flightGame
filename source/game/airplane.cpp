#include "airplane.hpp"

#include "resourceManager.hpp"

#include <aw/runtime/renderers/meshRenderer.hpp>
#include <aw/runtime/scene/meshNode.hpp>
#include <aw/runtime/scene/sceneLoader.hpp>
#include <aw/utils/assetInputStream.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>
using namespace aw::constantsF;

#include <SFML/Window/Keyboard.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

DEFINE_LOG_CATEGORY(AirplaneE, aw::log::Error, "Airplane")

Airplane::Airplane()
{
}

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
  float sensitivity = 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    mFlightOrientation *= aw::Quaternion(1.f, delta * 1.f, 0.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    mFlightOrientation *= aw::Quaternion(1.f, -delta * 1.f, 0.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    mFlightOrientation *= aw::Quaternion(1.f, 0.f, delta * 1.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    mFlightOrientation *= aw::Quaternion(1.f, 0.f, -delta * 1.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    mFlightOrientation *= aw::Quaternion(1.f, 0.f, 0.f, delta * 1.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    mFlightOrientation *= aw::Quaternion(1.f, 0.f, 0.f, -delta * 1.f);
  // setPosition(getPosition() + glm::normalize(mFlightDirection) * mVelocity * delta);
  // mFlightOrientation = aw::Quaternion(aw::Vec3(0.f, 0.f, 0.f));
  auto flightDir = glm::rotate(mFlightOrientation, aw::Vec3(0.f, 0.f, 1.f));
  setPosition(getPosition() + glm::normalize(flightDir) * mVelocity * delta);
  // Airplane points torwards viewer, so rotate it by 180° in Y axis
  auto localRot = aw::Quaternion(aw::Vec3(0.f, PI, 0.f));
  mPlaneNode->localTransform().setRotation(mFlightOrientation);
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
