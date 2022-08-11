#ifndef DICE_ARGS_HPP
#define DICE_ARGS_HPP

#include "overload.hpp"

#include <any>
#include <optional>
#include <ostream>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

namespace args {

struct args_map
{
  using arg_value = std::optional<std::string_view>;
  using arg_entry = std::pair<std::string_view, arg_value>;
  std::string_view              invocation;
  std::vector<arg_entry>        named;
  std::vector<std::string_view> positional;

  friend std::ostream& operator<<(std::ostream& os, const args_map& a)
  {
    os << a.invocation << ": ";
    for ( const auto& n : a.named )
    {
      os << n.first;
      if ( n.second ) { os << "='" << *n.second << "', "; }
      else { os << "={}, "; }
    }
    for ( const auto& p : a.positional ) { os << "'" << p << "', "; }
    return os;
  }
};

struct flag_match
{
  char             sname;
  std::string_view lname;
};

// interpretation depends on what options take values and which are valueless flags.
args_map extract_args(char** argv, std::span<flag_match> flags);

constexpr auto replace = []<typename T>(T&&, T&& second) { return std::forward<T>(second); };

template <typename T>
struct named
{
  using value_type = T;

  std::string_view id;
  std::string_view lname { id };
  char             sname { lname[0] };

  std::optional<T> flag {};
  std::optional<T> init {};
  T (*reduce)(T&&, T&&) { replace };

  bool             required { false };
  std::string_view help {};
};

template <typename T>
struct positional
{
  using value_type = T;

  std::string_view id;

  bool             required { false };
  std::string_view help {};
};

struct opt_result
{
  std::size_t count {};
  std::any    value {};
};

struct parse_result
{};

template <typename... Ts>
auto parse(char** argv, Ts... opts)
{
  std::vector<flag_match> posix_flags;

  const auto push_if_flag = overload {
    [&]<typename T>(const named<T>& o)
    {
      if ( o.flag ) { posix_flags.push_back({ o.sname, o.lname }); }
    },
    []<typename T>(const positional<T>& p) {},
  };
  (..., push_if_flag(opts));

  auto args { extract_args(argv, posix_flags) };
  return args;
}

} // namespace args
#endif // DICE_ARGS_HPP