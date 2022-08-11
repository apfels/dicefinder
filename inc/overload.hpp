#ifndef OVERLOAD_HPP
#define OVERLOAD_HPP

template <typename... Ts>
struct overload : Ts...
{
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

#endif // OVERLOAD_HPP