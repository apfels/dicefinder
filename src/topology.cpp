#include "topology.hpp"

#include <algorithm>
#include <numeric>

namespace dice {

std::vector<dense_point> dense_points(const sparse_topology& sparse, point_index n_points)
{
  std::vector<dense_point> result;

  std::size_t n_corners {};
  for ( const auto& f : sparse.faces )
  {
    n_corners += f.point_indices.size();
    for ( const auto& p : f.point_indices ) { ++result[p].connectivity; }
  }

  auto avg_connectivity { float(n_corners) / float(n_points) };
  std::ranges::transform(
    result,
    result.begin(),
    [&](const dense_point& p)
    { return dense_point { .connectivity = p.connectivity / avg_connectivity }; });

  return result;
}

std::vector<dense_face> dense_faces(const sparse_topology& sparse)
{
  std::vector<dense_face> result;
  // this is a semisemi for because we need the actual index
  for ( auto old_face_i { 0uz }; old_face_i < sparse.faces.size(); ++old_face_i )
  {
    const auto& old_face { sparse.faces[old_face_i] };
    dense_face  new_face { old_face, {} };

    for ( auto candidate_face_i { 0uz }; candidate_face_i < sparse.faces.size();
          ++candidate_face_i )
    {
      if ( old_face_i != candidate_face_i )
      {
        const auto& candidate_face { sparse.faces[candidate_face_i] };

        auto n_common_points = std::ranges::count_if(
          old_face.point_indices,
          [&](auto i)
          {
            return std::ranges::find(candidate_face.point_indices, i) !=
              candidate_face.point_indices.end();
          });
        if ( n_common_points > 1 ) { new_face.neighbors.push_back(candidate_face_i); }
      }
    }
    result.push_back(new_face);
  }
  return result;
}

dense_topology make_dense(const sparse_topology& sparse)
{
  auto highest_point_index = std::transform_reduce(
    sparse.faces.begin(),
    sparse.faces.end(),
    point_index {},
    std::ranges::max,
    [](const auto& r) { return *std::ranges::max_element(r.point_indices); });
  auto n_points = highest_point_index + 1;

  return {
    .points   = dense_points(sparse, n_points),
    .faces    = dense_faces(sparse),
    .pairs    = sparse.pairs,
    .rotation = sparse.rotation,
  };
}

} // namespace dice