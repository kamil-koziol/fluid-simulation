#pragma once

#include "SFML/Graphics.hpp"
#include "verlet.hpp"
#include <vector>

class Solver {
public:
  Solver() {}

  void addObject(VerletObject object) { objects.emplace_back(object); }
  void addObject(sf::Vector2f pos, float radius) {
    objects.emplace_back(pos, radius);
  }
  void update() {
    applyGravity();
    for (int i = 0; i < objects.size(); i++) {
      objects[i].updatePosition(0.001f);
    }
    applyConstraints();
  };
  const std::vector<VerletObject> &getObjects() const { return objects; }

private:
  std::vector<VerletObject> objects;
  sf::Vector2f gravity = sf::Vector2f(0.0f, 100.0f);
  float time;

  void applyGravity() {
    for (int i = 0; i < objects.size(); i++) {
      objects[i].accelerate(gravity);
    }
  }

  void applyConstraints() {
    for (auto &obj : objects) {
      float rdh = obj.radius / 2;
      if (obj.pos.x < rdh) {
        obj.pos.x = rdh;
      }
      if (obj.pos.x > 800 - rdh) {
        obj.pos.x = 800 - rdh;
      }
      if (obj.pos.y > 800 - rdh) {
        obj.pos.y = 800 - rdh;
      }
    }
  }
};
