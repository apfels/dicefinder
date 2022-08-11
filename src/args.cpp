#include "args.hpp"

#include <ranges>

namespace args {

std::vector<std::string_view> view_string_array(char** argv)
{
  std::vector<std::string_view> args;
  for ( auto *i { argv }, str { argv[0] }; str; str = *(++i) ) // NOLINT
  {
    args.emplace_back(str);
  }
  return args;
};

struct option_info
{
  std::string_view              name;    // Name of main option that can have a value
  args_map::arg_value           value;   // Value, if any found
  std::vector<std::string_view> more {}; // This is for posix style joined switches like -xvf
};

option_info extract_single_arg(std::string_view arg, std::span<flag_match> flags)
{
  // special case, single dash is complete argument
  if ( arg == "-" ) { return { {}, arg }; }

  auto equals_pos = arg.find('=');

  // dd style
  if ( !arg.starts_with('-') && arg.contains('=') )
  {
    return {
      std::string_view {                 arg.begin(), arg.begin() + equals_pos},
      std::string_view {arg.begin() + equals_pos + 1,                arg.end()}
    };
  }

  // gnu style with space
  if ( arg.starts_with("--") && !arg.contains('=') )
  {
    std::string_view real_arg { arg.begin() + 2, arg.end() };

    bool is_flag =
      std::ranges::find_if(flags, [&](const flag_match& f) { return real_arg == f.lname; }) !=
      flags.end();

    if ( is_flag )
    {
      return { {}, {}, std::vector { std::string_view { arg.begin() + 2, arg.end() } } };
    }

    return {
      {arg.begin() + 2, arg.end() },
      {           }
    };
  }

  // posix style with space (keep below gnu style)
  if ( arg.starts_with('-') && !arg.contains('=') )
  {
    // posix style can have multiple valueless switches, or one valued option
    if (
      std::ranges::find_if(flags, [&](const flag_match& f) { return f.sname == arg[1]; }) ==
      flags.end() )
    {
      if ( arg.size() == 2 )
      {
        return {
          {arg.begin() + 1, arg.begin() + 2 },
          {                 }
        };
      }
      return {
        std::string_view {arg.begin() + 1, arg.begin() + 2},
        std::string_view {arg.begin() + 2,       arg.end()}
      };
    }

    // starts and ends with flag
    if (
      std::ranges::find_if(flags, [&](const flag_match& f) { return f.sname == arg.back(); }) !=
      flags.end() )
    {
      std::vector<std::string_view> more;
      for ( auto i { 1z }; i < arg.size(); ++i )
      {
        more.emplace_back(arg.begin() + i, arg.begin() + i + 1);
      }
      return { {}, {}, more };
    }

    // starts with flag, ends with valued
    std::vector<std::string_view> more;
    for ( auto i { 1z }; i < arg.size() - 1; ++i )
    {
      more.emplace_back(arg.begin() + i, arg.begin() + i + 1);
    }
    return {
      {arg.end() - 1, arg.end() },
      {           },
      more
    };
  }

  // posix or gnu style with equals
  if ( arg.starts_with('-') && arg.contains('=') )
  {
    return {
      std::string_view {arg.begin() + arg.find_first_not_of('-'), arg.begin() + equals_pos},
      std::string_view {            arg.begin() + equals_pos + 1,                arg.end()}
    };
  }

  // no option detected, just value
  return { {}, arg };
}

void named_in_place(args_map& result, auto begin, auto end, std::span<flag_match> flags)
{
  for ( auto it { begin }; it < end; ++it )
  {
    auto current { *it };
    auto opt { extract_single_arg(current, flags) };

    for ( auto m : opt.more ) { result.named.emplace_back(m, std::optional<std::string_view> {}); }

    bool no_value = !opt.value || opt.value->empty();

    if ( opt.name.empty() && no_value ) {}
    else if ( opt.name.empty() ) { result.positional.push_back(current); }
    else if ( no_value && it + 1 != end )
    {
      auto nextopt { extract_single_arg(*(it + 1), flags) };

      // If next is not a named option, use as value and skip it.
      if ( nextopt.name.empty() && nextopt.more.empty() )
      {
        result.named.emplace_back(opt.name, nextopt.value);
        ++it;
      }

      // If next is a named option as well, can't be this options's value.
      else { result.named.emplace_back(opt.name, opt.value); }
    }
    else { result.named.emplace_back(opt.name, opt.value); }
  }
}

args_map extract_args(char** argv, std::span<flag_match> flags)
{
  const auto args { view_string_array(argv) };

  args_map result;
  result.invocation = args[0];
  const auto ddash { std::ranges::find(args, "--") };
  named_in_place(result, args.begin() + 1, ddash, flags);

  if ( !(ddash == args.end()) )
  {
    for ( auto param : std::vector(ddash + 1, args.end()) ) { result.positional.push_back(param); }
  }

  return result;
}

} // namespace args