#include "bbox.hpp"
#include "interval.hpp"

const BBox3 BBox3::empty = BBox3(Interval::empty, Interval::empty, Interval::empty);
const BBox3 BBox3::universe = BBox3(Interval::universe, Interval::universe, Interval::universe);
