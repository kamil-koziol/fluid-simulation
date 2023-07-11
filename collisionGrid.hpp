#pragma once

#include "SFML/Graphics.hpp"
#include <vector>

class CollisionGrid {
public:
  CollisionGrid(unsigned int width, unsigned int height, unsigned int atomSize)
      : width(width), height(height), atomSize(atomSize) {
    cells = width * height / atomSize;
    gridWidth = width / atomSize;
    gridHeight = height / atomSize;

    grid = new std::vector<int>[cells];
  }
  ~CollisionGrid() { delete[] grid; }

  int getCollisionGridIdx(sf::Vector2f pos) {
    if (pos.x > width || pos.y > height) {
      return 0;
    }
    int x = (int)pos.x;
    int y = (int)pos.y;

    int ax = x / atomSize;
    int ay = y / atomSize;

    return ay * gridWidth + ax;
  }

  std::vector<int> getNeighbors(int idx) {
    std::vector<int> neighbors;

    int y = idx / gridWidth;
    int x = idx - (y * gridWidth);

    int range = 2;
    for (int i = -range; i <= range; i++) {
      for (int j = -range; j <= range; j++) {
        if (i == 0 && j == 0)
          continue;

        int neighX = x + j;
        int neighY = y + i;
        bool validNeighborPos = neighX >= 0 && neighX < gridWidth &&
                                neighY >= 0 && neighY < gridHeight;

        if (!validNeighborPos)
          continue;

        int neighIdx = neighY * gridWidth + neighX;
        for (auto val : grid[neighIdx]) {
          neighbors.emplace_back(val);
        }
      }
    }

    return neighbors;
  }

  void addVal(sf::Vector2f pos, int val) {
    int gidx = getCollisionGridIdx(pos);
    if (gidx < 0 || gidx >= cells)
      return;
    grid[gidx].emplace_back(val);
  }

  void addVal(int gridIdx, int val) {
    if (gridIdx < 0 || gridIdx >= cells)
      return;
    grid[gridIdx].emplace_back(val);
  }

  void removeVal(int gridIdx, int val) {
    if (gridIdx < 0 || gridIdx >= cells)
      return;
    auto &gridCell = grid[gridIdx];
    gridCell.erase(std::remove(gridCell.begin(), gridCell.end(), val),
                   gridCell.end());
  }

  void reset() {
    for (int i = 0; i < cells; i++) {
      grid[i].clear();
    }
  }

private:
  std::vector<int> *grid;
  unsigned int atomSize;
  unsigned int width;
  unsigned int gridWidth;
  unsigned int height;
  unsigned int gridHeight;
  unsigned int cells;
};
