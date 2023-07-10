#include "renderer.hpp"
#include "solver.hpp"
#include "verlet.hpp"
#include <SFML/Graphics.hpp>
#include <ctime>

const int WSIZE = 1600;
const int FRAMERATE = 60;
const float dt = 1.0f / (FRAMERATE * 1.0f);

int main() {
  sf::RenderWindow window(sf::VideoMode(WSIZE, WSIZE), "Fluid Simulation");
  window.setFramerateLimit(FRAMERATE);

  Solver solver(WSIZE);
  Renderer renderer(window);

  sf::Clock spawnClock;

  while (window.isOpen()) {

    // events

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (spawnClock.getElapsedTime().asSeconds() > 0.01f) {
      auto &obj = solver.addObject(sf::Vector2f(100.0f, 100.0f));
      obj.setVelocity(sf::Vector2f(100.0f, 30.0f), dt / 2.0f);

      auto &obj2 = solver.addObject(sf::Vector2f(100.0f, 110.0f));
      obj2.setVelocity(sf::Vector2f(100.0f, 30.0f), dt / 2.0f);

      spawnClock.restart();
    }

    solver.update(dt);

    window.clear(sf::Color::Black);

    renderer.render(solver);

    window.display();
  }

  return 0;
}
