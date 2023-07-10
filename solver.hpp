#pragma once

#include "SFML/Graphics.hpp"
#include "cmath"
#include "collisionGrid.hpp"
#include "verlet.hpp"
#include <vector>

class Solver {
public:
  Solver(int mapSize) : mapSize(mapSize) {
    gridSize = mapSize * mapSize / atomSize;
    collisionGrid = new std::vector<int>[gridSize];
  }

  VerletObject &addObject(sf::Vector2f pos) {
    return objects.emplace_back(pos, atomSize);
  }

  void update(float dt) {
    float sub_dt = dt / (subSteps * 1.0f);
    for (int i = 0; i < subSteps; i++) {
      applyGravity();
      applyConstraints();
      checkCollisions(sub_dt);

      for (int i = 0; i < objects.size(); i++) {
        objects[i].updatePosition(sub_dt);
      }
    }
  };

  const std::vector<VerletObject> &getObjects() const { return objects; }

private:
  std::vector<VerletObject> objects;
  sf::Vector2f gravity = sf::Vector2f(0.0f, 1000.0f);
  int subSteps = 4;
  int mapSize;
  int atomSize = 5;
  int gridSize;
  std::vector<int> *collisionGrid;

  void applyGravity() {
    for (int i = 0; i < objects.size(); i++) {
      objects[i].accelerate(gravity);
    }
  }

  void applyConstraints() {
    float size = mapSize;
    for (auto &obj : objects) {
      float rdh = obj.radius / 2;
      if (obj.pos.x < rdh) {
        obj.pos.x = rdh;
      }
      if (obj.pos.x > size - rdh) {
        obj.pos.x = size - rdh;
      }
      if (obj.pos.y > size - rdh) {
        obj.pos.y = size - rdh;
      }
      if (obj.pos.y < rdh) {
        obj.pos.y = rdh;
      }
    }
  }

  void checkCollisions(float dt) {

    for (int i = 0; i < objects.size(); i++) {
      for (int j = i; j < objects.size(); j++) {
        if (i == j) {
          continue;
        }

        sf::Vector2f diff = objects[i].pos - objects[j].pos;
        float dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

        float radSum = (objects[i].radius + objects[j].radius);
        bool colliding = radSum > dist;

        if (colliding) {
          sf::Vector2f normalized = diff / dist;
          float delta = dist - radSum;
          objects[i].pos -= normalized * (delta / 2.0f);
          objects[j].pos += normalized * (delta / 2.0f);
        }
      }
    }
  }
};
