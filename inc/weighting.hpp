#ifndef DICE_WEIGHTING_HPP
#define DICE_WEIGHTING_HPP

#include "layout.hpp"
#include "topology.hpp"

#include <ostream>
#include <tuple>

namespace dice {

using face_values = std::vector<face_value>;

struct point_weight
{
  float avg;
  float lower; // how much lower than avg is lowest weight
  float upper; // how much higher than avg is highest weight
  float stdev;
};

struct face_weight
{
  float avg;
  float lower; // same as point_lower but for faces
  float upper; // same as point_lower but for faces
  float stdev;
};

struct die_weight
{
  point_weight points;
  face_weight  faces;

  friend std::ostream& operator<<(std::ostream& os, const die_weight& w)
  {
    return os << "point: " << w.points.avg << " -" << w.points.lower << " +" << w.points.upper
              << " ~" << w.points.stdev << " :: face: " << w.faces.avg << " -" << w.faces.lower
              << " +" << w.faces.upper << " ~" << w.faces.stdev;
  }

  friend std::partial_ordering operator<=>(const die_weight& a, const die_weight& b)
  {
    // Discard avg, should be same for all results and introduces sorting errors with -Ofast.
    const auto aa = std::tie(
      a.points.lower,
      a.points.upper,
      a.points.stdev,
      a.faces.lower,
      a.faces.upper,
      a.faces.stdev);
    const auto bb = std::tie(
      b.points.lower,
      b.points.upper,
      b.points.stdev,
      b.faces.lower,
      b.faces.upper,
      b.faces.stdev);

    return aa <=> bb;
  }
};

face_values values_from(const dense_topology& topo, const layout& lo);
bool        is_lexical_rotation(const dense_topology& topo, const face_values& values);
die_weight  make_weight(const dense_topology& topo, const face_values& values);

} // namespace dice
#endif // DICE_WEIGHTING_HPP