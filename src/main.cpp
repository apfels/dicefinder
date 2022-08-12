#include "args.hpp"
#include "dice_args.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

int main(int /*argc*/, char** argv)
{
  auto options = dice::dicefinder_options(argv);
  if ( !options ) { return EXIT_FAILURE; }

  std::cout << "keep points < " << options->point_sdev << '\n';
  std::cout << "keep faces  < " << options->face_sdev << '\n';
  std::cout << "shape: " << (int)options->die << '\n';

  const auto start_time { std::chrono::steady_clock::now() };
  std::cerr << "Hello.\n";

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
