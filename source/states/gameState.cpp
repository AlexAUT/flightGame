#include "gameState.hpp"

#include <functional>

#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>

#include <aw/graphics/core/color.hpp>
#include <aw/graphics/core/postProcessRenderer.hpp>
#include <aw/graphics/core/shaderStage.hpp>
#include <aw/opengl/opengl.hpp>

#include <aw/runtime/scene/meshNode.hpp>
#include <aw/utils/math/frustum.hpp>

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
  mAirplane.setVelocity(2.f);
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
  // LogTemp() << "Delta: " << delta;

  mCamController.update(delta, mAirplane);
}

void GameState::render()
{
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  aw::DirectionalLight light;
  light.color = aw::Vec3(1.f, 1.f, 1.f);
  light.direction = glm::normalize(aw::Vec3(0.0f, 0.99f, 0.01f));
  light.energy = 0.9;

  auto shadowView = glm::lookAt({0.f, 0.f, 0.}, light.direction, {0.f, 1.f, 0.f});
  LogTemp() << shadowView;

  auto mapNode = (aw::MeshNode*)mScene.findNodeByName("map");
  auto shadowBox = aw::AABB::createFromTransform(mapNode->meshInstance().getMesh().getBounds(), shadowView);
  const float s = 0.001;
  shadowBox.min.x -= std::fmod(shadowBox.min.x, s);
  shadowBox.min.y -= std::fmod(shadowBox.min.y, s);
  shadowBox.max.x += (s - std::fmod(shadowBox.max.x, s));
  shadowBox.max.y += (s - std::fmod(shadowBox.max.y, s));

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    LogTemp() << shadowBox;

  aw::Camera lightCam(aw::Camera::createOrthograpic(shadowBox.min.x, shadowBox.max.x, shadowBox.min.y, shadowBox.max.y,
                                                    shadowBox.min.z, shadowBox.max.z));
  lightCam.setRotationEuler({-PI_2, 0.f, 0.f});
  lightCam.setPosition(aw::Vec3(0.f, 0.f, 0.f));

  std::array<float, 5> splitDistances = {-0.1, -5, -15, -40, -100.f};
  for (auto& d : splitDistances)
  {
    auto c = mCamera.getProjectionMatrix() * aw::Vec4(0.f, 0.f, d, 1.f);
    d = c.z / c.w;
  }

  std::array<aw::Camera, 5> mShadowCamera;

  auto inverseCam = glm::inverse(mCamera.getVPMatrix());
  for (unsigned i = 0; i < splitDistances.size() - 1; i++)
  {
    auto n = splitDistances[i];
    auto f = splitDistances[i + 1];

    aw::AABB mapBounds;
    auto corners = aw::frustum::split(n, f, inverseCam);
    for (auto& v : corners)
    {
      v = lightCam.getViewMatrix() * aw::Vec4(v, 1.0);
      mapBounds += v;
    }
    mapBounds.min.x -= std::fmod(mapBounds.min.x, s);
    mapBounds.min.y -= std::fmod(mapBounds.min.y, s);
    mapBounds.max.x += (s - std::fmod(mapBounds.max.x, s));
    mapBounds.max.y += (s - std::fmod(mapBounds.max.y, s));

    LogTemp() << "Center: " << mapBounds.getCenter();

    mShadowCamera[i] = aw::Camera::createOrthograpic(mapBounds.min.x, mapBounds.max.x, mapBounds.min.y, mapBounds.max.y,
                                                     shadowBox.min.z, shadowBox.max.z);
    mShadowCamera[i].setRotationEuler({-PI_2, 0.f, 0.f});
    mShadowCamera[i].setPosition({mapBounds.getCenter().x, 0.f, -mapBounds.getCenter().y});
  }

  mShadowFramebuffer.bind();
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  for (int i = 0; i < 4; i++)
  {
    glViewport(1024 * (i % 2), 1024 * (i / 2), 1024, 1024);
    mMeshRenderer.renderShadowMap(mShadowCamera[i], mMeshShadowMapShader, light);
  }
  glViewport(0, 0, mEngine.getWindow().getSize().x, mEngine.getWindow().getSize().y);
  mShadowFramebuffer.unbind();

  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  mMeshForwardShader.bind();
  const aw::Mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
  for (unsigned i = 0; i < mShadowCamera.size(); i++)
  {
    mMeshForwardShader.setUniform("shadow_mvp_biased[" + std::to_string(i) + "]",
                                  biasMatrix * mShadowCamera[i].getVPMatrix());
  }
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
