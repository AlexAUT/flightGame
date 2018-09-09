#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>

#include "states/gameState.hpp"

int main()
{
  aw::LOG_INITIALIZE();

  LogTemp() << "WTF STARTING?";

  aw::Engine engine;

  auto initState = std::make_shared<GameState>(engine);
  engine.getStateMachine().pushState(initState);

  auto returnValue = engine.run();
  engine.getSettings().save();

  LogTemp() << "Returned from gameloop with exit code: " << returnValue;

  return returnValue;
}


/*#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window({800, 600}, "TEst");
    while(window.isOpen())
    {
        sf::Event e;
        while(window.pollEvent(e))
        {

        }
        window.clear(sf::Color::Red);
        window.display();
    }

}*/