#include <chrono>
#include <cstdlib>
#include <iostream>


int main(int argc, char** argv)
{
  for(auto i{0z}; i<argc; ++i) { std::cout << argv[i]; } // NOLINT
  std::cout << '\n';

  const auto start_time { std::chrono::steady_clock::now() };
  std::cerr << "Hello.\n";

  const auto end_time { std::chrono::steady_clock::now() };
  std::cerr << "Bye. ("
            << std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time)
            << ")\n";

  return EXIT_SUCCESS;
}
