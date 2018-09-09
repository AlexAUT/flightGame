#include "gameState.hpp"

#include <functional>

#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>

#include <aw/graphics/core/color.hpp>
#include <aw/graphics/core/postProcessRenderer.hpp>
#include <aw/graphics/core/shaderStage.hpp>
#include <aw/opengl/opengl.hpp>

#include <aw/graphics/3d/directionalLight.hpp>

using namespace aw::constantsF;

DEFINE_LOG_CATEGORY(GameD, aw::log::Debug, "GameState")

GameState::GameState(aw::Engine& engine)
    : aw::State(engine.getStateMachine()), mEngine(engine),
      mCamera(aw::Camera::createPerspective(800.f / 600.f, 60.f * TO_RAD, 0.1f, 200.f)), mCamController(&mCamera)

{
  auto color = aw::Colors::CADETBLUE;
  GL_CHECK(glClearColor(color.r, color.g, color.b, 1.0));

  mEventListenerId =
      mEngine.getWindow().registerEventListener(std::bind(&GameState::processEvent, this, std::placeholders::_1));

  mLevel.load("level1.json", mScene, mResourceManager, mMeshRenderer);
  mAirplane.loadFromAssetFile("airplane1.json", mScene, mResourceManager, mMeshRenderer);
  mAirplane.setPosition({-0.5f, 2.f, 0.f});
  mAirplane.setVelocity(0.25f);
  mAirplane.setFlightDirection({0.0f, 0.f, 1.f});

  auto vMesh = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Vertex, "shaders/mesh.vert");

  auto fShadowMap = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Fragment, "shaders/setAlpha1.frag");
  mMeshShadowMapShader.link(*vMesh, *fShadowMap);

  auto fMeshForward = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Fragment, "shaders/simpleMeshForward.frag");
  mMeshForwardShader.link(*vMesh, *fMeshForward);

#ifdef AW_USE_OPENGL
  GLuint vao;
  GL_CHECK(glGenVertexArrays(1, &vao));
  GL_CHECK(glBindVertexArray(vao));
#endif
  mShadowFramebuffer.create(2048, 2048, 8, 0);
}

GameState::~GameState()
{
  mEngine.getWindow().unregisterEventListener(mEventListenerId);
}

void GameState::update(float delta)
{
  mAirplane.update(delta);
  LogTemp() << "Delta: " << delta;

  mCamController.update(delta, mAirplane);
}

void GameState::render()
{
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  aw::DirectionalLight light;
  light.color = aw::Vec3(1.f, 1.f, 1.f);
  light.direction = glm::normalize(aw::Vec3(-0.25f, 0.5f, -0.25f));
  light.energy = 0.9;

  auto c = mCamera.getPosition();
  auto fov = mCamera.getFieldOfView();
  auto v = glm::normalize(mCamera.getViewDirection());
  auto up = glm::normalize(mCamera.getUpDirection());
  auto left = glm::normalize(mCamera.getLeftDirection());
  LogTemp() << "Up direction: " << up;

  float farDistance = 10.f;
  auto nearMid = c + v * mCamera.getNearPlane();
  auto farMid = c + v * farDistance;
  // auto midPos = c + v * (mCamera.getNearPlane() + (0.5f * (farDistance - mCamera.getNearPlane())));

  auto nearHalfWidth = glm::tan(fov) * mCamera.getNearPlane();
  auto farHalfWidth = glm::tan(fov) * farDistance;

  aw::AABB boundingBox;
  boundingBox += nearMid + left * nearHalfWidth;
  boundingBox += nearMid - left * nearHalfWidth;
  boundingBox += farMid + left * farHalfWidth;
  boundingBox += farMid - left * farHalfWidth;

  LogTemp() << "Near and far: " << nearHalfWidth << " " << farHalfWidth;
  LogTemp() << boundingBox;

  // auto aPos = mAirplane.getPosition();
  // aw::Camera lightCam(aw::Camera::createOrthograpic(boundingBox.min.x, boundingBox.max.x, boundingBox.min.z,
  //                                                  boundingBox.max.z, 0.1, 12.f
  aw::Camera lightCam(aw::Camera::createOrthograpic(-10.f, 10.f, -10.f, 10.f, 0.1f, 12.f));
  lightCam.setRotationEuler({-PI_2, 0.f, 0.f});
  lightCam.setPosition(aw::Vec3(0.f, 5.f, 0.f));

  mShadowFramebuffer.bind();
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  mMeshRenderer.renderShadowMap(lightCam, mMeshShadowMapShader, light);

  mShadowFramebuffer.unbind();

  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  mMeshRenderer.renderForwardPassWithShadow(mCamera, lightCam, mShadowFramebuffer.getDepthTexture(), mMeshForwardShader,
                                            light);

  glDisable(GL_DEPTH_TEST);
  // aw::PostProcessRenderer::render(mShadowFramebuffer.getColorTexture());
}

void GameState::processEvent(const aw::WindowEvent& event)
{
  if (event.type == aw::WindowEvent::Closed)
    mEngine.terminate();

  if (event.type == aw::WindowEvent::KeyReleased)
  {
    if (event.key.code == sf::Keyboard::Escape)
      mEngine.terminate();
  }

  if (event.type == aw::WindowEvent::Resized)
  {
    glViewport(0, 0, event.size.width, event.size.height);
    float aspect = event.size.width / float(event.size.height);
    mCamera.setAspectRatio(aspect);
  }

  mCamController.processEvent(event);
}
