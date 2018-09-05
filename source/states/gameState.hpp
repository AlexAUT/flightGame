#pragma once

#include "../game/cameraController.hpp"
#include "../game/level.hpp"
#include "../game/resourceManager.hpp"

#include <aw/engine/runtime/state.hpp>
#include <aw/engine/window.hpp>
#include <aw/graphics/core/camera.hpp>
#include <aw/graphics/core/shaderProgram.hpp>
#include <aw/runtime/renderers/meshRenderer.hpp>
#include <aw/runtime/scene/scene.hpp>

namespace aw
{
class Engine;
} // namespace aw

class GameState : public aw::State
{
public:
  GameState(aw::Engine& engine);
  ~GameState();

  void update(float delta) override final;
  void render() override final;

  void processEvent(const aw::WindowEvent& event);

private:
  aw::Engine& mEngine;
  unsigned mEventListenerId;

  aw::Camera mCamera;
  CameraController mCamController;

  aw::ShaderProgram mMeshForwardShader;
  ResourceManager mResourceManager;
  aw::Scene mScene;
  aw::MeshRenderer mMeshRenderer;

  Level mLevel;
};
