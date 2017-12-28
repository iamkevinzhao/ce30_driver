#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <vector>
#include "export.h"

namespace ce30_driver {
struct API Point {
  Point();
  Point(const float& x, const float& y, const float& z);
  float x;
  float y;
  float z;
};

struct API PointCloud {
  std::vector<Point> points;
};
}
#endif // DATA_TYPES_H
