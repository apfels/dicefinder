#ifndef DICE_ARGS_HPP
#define DICE_ARGS_HPP

#include "overload.hpp"

#include <any>
#include <charconv>
#include <optional>
#include <ostream>
#include <span>
#include <string_view>
#include <unordered_map>
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

constexpr auto replace = []<typename T>(T, T second) { return std::forward<T>(second); };

template <typename T>
struct named
{
  using value_type = T;

  std::string_view id;
  std::string_view lname { id };
  char             sname { lname[0] };
  std::optional<T> init {};
  std::optional<T> flag {};
  T (*reduce)(T, T) { replace };

  std::string_view alt { id };
  std::string_view help {};
};

template <typename T>
struct positional
{
  using value_type = T;

  std::string_view id;
  std::optional<T> init {};
  T (*reduce)(T, T) { replace };

  std::string_view alt { id };
  std::string_view help {};
};

struct opt_result
{
  std::size_t count {};
  std::any    value {};
};

struct parse_result
{
  enum status_e : unsigned char
  {
    good = 0,
    warn = 1,
    err  = 2,
  } status { good };

  std::vector<std::string>                       messages;
  std::string                                    synopsis;
  std::unordered_map<std::string_view, std::any> opts;

  void raise(status_e s, std::string&& msg)
  {
    if ( s > status ) { status = s; }
    messages.push_back(msg);
  }

  template <typename T>
  std::optional<T> getopt(std::string_view id)
  {
    return std::any_cast<std::optional<T>>(opts.at(id));
  }
};

template <typename T>
struct value_conv;

template <typename T>
concept arithmetic = std::floating_point<T> || std::integral<T>;

template <arithmetic T>
struct value_conv<T>
{
  std::optional<T> operator()(std::string_view sv)
  {
    T number;

    auto res { std::from_chars(sv.begin(), sv.end(), number) };
    if ( res.ec != std::errc {} ) { return {}; }

    return number;
  }
};

template <>
struct value_conv<std::string_view> : std::identity
{
  using std::identity::operator();
};

// This is a lambda because it's used in an overload{}
constexpr auto match_named =
  []<typename T>(parse_result& result, const args_map::arg_entry& arg, const named<T>& opt)
{
  // Main test here. Is this the option we're looking for?
  if ( (arg.first.size() == 1 && arg.first.starts_with(opt.sname)) || arg.first == opt.lname )
  {
    if ( opt.flag && arg.second )
    {
      result.raise(parse_result::err, std::string(arg.first) + " takes no argument");
    }
    else if ( !opt.flag && !arg.second )
    {
      result.raise(parse_result::err, std::string(arg.first) + " needs argument");
    }
    else if ( opt.flag )
    {
      auto old_val { std::any_cast<std::optional<T>>(result.opts[opt.id]) };
      if ( old_val ) { result.opts[opt.id] = std::optional<T> { opt.reduce(*old_val, *opt.flag) }; }
      else { result.opts[opt.id] = opt.flag; }
    }
    else
    {
      auto val { value_conv<T> {}(*arg.second) };
      if ( !val )
      {
        result.raise(
          parse_result::err,
          std::string(arg.first) + " can't recognize argument '" + std::string(*arg.second) + "'");
      }
      else
      {
        auto old_val { std::any_cast<std::optional<T>>(result.opts[opt.id]) };
        if ( old_val ) { result.opts[opt.id] = std::optional<T> { opt.reduce(*old_val, *val) }; }
        else { result.opts[opt.id] = val; }
      }
    }

    return true;
  }
  return false;
};

template <typename... Ts>
std::string synopsis(std::string_view app_name, Ts... opts)
{
  std::string msg { std::string { app_name } + ' ' };

  std::string flags_string {};

  const auto posix_flag = overload {
    [&]<typename T>(const named<T>& o)
    {
      if ( o.sname && o.flag ) { flags_string.append({ o.sname }); }
    },
    [](const auto&) {},
  };
  (..., posix_flag(opts));

  if ( !flags_string.empty() ) { msg += std::string { "[-" } + flags_string + "] "; }

  const auto gnu_flag = overload {
    [&]<typename T>(const named<T>& o)
    {
      if ( !o.sname && !o.lname.empty() && o.flag )
      {
        msg += std::string { "[--" } + std::string { o.lname } + "] ";
      }
    },
    [](const auto&) {},
  };
  (..., gnu_flag(opts));

  const auto valued = overload {
    [&]<typename T>(const named<T>& o)
    {
      if ( !o.flag && (o.sname || !o.lname.empty()) )
      {
        msg += '[';
        if ( o.sname ) { msg += std::string { '-' } + o.sname; }
        if ( o.sname && !o.lname.empty() ) { msg += '|'; }
        if ( !o.lname.empty() ) { msg += std::string { "--" } + std::string { o.lname }; }
        msg += std::string { ' ' } + std::string { o.alt } + "] ";
      }
    },
    [](const auto&) {},
  };
  (..., valued(opts));

  const auto operands = overload {
    [&]<typename T>(const positional<T>& o)
    { msg += std::string { '<' } + std::string { o.id } + "> "; },
    [](const auto&) {},
  };
  (..., operands(opts));

  return msg;
};

template <typename... Ts>
auto parse(char** argv, Ts... opts)
{
  std::vector<flag_match> posix_flags;

  const auto push_if_flag = overload {
    [&]<typename T>(const named<T>& o)
    {
      if ( o.flag ) { posix_flags.push_back({ o.sname, o.lname }); }
    },
    [](const auto& p) {},
  };
  (..., push_if_flag(opts));

  auto         args { extract_args(argv, posix_flags) };
  parse_result result;

  result.synopsis = synopsis(args.invocation, opts...);

  const auto init_results = [&](const auto& opt) { result.opts.emplace(opt.id, opt.init); };

  (..., init_results(opts));

  const auto test_named = overload {
    match_named,
    [](auto&, const auto&, const auto&) { return false; },
  };

  for ( const auto& arg : args.named )
  {
    bool matched = (... || test_named(result, arg, opts));
    if ( !matched )
    {
      result.raise(parse_result::err, "option '" + std::string(arg.first) + "' not known");
    }
  }

  return result;
}

} // namespace args
#endif // DICE_ARGS_HPP