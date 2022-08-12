#ifndef DICE_TOPOLOGY_HPP
#define DICE_TOPOLOGY_HPP

#include <cstdint>
#include <vector>

namespace dice {

using point_index = std::uint16_t;
using face_index  = std::uint16_t;

// sparse_topology data
// ====================
struct sparse_point
{}; // stub

struct sparse_face
{
  std::vector<point_index> point_indices;
};

// dense_topology data
// ===================
struct dense_point : public sparse_point
{
  float connectivity;
};

struct dense_face : public sparse_face
{
  std::vector<face_index> neighbors;
};

// common data
// ===========
struct face_pair
{
  face_index first;
  face_index second;
};

struct symmetry_group
{
  std::vector<face_index> offsets;
  face_index              repeat;
};

// topology structs
// ================
struct sparse_topology
{
  std::vector<sparse_face> faces;
  std::vector<face_pair>   pairs;
  symmetry_group           rotation;
};

struct dense_topology
{
  std::vector<dense_point> points;
  std::vector<dense_face>  faces;

  // these are unchanged from sparse_topology
  std::vector<face_pair> pairs;
  symmetry_group         rotation;
};

dense_topology make_dense(const sparse_topology& sparse);

} // namespace dice

#endif // DICE_TOPOLOGY_HPP