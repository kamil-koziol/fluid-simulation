#include "renderer.hpp"
#include "solver.hpp"
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
unsigned int falls = 6;
unsigned int maxObjects = WSIZE * WSIZE / (3.2 * 8 * 8) - 500;
unsigned int current_object = 0;
std::vector<sf::Color> colors;
unsigned int frameCount = 0;

void loadColorsFromFile(const std::string &filename) {
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);

      int r, g, b;
      iss >> r >> g >> b;

      sf::Color color(r, g, b);
      colors.emplace_back(color);
    }

    file.close();
  } else {
    std::cout << "Failed to open file: " << filename << std::endl;
  }
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WSIZE, WSIZE), "Fluid Simulation");
  window.setFramerateLimit(FRAMERATE);

  Solver solver(WSIZE);
  Renderer renderer(window);

  sf::Clock spawnClock;

  // read colors
  loadColorsFromFile("colors.txt");

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
    }

    // if (spawnClock.getElapsedTime().asSeconds() > 0.1f) {
    if (frameCount % 6 == 0) {
      if (solver.getObjects().size() < maxObjects) {
        for (int i = 0; i < falls; i++) {
          for (int j = 0; j < falls; j++) {
            auto &obj = solver.addObject(
                sf::Vector2f(100.0f + j * 20.0f, 100.0f + i * 20.0f));
            obj.setVelocity(sf::Vector2f(2100.0f, 0.0f), dt / 8.0f);
            obj.col = colors[current_object++];
          }
        }
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
