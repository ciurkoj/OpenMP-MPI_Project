#include "Position.h"

Position::Position(int x, int y, int z) : x(x), y(y), z(z) {}

std::string Position::convertToString(int index) {
  std::string convertedToString = std::to_string(index) + ".\t[" + std::to_string(this->x) + ", " +
                                  std::to_string(this->y) + ", " + std::to_string(this->z) + "]";
  return convertedToString;
}