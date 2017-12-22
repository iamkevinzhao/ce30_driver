#include "data_types.h"

namespace ce30_driver {
Point::Point() : Point(0.0f, 0.0f, 0.0f) {}

Point::Point(const float& x, const float& y, const float& z) {
  this->x = x;
  this->y = y;
  this->z = z;
}
}
