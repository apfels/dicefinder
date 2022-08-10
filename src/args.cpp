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
  std::string_view name;    // Name of main option that can have a value
  std::string_view value;   // Value, if any found
  std::string_view more {}; // This is for posix style joined switches like -xvf
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

void named_in_place(args_map& result, auto begin, auto end, std::string_view posix_switches)
{
  for ( auto it { begin }; it < end; ++it )
  {
    auto current { *it };
    auto opt { extract_single_arg(current, posix_switches) };

    for ( auto i { 0z }; i < opt.more.size(); ++i )
    {
      result.named.emplace(
        std::string_view { opt.more.begin() + i, opt.more.begin() + i + 1 },
        std::string_view {});
    }

    if ( opt.name.empty() && opt.value.empty() ) {}
    else if ( opt.name.empty() ) { result.positional.push_back(current); }
    else if ( opt.value.empty() && it + 1 != end )
    {
      auto nextopt { extract_single_arg(*(it + 1), posix_switches) };

      // If next is not a named option, use as value and skip it.
      if ( nextopt.name.empty() )
      {
        result.named.emplace(opt.name, nextopt.value);
        ++it;
      }

      // If next is a named option as well, can't be this options's value.
      else { result.named.emplace(opt.name, opt.value); }
    }
    else { result.named.emplace(opt.name, opt.value); }
  }
}

args_map extract_args(char** argv, std::string_view posix_switches)
{
  const auto args { view_string_array(argv) };

  args_map result;
  result.invocation = args[0];
  const auto ddash { std::ranges::find(args, "--") };
  named_in_place(result, args.begin() + 1, ddash, posix_switches);

  if ( !(ddash == args.end()) )
  {
    for ( auto param : std::vector(ddash + 1, args.end()) ) { result.positional.push_back(param); }
  }

  return result;
}

} // namespace args