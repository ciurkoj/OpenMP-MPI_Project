#pragma once
#include "Position.h"
#include <vector>

class Particle {
public:
  Particle(int X, int Y, int Z);

  void moveParticle(int pos_alter_var);

  Position initialPosition;
  Position currentPosition;
};