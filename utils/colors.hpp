#pragma once

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<sf::Color> loadColorsFromFile(const std::string &filename) {
  std::vector<sf::Color> colors;

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

  return colors;
}
