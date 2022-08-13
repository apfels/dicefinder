#ifndef DICE_FIND_DICE_HPP
#define DICE_FIND_DICE_HPP

#include "dice_definitions.hpp"
#include "multi_call.hpp"
#include "weighting.hpp"

#include <iostream>
#include <mutex>
#include <vector>

namespace dice {

struct find_dice_settings
{
  const sparse_topology& topo;
  std::size_t            batch_size;
  float                  point_max_sdev;
  float                  face_max_sdev;
  bool                   print_progress;
};

inline auto find_dice(find_dice_settings&& settings)
{
  using die_result = std::pair<die_weight, face_values>;

  auto                    die { make_dense(settings.topo) };
  layout                  main_layout(die.pairs.size());
  std::vector<layout>     layout_batches;
  std::vector<die_result> all_results;
  std::mutex              result_mutex;

  std::atomic<std::size_t> done_batches;
  std::size_t              n_batches {};

  while ( !main_layout.past_end() )
  {
    layout_batches.push_back(main_layout);
    main_layout.batch_advance(settings.batch_size);
    ++n_batches;
  }

  if ( settings.print_progress ) { std::cerr << n_batches << " batches:\n"; }

  multi_call(
    [&](layout lo)
    {
      auto batch { 0z };

      std::vector<die_result> results;
      while ( batch < settings.batch_size && !lo.past_end() )
      {
        auto values { values_from(die, lo) };
        if ( is_lexical_rotation(die, values) )
        {
          auto w { make_weight(die, values) };
          if ( w.points.sdev <= settings.point_max_sdev && w.faces.sdev <= settings.face_max_sdev )
          {
            results.emplace_back(w, values);
          }
        }
        if ( lo.advance() ) { ++batch; }
      }

      { // lock_guard scope
        std::lock_guard l { result_mutex };
        all_results.insert(all_results.end(), results.begin(), results.end());
      }
      if ( settings.print_progress )
      {
        // This is a multithreaded environment, so concat all strings before calling operator<<.
        std::cerr << std::to_string(++done_batches) + "... ";
      }
    },
    layout_batches);

  if ( settings.print_progress ) { std::cerr << "DONE\n\n"; }

  return all_results;
}

} // namespace dice
#endif // DICE_FIND_DICE_HPP