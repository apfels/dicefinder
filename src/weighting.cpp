#include "weighting.hpp"

#include <cmath>
#include <numeric>
#include <ranges>

namespace dice {

face_values values_from(const dense_topology& topo, const layout& lo)
{
  face_values values(topo.faces.size());

  for ( auto i { 0z }; i < topo.pairs.size(); ++i )
  {
    const auto& pair    = topo.pairs[i];
    values[pair.first]  = lo.at(i, false);
    values[pair.second] = lo.at(i, true);
  }
  return values;
}

float average(const std::ranges::range auto& r)
{
  return std::reduce(std::ranges::begin(r), std::ranges::end(r), 0.f, std::plus {}) /
    float(std::ranges::size(r));
}

float pop_variance(const std::ranges::range auto& r, float avg)
{
  auto var = std::transform_reduce(
    std::ranges::begin(r),
    std::ranges::end(r),
    0.f,
    std::plus {},
    [&](float v) { return (v - avg) * (v - avg); });

  return var / float(std::ranges::size(r));
}

face_values rotate(const dense_topology& topo, const face_values& values)
{
  auto        n { values.size() };
  face_values rotated(n);
  for ( auto i { 0uz }; i < n; ++i ) { rotated[(i + topo.rotation.offsets[i]) % n] = values[i]; }
  return rotated;
}

bool is_lexical_rotation(const dense_topology& topo, const face_values& values)
{
  auto rot { values };
  for ( auto i { 0z }; i < topo.rotation.repeat; ++i )
  {
    rot = rotate(topo, rot);
    if ( std::ranges::lexicographical_compare(rot, values) ) { return false; }
  }
  return true;
}

point_weight make_point_weight(const dense_topology& topo, const std::vector<face_value>& values)
{
  const auto n_points = topo.points.size();

  point_weight result {};

  // All faces that this point is part of are added to total weight
  std::vector<float> weights(n_points);

  for ( auto i { 0z }; i < topo.faces.size(); ++i )
  {
    const auto& f { topo.faces[i] };
    for ( const auto& p : f.point_indices ) { weights[p] += float(values[i]); }
  }
  for ( auto i { 0z }; i < topo.points.size(); ++i ) { weights[i] /= topo.points[i].connectivity; }

  // Calculate statistics
  auto avg_point { average(weights) };
  result.avg = avg_point;
  auto minmax_point { std::ranges::minmax_element(weights) };
  result.lower = avg_point - *minmax_point.min;
  result.upper = *minmax_point.max - avg_point;
  result.sdev  = std::sqrt(pop_variance(weights, avg_point));

  return result;
}

face_weight make_face_weight(const dense_topology& topo, const std::vector<face_value>& values)
{
  const auto n_faces = topo.faces.size();

  face_weight result {};

  // All neighboring faces added to weight
  std::vector<float> face_weights(n_faces);
  for ( auto i { 0z }; i < topo.faces.size(); ++i )
  {
    const auto& f { topo.faces[i] };
    for ( const auto& other_i : f.neighbors ) { face_weights[i] += float(values[other_i]); }
  }

  // Calculate statistics like with points
  auto avg_face { average(face_weights) };
  result.avg = avg_face;
  auto minmax_face { std::ranges::minmax_element(face_weights) };
  result.lower = avg_face - *minmax_face.min;
  result.upper = *minmax_face.max - avg_face;
  result.sdev  = std::sqrt(pop_variance(face_weights, avg_face));

  return result;
}

die_weight make_weight(const dense_topology& topo, const std::vector<face_value>& values)
{
  return { .points = make_point_weight(topo, values), .faces = make_face_weight(topo, values) };
}

} // namespace dice