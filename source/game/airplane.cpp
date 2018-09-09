#include "airplane.hpp"

#include "resourceManager.hpp"

#include <aw/runtime/renderers/meshRenderer.hpp>
#include <aw/runtime/scene/meshNode.hpp>
#include <aw/runtime/scene/sceneLoader.hpp>
#include <aw/utils/assetInputStream.hpp>
#include <aw/utils/log.hpp>

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
    mFlightDirection = glm::rotateX(mFlightDirection, sensitivity * delta);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    mFlightDirection = glm::rotateX(mFlightDirection, -sensitivity * delta);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    mFlightDirection = glm::rotateY(mFlightDirection, -sensitivity * delta);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    mFlightDirection = glm::rotateY(mFlightDirection, sensitivity * delta);

  setPosition(getPosition() + glm::normalize(mFlightDirection) * mVelocity * delta);

  glm::mat4 lookMat =
      glm::lookAt(glm::vec3(0.f, 0.f, 0.f), mFlightDirection * glm::vec3(1.f, 1.f, -1.f), glm::vec3(0, 1, 0));
  auto rotation = glm::toQuat(lookMat);
  mPlaneNode->localTransform().setRotation(rotation);
}

void Airplane::setPosition(aw::Vec3 pos)
{
  mPlaneNode->localTransform().setPosition(pos);
}

void Airplane::setVelocity(float velocity)
{
  mVelocity = velocity;
}

void Airplane::setFlightDirection(aw::Vec3 flightDirection)
{
  mFlightDirection = flightDirection;
}

aw::Vec3 Airplane::getPosition() const
{
  return mPlaneNode->localTransform().getPosition();
}

aw::Vec3 Airplane::getFlightDirection() const
{
  return mFlightDirection;
}
float Airplane::getVelocity() const
{
  return mVelocity;
}
