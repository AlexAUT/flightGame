#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>

#include "states/gameState.hpp"

int main()
{
  aw::LOG_INITIALIZE();

  aw::Engine engine;

  auto initState = std::make_shared<GameState>(engine);
  engine.getStateMachine().pushState(initState);

  auto returnValue = engine.run();
  engine.getSettings().save();

  LogTemp() << "Returned from gameloop with exit code: " << returnValue;

  return returnValue;
}
