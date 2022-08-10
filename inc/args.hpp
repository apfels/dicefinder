#ifndef DICE_ARGS_HPP
#define DICE_ARGS_HPP

#include <ostream>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace args {

struct args_map
{
  std::string_view                                            invocation;
  std::unordered_multimap<std::string_view, std::string_view> named;
  std::vector<std::string_view>                               positional;

  friend std::ostream& operator<<(std::ostream& os, const args_map& a)
  {
    os << a.invocation << ": ";
    for ( const auto& n : a.named ) { os << n.first << "='" << n.second << "', "; }
    for ( const auto& p : a.positional ) { os << "'" << p << "', "; }
    return os;
  }
};

args_map extract_args(char** argv, std::string_view posix_switches = {});

} // namespace args
#endif // DICE_ARGS_HPP