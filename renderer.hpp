#pragma once
#include "SFML/Graphics.hpp"
#include "solver.hpp"

class Renderer {
public:
  explicit Renderer(sf::RenderTarget &target) : m_target(target) {}

  void render(const Solver &solver) const {
    // Render objects
    sf::CircleShape circle(1.0f);
    circle.setPointCount(32);
    circle.setOrigin(1.0f, 1.0f);

    const auto &objects = solver.getObjects();
    for (const auto &obj : objects) {
      circle.setPosition(obj.pos);
      circle.setScale(obj.radius, obj.radius);
      circle.setFillColor(obj.col);

      m_target.draw(circle);
    }
  }

private:
  sf::RenderTarget &m_target;
};
