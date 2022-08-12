#ifndef DICE_MULTICALL_HPP
#define DICE_MULTICALL_HPP

#include <atomic>
#include <ranges>
#include <thread>
#include <vector>

namespace dice {

// clang-format <15 is not very good with requires
// clang-format off

template <std::ranges::random_access_range R>
auto multi_call(
  auto&& fn,
  const R& datasets,
  std::size_t n_jobs)
requires requires { fn(std::declval< typename R::value_type >()); }

// clang-format on
{
  if ( n_jobs < 1 ) { throw std::logic_error("no jobs allocated to multi_call."); }

  const auto         size { std::ranges::size(datasets) };
  std::atomic_size_t count {};

  const auto job_fn = [&]()
  {
    for ( ;; )
    {
      std::size_t i { count++ };
      if ( i >= size ) { break; }
      fn(*(std::ranges::begin(datasets) + i));
    }
  };

  std::vector<std::thread> jobs;
  for ( auto i { 0uz }; i < n_jobs; ++i ) { jobs.emplace_back(job_fn); }
  for ( auto& job : jobs ) { job.join(); };
}

} // namespace dice

#endif // DICE_MULTICALL_HPP