#pragma once

#include "SFML/Graphics.hpp"
#include "cmath"
#include "collisionGrid.hpp"
#include "verlet.hpp"
#include <cstdio>
#include <fstream>
#include <vector>

class Solver {
public:
  Solver(int mapSize) : mapSize(mapSize), collisionGrid(mapSize, mapSize, 8) {}

  VerletObject &addObject(sf::Vector2f pos) {
    return objects.emplace_back(pos, atomSize);
  }

  void update(float dt) {
    float sub_dt = dt / (subSteps * 1.0f);
    for (int i = 0; i < subSteps; i++) {
      checkCollisions(sub_dt);

      applyGravity();
      applyConstraints();

      for (int i = 0; i < objects.size(); i++) {
        objects[i].updatePosition(sub_dt);
      }
    }
  };

  void takeSnapshot(std::string filename) {
    std::ofstream file(filename);

    for (int i = 0; i < objects.size(); i++) {
      int x = objects[i].pos.x;
      int y = objects[i].pos.y;
      file << x << " " << y << '\n';
    }

    file.close();
  }

  const std::vector<VerletObject> &getObjects() const { return objects; }

private:
  std::vector<VerletObject> objects;
  sf::Vector2f gravity = sf::Vector2f(0.0f, 1000.0f);
  CollisionGrid collisionGrid;
  int subSteps = 8;
  const int atomSize = 8;

  int mapSize;

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
        obj.pos.x = rdh + 1;
      }
      if (obj.pos.x > size - rdh) {
        obj.pos.x = size - rdh - 1;
      }
      if (obj.pos.y > size - rdh) {
        obj.pos.y = size - rdh - 1;
      }
      if (obj.pos.y < rdh) {
        obj.pos.y = rdh + 1;
      }
    }
  }

  void checkCollisions(float dt) {
    collisionGrid.reset();

    // build collisionGrid
    for (int i = 0; i < objects.size(); i++) {
      collisionGrid.addVal(objects[i].pos, i);
    }

    // handle collision
    for (int i = 0; i < objects.size(); i++) {
      int idxInGrid = collisionGrid.getCollisionGridIdx(objects[i].pos);
      auto neighbors = collisionGrid.getNeighbors(idxInGrid);

      for (auto neighIdx : neighbors) {
        if (neighIdx == i)
          continue;

        sf::Vector2f diff = objects[i].pos - objects[neighIdx].pos;
        float dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

        float radSum = (objects[i].radius + objects[neighIdx].radius);
        bool colliding = radSum > dist;

        if (colliding) {
          sf::Vector2f normalized = diff / dist;
          float delta = dist - radSum;

          auto oldGridIdx = collisionGrid.getCollisionGridIdx(objects[i].pos);
          auto oldNeighGridIdx =
              collisionGrid.getCollisionGridIdx(objects[neighIdx].pos);

          objects[i].pos -= normalized * (delta / 2.0f);
          objects[neighIdx].pos += normalized * (delta / 2.0f);

          // update collision grid if positions changed

          auto newGridIdx = collisionGrid.getCollisionGridIdx(objects[i].pos);
          auto newNeighGridIdx =
              collisionGrid.getCollisionGridIdx(objects[neighIdx].pos);

          if (oldGridIdx != newGridIdx) {
            collisionGrid.removeVal(oldGridIdx, i);
            collisionGrid.addVal(newGridIdx, i);
          }

          if (oldNeighGridIdx != newNeighGridIdx) {
            collisionGrid.removeVal(oldNeighGridIdx, neighIdx);
            collisionGrid.addVal(newNeighGridIdx, neighIdx);
          }
        }
      }
    }
  }
};
