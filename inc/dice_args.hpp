#ifndef DICE_DICE_ARGS_HPP
#define DICE_DICE_ARGS_HPP

#include "args.hpp"
#include "dice_definitions.hpp"

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
  rhombic_triacontahedron,
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
    if ( sv == "rhombic_triacontahedron" || sv == "d30" )
    {
      return ::dice::shape::rhombic_triacontahedron;
    }
    return {};
  }
};
} // namespace args

namespace dice {

inline const sparse_topology& shape_topology(shape s)
{
  switch ( s )
  {
  case shape::hexahedron : return hexahedron();
  case shape::octahedron : return octahedron();
  case shape::pent_trapezohedron : return pent_trapezohedron();
  case shape::dodecahedron : return dodecahedron();
  case shape::icosahedron : return icosahedron();
  case shape::rhombic_triacontahedron : return rhombic_triacontahedron();
  default : return hexahedron(); // gcc doesn't like no default
  }
}

struct dicefinder_options_result
{
  float                  point_sdev;
  float                  face_sdev;
  const sparse_topology& shape;
  bool                   print_progress;
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
    args::named<bool> {
      .id    = "print-progress",
      .sname = 'v',
      .init  = false,
      .flag  = true,
      .alt   = "bool",
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
    .point_sdev     = args.getopt<float>("point-sdev").value(),
    .face_sdev      = args.getopt<float>("face-sdev").value(),
    .shape          = shape_topology(args.getopt<shape>("shape").value()),
    .print_progress = args.getopt<bool>("print-progress").value(),
  };
}

} // namespace dice

#endif // DICE_DICE_ARGS_HPP