#include "args.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

enum class shape
{
  hexahedron,
  octahedron,
  pent_trapezohedron,
  dodecahedron,
  icosahedron,
};

template <>
struct args::value_conv<shape>
{
  std::optional<shape> operator()(std::string_view sv)
  {
    if ( sv == "hexahedron" || sv == "d6" ) { return shape::hexahedron; }
    if ( sv == "octahedron" || sv == "d8" ) { return shape::octahedron; }
    if ( sv == "pent_trapezohedron" || sv == "d10" ) { return shape::pent_trapezohedron; }
    if ( sv == "dodecahedron" || sv == "d12" ) { return shape::dodecahedron; }
    if ( sv == "icosahedron" || sv == "d20" ) { return shape::icosahedron; }
    return {};
  }
};

int main(int /*argc*/, char** argv)
{
  auto args = args::parse(
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
    std::cerr << "issues:\n";
    for ( const auto& m : args.messages ) { std::cerr << " - " << m << '\n'; }
    std::cerr << "\nusage: " << args.synopsis << "\n\n";
    return EXIT_FAILURE;
  }

  std::cout << args.getopt<float>("point-sdev").value() << '\n';

  const auto start_time { std::chrono::steady_clock::now() };
  std::cerr << "Hello.\n";

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
