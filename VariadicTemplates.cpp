#include "catch.hpp"
#include <tuple>
#include <array>

using std::string;
using std::vector;

class TestClass {

};

template<class T>
struct is_vector {
  using value = std::false_type;
};

template<class T>
struct is_vector<vector<T>> {
  using value = std::true_type;
};

template<typename T>
struct DoStaff {
  int operator()(const std::enable_if_t<is_vector<T>::value, T>& argument) {
    int result = 0;
    DoStaff<T::value_type> policy;
    for (const T::value_type& item : argument) {
      result += policy(item);
    }
    return result;
  }
};

template<>
struct DoStaff<int> {
  int operator()(int argument) {
    return argument * 2;
  }
};

template<>
struct DoStaff<string> {
  int operator()(const std::string& argument) {
    return std::stoi(argument);
  }
};

template<typename... T>
std::array<int, sizeof...(T)> DoSomething(const T&...) {
  std::array<int, sizeof...(T)> result;
}

template<typename... T, std::size_t... I>
std::array<int, sizeof...(T)> Loop(std::tuple<T&...> argument, std::index_sequence<I...>) {
  

  return std::apply([std::array<int, sizeof...(T)>& result](auto&&... args)->std::array<int, sizeof...(T)> {

  });

  for (size_t i = 0; i < sizeof...(T); ++i) {
    DoStaff<std::tuple_element_t<i, argument>> policy;
    result[i] = policy(std::get<i>(argument));
  }
  return result;
}

template<typename... T>
std::array<int, sizeof...(T)> Loop(const T&... arguments) {
  return Loop(std::tie(arguments...), std::index_sequence_for<T...>);
}

TEST_CASE("Test Loop Variadic Template") {
  std::array<int, 3> result = Loop<int, string, vector<string>>(1, "531", { "+101", "+999", "-100" });
  CHECK(result[0] == 1);
  CHECK(result[1] == 531);
  CHECK(result[2] == 1000);
}

class VariadicTypeStorage {

};
