#include "args.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

int main(int /*argc*/, char** argv)
{
  auto args = args::parse(
    argv,
    args::named<int> { .id = "use-alt", .sname = 'a', .flag = 2 },
    args::positional<std::string_view> { .id = "help" });

  std::cout << args << '\n';

  const auto start_time { std::chrono::steady_clock::now() };
  std::cerr << "Hello.\n";

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
