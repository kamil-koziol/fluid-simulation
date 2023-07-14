#pragma once

#include "SFML/Graphics.hpp"
#include "cmath"
#include "collisionGrid.hpp"
#include "verlet.hpp"
#include <cstdio>
#include <fstream>
#include <vector>

const int ATOM_SIZE = 8;

struct GridVerletObject {
  VerletObject &obj;
  int oldIndexInGrid;
  int newIndexInGrid;
  int indexInObjects;
};

class Solver {
private:
  std::vector<VerletObject> objects;
  sf::Vector2f gravity = sf::Vector2f(0.0f, 1000.0f);
  int subSteps = 8;
  const int atomSize = ATOM_SIZE;
  unsigned int maxObjects;

  int mapSize;
  CollisionGrid collisionGrid;

public:
  Solver(int mapSize)
      : mapSize(mapSize), collisionGrid(mapSize, mapSize, ATOM_SIZE) {
    maxObjects = mapSize * mapSize / (3.2 * atomSize * atomSize) - 500;
    maxObjects /= 3;
  }

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

  void generateExplosion(sf::Vector2f position, float force, float dt) {
    for (auto &obj : objects) {
      auto vec = obj.pos - position;
      float dist = sqrt(vec.x * vec.x + vec.y * vec.y);
      auto norm = vec / dist;

      if (dist == 0)
        continue;

      float explosionForce = (1 / (dist * dist)) * force;
      obj.addVelocity(norm * explosionForce, dt);
    }
  }

  int getSubsteps() const { return subSteps; }
  const std::vector<VerletObject> &getObjects() const { return objects; }
  int getMaxObjects() const { return maxObjects; }

private:
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

  void handleObjectsCollision(VerletObject &obj1, VerletObject &obj2) {
    sf::Vector2f diff = obj1.pos - obj2.pos;
    float dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

    float radSum = (obj1.radius + obj2.radius);
    bool colliding = radSum > dist;

    if (colliding) {
      sf::Vector2f normalized = diff / dist;
      float delta = dist - radSum;

      obj1.pos -= normalized * (delta / 2.0f);
      obj2.pos += normalized * (delta / 2.0f);
    }
  }

  void handleGridUpdate(GridVerletObject gvo1, GridVerletObject gvo2) {
    // update collision grid if positions changed
    if (gvo1.oldIndexInGrid != gvo1.newIndexInGrid) {
      collisionGrid.removeVal(gvo1.oldIndexInGrid, gvo1.indexInObjects);
      collisionGrid.addVal(gvo1.oldIndexInGrid, gvo1.indexInObjects);
    }

    if (gvo2.oldIndexInGrid != gvo2.newIndexInGrid) {
      collisionGrid.removeVal(gvo1.oldIndexInGrid, gvo1.indexInObjects);
      collisionGrid.addVal(gvo1.oldIndexInGrid, gvo1.indexInObjects);
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

        auto &obj1 = objects[i];
        auto &obj2 = objects[neighIdx];

        auto oldGridIdx = collisionGrid.getCollisionGridIdx(obj1.pos);
        auto oldNeighGridIdx = collisionGrid.getCollisionGridIdx(obj2.pos);

        handleObjectsCollision(obj1, obj2);

        auto newGridIdx = collisionGrid.getCollisionGridIdx(obj1.pos);
        auto newNeighGridIdx = collisionGrid.getCollisionGridIdx(obj2.pos);

        handleGridUpdate({obj1, oldGridIdx, newGridIdx, i},
                         {obj2, oldNeighGridIdx, newNeighGridIdx, neighIdx});
      }
    }
  }
};
