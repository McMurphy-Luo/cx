#include "catch.hpp"
#include <tuple>
#include <array>

using std::string;
using std::vector;

namespace
{
  class TestClass {

  };

  template<class T>
  struct is_vector {
    static constexpr bool value = false;
  };

  template<class T>
  struct is_vector<vector<T>> {
    static constexpr bool value = true;
  };
}

template<size_t index, typename... T>
std::tuple_element_t<index, std::tuple<T...>> get(T&&... arguments) {
  return std::get<index>(std::forward_as_tuple(arguments...));
}

TEST_CASE("Test traits") {
  CHECK(std::is_same<int, int>::value);
  if (std::is_same<int, int>::value) {
    std::putchar('Y');
  } else {
    std::putchar('N');
  }
}

TEST_CASE("Test get") {
  CHECK(get<0>(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" }) == 1);
  CHECK(get<1>(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" }) == string{ "531" });
  vector<string> result = get<2>(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" });
  CHECK(result.size() == 3);
  CHECK(result[0] == "+101");
  CHECK(result[1] == "+999");
  CHECK(result[2] == "-100");
}

int ParseGeneric(int argument) {
  return argument;
}

int ParseGeneric(const std::string& argument) {
  return std::stoi(argument);
}

template<typename T>
int ParseGeneric(const vector<T>& argument) {
  int result = 0;
  for (const vector<T>::value_type& item : argument) {
    result += ParseGeneric(item);
  }
  return result;
}

template<typename... T, size_t... I>
std::array<int, sizeof...(T)> ParseImpl(std::tuple<T&&...> arguments, std::index_sequence<I...>) {
  std::array<int, sizeof...(T)> result;
  ( void (result.at(I) = ParseGeneric(std::get<I>(arguments))), ... );
  return result;
}

template<typename... T>
std::array<int, sizeof...(T)> Parse(T&&... arguments) {
  return ParseImpl<T...>(std::forward_as_tuple(std::forward<T>(arguments)...), std::index_sequence_for<T...>{});
}

TEST_CASE("Test Loop Variadic Template") {
  std::array<int, 3> result = Parse(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" });
  CHECK(result[0] == 1);
  CHECK(result[1] == 531);
  CHECK(result[2] == 1000);

  vector<string> test_vector_2{ "-99", "+15", "-1" };
  result = Parse(11, string{ "321" }, test_vector_2);
  CHECK(result[0] == 11);
  CHECK(result[1] == 531);
  CHECK(result[2] == -85);
  CHECK(test_vector_2.size() == 3);
  CHECK(test_vector_2[0] == "-99");
  CHECK(test_vector_2[1] == "+15");
  CHECK(test_vector_2[1] == "-1");
}

class VariadicTypeStorage {

};
