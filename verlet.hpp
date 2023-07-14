#pragma once

#include "SFML/Graphics.hpp"

struct VerletObject {
  sf::Vector2f pos;
  sf::Vector2f pos_old;
  sf::Vector2f acc;
  float radius = 5.0f;
  sf::Color col = sf::Color::Blue;

  VerletObject() = default;
  VerletObject(sf::Vector2f pos_, float radius_)
      : pos(pos_), pos_old(pos_), acc(sf::Vector2f(0.0f, 0.0f)),
        radius(radius_) {}

  void updatePosition(float dt) {
    sf::Vector2f vel = pos - pos_old;
    pos_old = pos;
    pos = pos + vel + acc * (dt * dt);
    acc = sf::Vector2f(0.0f, 0.0f);
  }

  void accelerate(sf::Vector2f acc_) { acc += acc_; }
  void setVelocity(sf::Vector2f vel_, float dt) { pos_old = pos - (vel_ * dt); }
  void addVelocity(sf::Vector2f vel_, float dt) { pos_old -= vel_ * dt; }
};
