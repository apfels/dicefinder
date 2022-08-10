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
  std::string_view name;
  std::string_view value;
  std::string_view more {}; // This is needed for posix style joined switches like -xvf
};

option_info extract_single_arg(std::string_view arg, std::string_view posix_switches = {})
{
  auto equals_pos = arg.find('=');

  // dd style
  if ( !arg.starts_with('-') && arg.contains('=') )
  {
    return {
      {                 arg.begin(), arg.begin() + equals_pos},
      {arg.begin() + equals_pos + 1,                arg.end()}
    };
  }

  // gnu style with space
  if ( arg.starts_with("--") && !arg.contains('=') )
  {
    return {
      {arg.begin() + 2, arg.end() },
      {           }
    };
  }

  // posix style with space (keep below gnu style)
  if ( arg.starts_with('-') && !arg.contains('=') )
  {
    // posix style can have multiple valueless switches, or one valued option
    if ( !posix_switches.contains(arg[1]) )
    {
      return {
        {arg.begin() + 1, arg.begin() + 2},
        {arg.begin() + 2,       arg.end()}
      };
    }

    // clang-format off
    if ( posix_switches.contains(arg.back()) )
    {
      return { {}, {}, { arg.begin()+1,arg.end() } };
    }

    return { { arg.end()-1,arg.end() }, {}, { arg.begin()+1,arg.end()-1 } };
    // clang-format on
  }

  // posix or gnu style with equals
  if ( arg.starts_with('-') && arg.contains('=') )
  {
    return {
      {arg.begin() + arg.find_first_not_of('-'), arg.begin() + equals_pos},
      {            arg.begin() + equals_pos + 1,                arg.end()}
    };
  }

  // no option detected, just value
  return { {}, arg };
}

args_map extract_args(char** argv, std::string_view posix_switches)
{
  const auto args { view_string_array(argv) };
  args_map   result;

  result.invocation = args[0];
  const auto ddash { std::ranges::find(args, "--") };

  for ( auto it { args.begin() + 1 }; it < ddash; ++it )
  {
    auto current { *it };
    auto opt { extract_single_arg(current, posix_switches) };

    for ( auto i { 0z }; i < opt.more.size(); ++i )
    {
      result.named.emplace(
        std::string_view { opt.more.begin() + i, opt.more.begin() + i + 1 },
        std::string_view {});
    }

    if ( opt.name.empty() && opt.value.empty() )
    { /* skip */
    }
    else if ( opt.name.empty() ) { result.positional.push_back(current); }
    else if ( opt.value.empty() && it + 1 != args.end() )
    {
      auto nextopt { extract_single_arg(*(it + 1), posix_switches) };

      // If next is not a named option, use it as value and skip.
      if ( nextopt.name.empty() )
      {
        result.named.emplace(opt.name, nextopt.value);
        ++it;
      }
      // If next is a named option as well, it's not this options's value.
      else { result.named.emplace(opt.name, opt.value); }
    }
    else { result.named.emplace(opt.name, opt.value); }
  }

  if ( !(ddash == args.end()) )
  {
    for ( auto param : std::vector(ddash + 1, args.end()) ) { result.positional.push_back(param); }
  }

  return result;
}

} // namespace args