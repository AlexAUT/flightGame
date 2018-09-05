#include "gameState.hpp"

#include <functional>

#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>
#include <aw/utils/math/constants.hpp>

#include <aw/graphics/core/color.hpp>
#include <aw/graphics/core/shaderStage.hpp>
#include <aw/opengl/opengl.hpp>

using namespace aw::constantsF;

DEFINE_LOG_CATEGORY(GameD, aw::log::Debug, "GameState")

GameState::GameState(aw::Engine& engine)
    : aw::State(engine.getStateMachine()), mEngine(engine),
      mCamera(aw::Camera::createPerspective(800.f / 600.f, 60.f * TO_RAD)), mCamController(&mCamera)

{
  auto color = aw::Colors::BISQUE;
  GL_CHECK(glClearColor(color.r, color.g, color.b, 1.0));

  mEventListenerId =
      mEngine.getWindow().registerEventListener(std::bind(&GameState::processEvent, this, std::placeholders::_1));

  mLevel.load("level1.json", mScene, mResourceManager, mMeshRenderer);

  auto vMesh = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Vertex, "shaders/mesh.vert");
  auto fMeshForward = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Fragment, "shaders/simpleMeshForward.frag");
  mMeshForwardShader.link(*vMesh, *fMeshForward);

  GLuint vao;
  GL_CHECK(glGenVertexArrays(1, &vao));
  GL_CHECK(glBindVertexArray(vao));
}

GameState::~GameState()
{
  mEngine.getWindow().unregisterEventListener(mEventListenerId);
}

void GameState::update(float delta)
{
  mCamController.update(delta);
}

void GameState::render()
{
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  mMeshRenderer.renderForwardPass(mCamera, mMeshForwardShader);
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

  mCamController.processEvent(event);
}
