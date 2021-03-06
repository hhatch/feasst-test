#include <cmath>
#include "confinement/include/half_space.h"

namespace feasst {

class MapHalfSpace {
 public:
  MapHalfSpace() {
    auto obj = MakeHalfSpace({
      {"dimension", "1"},
      {"intersection", "1"},
      {"direction", "1"},
    });
    obj->deserialize_map()["HalfSpace"] = obj;
  }
};

static MapHalfSpace mapper_ = MapHalfSpace();

HalfSpace::HalfSpace(const argtype &args) : Shape() {
  args_.init(args);
  dimension_ = args_.key("dimension").integer();
  intersection_ = args_.key("intersection").dble();
  direction_ = args_.key("direction").integer();
  ASSERT(direction_ == -1 or direction_ == 1, "invalid direction: "
    << direction_);
}

double HalfSpace::nearest_distance(const Position& point) const {
  DEBUG("c " << point.coord(dimension_) << " " <<
                -1.*direction_*(point.coord(dimension_) - intersection_));
  return -1.*direction_*(point.coord(dimension_) - intersection_);
}

}  // namespace feasst
