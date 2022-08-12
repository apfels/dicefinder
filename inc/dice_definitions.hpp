#ifndef DICE_DEFINITIONS_HPP
#define DICE_DEFINITIONS_HPP

#include "topology.hpp"

namespace dice {

inline dense_topology hexahedron()
{
  const static sparse_topology sparse_hexahedron
  {
    .faces = {
      {{0,1,2,3}},
      {{0,3,4,5}},
      {{0,5,6,1}},
      {{1,6,7,2}},
      {{2,7,4,3}},
      {{4,7,6,5}},
    },
    .pairs = {
      {0, 5},
      {1, 3},
      {2, 4},
    },
    .rotation = {
      .offsets = { 0, 1, 1, 1, 3, 0 },
      .repeat = 3
    },
  };
  return make_dense(sparse_hexahedron);
}

inline dense_topology octahedron()
{
  const static sparse_topology sparse_octahedron {
    .faces = {
      {{0,1,2}},
      {{0,2,3}},
      {{0,3,4}},
      {{0,4,1}},
      {{1,4,5}},
      {{1,5,2}},
      {{2,5,3}},
      {{3,5,4}},
    },
    .pairs = {
      {0,7},
      {1,4},
      {2,5},
      {3,6},
    },
    .rotation = {
      .offsets = { 0, 2, 2, 2, 2, 4, 4, 0 },
      .repeat = 2
    },
  };
  return make_dense(sparse_octahedron);
}

inline dense_topology pent_trapezohedron()
{
  const sparse_topology sparse_pent_trapezohedron {
    .faces = {
      {{0,1,2,3}},
      {{0,3,4,5}},
      {{0,5,6,7}},
      {{0,7,8,9}},
      {{0,9,10,1}},
      {{1,10,11,2}},
      {{2,11,4,3}},
      {{4,11,6,5}},
      {{6,11,8,7}},
      {{8,11,10,9}},
    },
    .pairs = {
      {0,8},
      {1,9},
      {2,5},
      {3,6},
      {4,7},
    },
    .rotation = {},
  };
  return make_dense(sparse_pent_trapezohedron);
}

inline dense_topology dodecahedron()
{
  const sparse_topology sparse_dodecahedron {
    .faces = {
      {{0,1,2,3,4}},
      {{0,4,5,6,7}},
      {{0,7,8,9,1}},
      {{1,9,10,11,2}},
      {{2,11,12,13,3}},
      {{3,13,14,5,4}},
      {{5,14,15,16,6}},
      {{6,16,17,8,7}},
      {{8,17,18,10,9}},
      {{10,18,19,12,11}},
      {{12,19,15,14,13}},
      {{15,16,17,18,19}},
    },
    .pairs = {
      {0,11},
      {1,9},
      {2,10},
      {3,6},
      {4,7},
      {5,8},
    },
    .rotation = {
      .offsets = {0, 1, 1, 1, 1, 8, 1, 1, 1, 1, 8, 0},
      .repeat = 4,
    },
  };
  return make_dense(sparse_dodecahedron);
}

inline dense_topology icosahedron()
{
  const sparse_topology sparse_icosahedron {
    .faces = {
      {{0,1,2}},
      {{0,2,3}},
      {{0,3,4}},
      {{0,4,5}},
      {{0,5,1}},
      {{1,5,6}},
      {{1,6,7}},
      {{1,7,2}},
      {{2,7,8}},
      {{2,8,3}},
      {{3,8,9}},
      {{3,9,4}},
      {{4,9,10}},
      {{4,10,5}},
      {{5,10,6}},
      {{6,10,11}},
      {{6,11,7}},
      {{7,11,8}},
      {{8,11,9}},
      {{9,11,10}},
    },
    .pairs = {
      {0,19},
      {1,15},
      {2,16},
      {3,17},
      {4,18},
      {5,10},
      {6,11},
      {7,12},
      {8,13},
      {9,14},
    },
    .rotation = {
      .offsets = {0, 3, 3, 3, 3, 3, 3, 14, 14, 14, 3, 3, 3, 3, 3, 3, 14, 14, 14, 0},
      .repeat = 2
    },
  };
  return make_dense(sparse_icosahedron);
}

} // namespace dice
#endif // DICE_DEFINITIONS_HPP