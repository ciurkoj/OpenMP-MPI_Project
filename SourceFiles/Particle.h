#pragma once
#include "Position.h"
#include <vector>

class Particle
{
public:
    Particle(int X, int Y, int Z);

    Position initialPosition;
    Position currentPosition;

    void moveParticle(int pos_alter_var);
    void moveToTheCenter(Particle *centerPoint, int pos_alter_var);
};