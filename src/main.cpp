#include "args.hpp"
#include "dice_args.hpp"
#include "multi_call.hpp"
#include "weighting.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

constexpr std::size_t batch_size { 2000 };

int main(int /*argc*/, char** argv)
{
  const auto start_time { std::chrono::steady_clock::now() };
  std::cerr << "Hello.\n";

  auto options = dice::dicefinder_options(argv);
  if ( !options ) { return EXIT_FAILURE; }

  std::cerr << "points < " << options->point_sdev << '\n'
            << "faces  < " << options->face_sdev << '\n';

  auto         cores = std::thread::hardware_concurrency();
  dice::layout layout(options->die.pairs.size());

  std::vector<dice::layout> layout_data;

  while ( !layout.past_end() )
  {
    layout_data.push_back(layout);
    layout.batch_advance(batch_size);
  }

  using die_result = std::pair<dice::die_weight, dice::face_values>;

  std::vector<die_result> all_results;
  std::mutex              result_mutex;

  try
  {
    dice::multi_call(
      [&](dice::layout lo)
      {
        auto batch { 0z };

        std::vector<die_result> results;
        while ( batch < batch_size && !lo.past_end() )
        {
          auto values { dice::values_from(options->die, lo) };
          if ( dice::is_lexical_rotation(options->die, values) )
          {
            auto w { dice::make_weight(options->die, values) };
            if ( w.points.stdev <= options->point_sdev && w.faces.stdev <= options->face_sdev )
            {
              results.emplace_back(w, values);
            }
          }
          if ( lo.advance() ) { ++batch; }
        }
        { // lock guard scope
          std::lock_guard l { result_mutex };
          all_results.insert(all_results.end(), results.begin(), results.end());
        }
      },
      layout_data,
      cores > 0 ? cores : 4);
  }
  catch ( std::exception& e )
  {
    std::cerr << "aborted: multi_call exception: " << e.what();
    return EXIT_FAILURE;
  }

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
