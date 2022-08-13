#include "args.hpp"
#include "dice_args.hpp"
#include "find_dice.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

constexpr std::size_t batch_size { 2000 };

int main(int /*argc*/, char** argv)
{
  const auto start_time { std::chrono::steady_clock::now() };
  auto       options = dice::dicefinder_options(argv);
  if ( !options ) { return EXIT_FAILURE; }

  auto results = dice::find_dice({
    .topo           = options->shape,
    .batch_size     = batch_size,
    .point_max_sdev = options->point_sdev,
    .face_max_sdev  = options->face_sdev,
    .print_progress = options->print_progress,
  });

  std::cerr << "Got " << results.size() << " results.\n";

  std::ranges::sort(results, std::greater {});

  for ( auto& r : results )
  {
    std::cout << r.first << " :: ";
    for ( auto f : r.second ) { std::cout << f << " "; }
    std::cout << "\n";
  }

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
