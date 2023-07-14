#include "renderer.hpp"
#include "solver.hpp"
#include "utils/colors.hpp"
#include "verlet.hpp"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const int WSIZE = 1080;
const int FRAMERATE = 60;
const float dt = 1.0f / (FRAMERATE * 1.0f);
unsigned int falls = 2;

unsigned int objectsSpawned = 0;
std::vector<sf::Color> colors;
unsigned int frameCount = 0;

void spawnFluid(Solver &solver, unsigned int amount);

int main() {
  sf::RenderWindow window(sf::VideoMode(WSIZE, WSIZE), "Fluid Simulation");
  window.setFramerateLimit(FRAMERATE);

  Solver solver(WSIZE);
  Renderer renderer(window);

  sf::Clock spawnClock;

  // read colors
  colors = loadColorsFromFile("colors.txt");

  while (window.isOpen()) {

    // events

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Equal) {
          falls += 1;
        } else if (event.key.code == sf::Keyboard::Space) {
          solver.takeSnapshot("objects.txt");
        }
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
        solver.generateExplosion(mouse, 400000.0f, dt);
      }
    }

    if (frameCount % 6 == 0) {
      if (solver.getObjects().size() < solver.getMaxObjects()) {
        spawnFluid(solver, falls);
      }

      spawnClock.restart();
    }

    solver.update(dt);

    window.clear(sf::Color::Black);

    renderer.render(solver);

    window.display();

    frameCount++;
  }

  return 0;
}

void spawnFluid(Solver &solver, unsigned int amount) {
  for (int i = 0; i < amount; i++) {
    for (int j = 0; j < amount; j++) {
      auto &obj = solver.addObject(
          sf::Vector2f(100.0f + j * 20.0f, 100.0f + i * 20.0f));
      obj.setVelocity(sf::Vector2f(2100.0f, 0.0f),
                      dt / (solver.getSubsteps() * 1.0f));
      obj.col = colors[objectsSpawned++];
    }
  }
}
