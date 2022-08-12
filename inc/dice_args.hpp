#ifndef DICE_DICE_ARGS_HPP
#define DICE_DICE_ARGS_HPP

#include "args.hpp"

#include <iostream>
#include <limits>

namespace dice {
enum class shape
{
  hexahedron,
  octahedron,
  pent_trapezohedron,
  dodecahedron,
  icosahedron,
};
} // namespace dice

namespace args {
template <>
struct value_conv<::dice::shape>
{
  std::optional<::dice::shape> operator()(std::string_view sv)
  {
    if ( sv == "hexahedron" || sv == "d6" ) { return ::dice::shape::hexahedron; }
    if ( sv == "octahedron" || sv == "d8" ) { return ::dice::shape::octahedron; }
    if ( sv == "pent_trapezohedron" || sv == "d10" ) { return ::dice::shape::pent_trapezohedron; }
    if ( sv == "dodecahedron" || sv == "d12" ) { return ::dice::shape::dodecahedron; }
    if ( sv == "icosahedron" || sv == "d20" ) { return ::dice::shape::icosahedron; }
    return {};
  }
};
} // namespace args

namespace dice {

struct dicefinder_options_result
{
  float point_sdev;
  float face_sdev;
  shape die;
};

inline std::optional<dicefinder_options_result>
dicefinder_options(char** argv, std::ostream& error_stream = std::cerr)
{
  const auto args = args::parse(
    argv,
    args::named<float> {
      .id    = "point-sdev",
      .sname = 'p',
      .init  = std::numeric_limits<float>::max(),
      .alt   = "float",
    },
    args::named<float> {
      .id    = "face-sdev",
      .sname = 'f',
      .init  = std::numeric_limits<float>::max(),
      .alt   = "float",
    },
    args::positional<shape> {
      .id = "shape",
    });

  if ( args.status != args.good )
  {
    error_stream << "issues:\n";
    for ( const auto& m : args.messages ) { error_stream << " - " << m << '\n'; }
    error_stream << "\nusage: " << args.synopsis << "\n\n";
    return {};
  }

  return dicefinder_options_result {
    .point_sdev = args.getopt<float>("point-sdev").value(),
    .face_sdev  = args.getopt<float>("face-sdev").value(),
    .die        = args.getopt<shape>("shape").value(),
  };
}

} // namespace dice

#endif // DICE_DICE_ARGS_HPP