#include "renderer.hpp"
#include "solver.hpp"
#include "verlet.hpp"
#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 800), "My window");

  Solver solver;
  Renderer renderer(window);
  solver.addObject(sf::Vector2f(100.0f, 200.0f), 10.0f);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    solver.update();

    window.clear(sf::Color::Black);

    renderer.render(solver);

    window.display();
  }

  return 0;
}
