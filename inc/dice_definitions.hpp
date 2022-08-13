#ifndef DICE_DEFINITIONS_HPP
#define DICE_DEFINITIONS_HPP

#include "topology.hpp"

namespace dice {

// NOLINTBEGIN(*-avoid-magic-numbers): data definition file

inline const sparse_topology& hexahedron()
{
  const static sparse_topology t {
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
  return t;
}

inline const sparse_topology& octahedron()
{
  const static sparse_topology t {
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
  return t;
}

inline const sparse_topology& pent_trapezohedron()
{
  const static sparse_topology t {
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
  return t;
}

inline const sparse_topology& dodecahedron()
{
  const static sparse_topology t {
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
  return t;
}

inline const sparse_topology& icosahedron()
{
  const static sparse_topology t {
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
  return t;
}

inline const sparse_topology& rhombic_triacontahedron()
{
  const static sparse_topology t {
    .faces = {
      {{3,23,24,4}},
      {{2,26,23,3}},
      {{0,2,3,4}},
      {{0,4,12,10}},
      {{0,10,1,2}},
      {{1,10,20,19}},
      {{1,19,25,2}},
      {{2,25,27,26}},
      {{23,26,27,29}},
      {{23,29,8,16}},
      {{15,24,23,16}},
      {{4,24,15,17}},
      {{4,17,13,12}},
      {{10,12,13,14}},
      {{13,17,15,22}},
      {{19,30,27,25}},
      {{8,29,27,31}},
      {{6,31,27,30}},
      {{6,30,10,28}},
      {{11,28,19,20}},
      {{10,14,11,20}},
      {{11,14,13,18}},
      {{9,18,13,22}},
      {{9,22,15,21}},
      {{8,21,15,16}},
      {{5,9,21,8}},
      {{5,8,31,6}},
      {{5,6,7,9}},
      {{7,11,18,9}},
      {{6,28,11,7}},
    },
    .pairs = {
      {0,29},
      {1,28},
      {2,27},
      {3,26},
      {4,25},
      {5,24},
      {6,23},
      {7,22},
      {8,21},
      {9,20},
      {10,19},
      {11,18},
      {12,17},
      {13,16},
      {14,15},
    },
    .rotation = {
      .offsets = {0, 10, 8, 6, 20, 20, 17, 7, 4, 23, 21, 19, 25, 29, 22,
                  7, 26, 4, 10, 8, 6, 25, 22, 12, 9, 9, 23, 21, 19, 0},
      .repeat = 1
    },
  };
  return t;
}

// NOLINTEND(*-avoid-magic-numbers)

} // namespace dice
#endif // DICE_DEFINITIONS_HPP