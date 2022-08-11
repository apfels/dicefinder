#include "args.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

int main(int /*argc*/, char** argv)
{
  auto args = args::parse(
    argv,
    args::named<float> { .id    = "point-sdev",
                         .sname = 'p',
                         .init  = std::numeric_limits<float>::max(),
                         .alt   = "float",
                         .help  = "maximum accepted standard deviation for point weights." },
    args::named<float> { .id    = "face-sdev",
                         .sname = 'f',
                         .init  = std::numeric_limits<float>::max(),
                         .alt   = "float",
                         .help  = "maximum accepted standard deviation for face weights." },
    args::positional<std::string_view> { .id   = "shape",
                                         .help = "die to process. d6/d8/d10/d12/d20" });

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
