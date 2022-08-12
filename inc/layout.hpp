#ifndef DICE_LAYOUT_HPP
#define DICE_LAYOUT_HPP

#include "fast_bitset.hpp"

namespace dice {

using face_value = std::uint16_t;

class layout
{
  fast_bitset                 face_flips;
  std::vector<face_value>     face_values;
  static constexpr face_value initial { 1 };

public:
  explicit layout(std::size_t half_n)
  : face_flips { half_n }
  {
    for ( auto i { 0uz }; i < half_n; ++i ) { face_values.push_back(i + initial); }
  }

  [[nodiscard]] face_value at(std::size_t i, bool alt_value) const
  {
    bool use_alt { bool(face_flips[i]) != alt_value };

    return use_alt ? (face_values.size() + initial) * 2 - 1 - face_values[i] : face_values[i];
  }

  // returns true if face_values changed
  bool advance()
  {
    ++face_flips;
    if ( bool(face_flips[0]) )
    {
      face_flips.unset_all();
      std::ranges::next_permutation(face_values);
      return true;
    }
    return false;
  }

  void batch_advance(std::size_t n)
  {
    for ( auto i { 0uz }; i < n; ++i ) { std::ranges::next_permutation(face_values); }
  }

  [[nodiscard]] bool past_end() const { return face_values[0] != initial; }
};

} // namespace dice
#endif // DICE_LAYOUT_HPP