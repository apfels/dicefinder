#ifndef DICE_FAST_BITSET_HPP
#define DICE_FAST_BITSET_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

namespace dice {

class fast_bitset
{
  // this is larger than vector<bool>, but potentially faster and not as tricky to use right.
  std::vector<char8_t> bits {};

public:
  explicit fast_bitset(std::size_t n)
  : bits(n)
  {}

  fast_bitset& operator++()
  {
    for ( auto i { bits.size() }; i > 0; --i )
    {
      // simply binary addition
      if ( bool(bits[i - 1]) ) { bits[i - 1] = 0; }
      else
      {
        bits[i - 1] = 1;
        break;
      }
    }
    return *this;
  }

  // forward some accessor functions
  decltype(auto) operator[](std::size_t i) { return bits[i]; }

  decltype(auto) operator[](std::size_t i) const { return bits[i]; }

  decltype(auto) size() { return bits.size(); }

  void unset_all() { std::ranges::fill(bits, 0); }
};

} // namespace dice
#endif // DICE_FAST_BITSET_HPP