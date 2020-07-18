#include "catch.hpp"
#include <tuple>
#include <array>

using std::string;
using std::vector;

template<typename... T>
bool EmptyVariadicTypes(T...) {
  return true;
}

TEST_CASE("Test Empty Varadic Types") {
  CHECK(EmptyVariadicTypes());
}

template<typename T, typename... U>
bool NoneEmptyVariadicTypes() {
  return true;
}

TEST_CASE("Test None Empty Variadic Types") {
  // CHECK(NoneEmptyVariadicTypes());
}

template<typename... T>
std::enable_if_t<std::conjunction_v<std::is_same<int, T>...>, size_t>
VariadicInt(T... arguments) {
  return sizeof...(arguments);
}

TEST_CASE("Test Varadic Int") {
  CHECK(VariadicInt(1, 2, 3) == 3);
  CHECK(VariadicInt(1) == 1);
  CHECK(VariadicInt() == 0);
  // CHECK(VariadicInt(5.2));
  // CHECK(VariadicInt(5l));
  // CHECK(VariadicInt(5, 2l));
  // CHECK(VariadicInt(5, '3'));
}

template<typename T, typename... U>
std::enable_if_t<std::is_same_v<int, T>, int>
Maximum(T argument, U... arguments) {
  int max_value = Maximum(arguments...);
  return argument > max_value ? argument : max_value;
}

template<>
int Maximum<int>(int argument) {
  return argument;
}

TEST_CASE("Test Maximum") {
  CHECK(Maximum(5) == 5);
  CHECK(Maximum(5, 65, 7, -12, 0) == 65);
  // CHECK(Maximum(5, 65, 7, -12l, 0) == 65);
  // CHECK(Maximum());
}

TEST_CASE("Test Tuple") {
  std::tuple<int, string, vector<int>, vector<string>> tuple;
  std::get<0>(tuple) = 9999;
  CHECK(std::get<1>(tuple).empty());
  CHECK(std::get<2>(tuple).empty());
  std::get<3>(tuple).push_back("123");
  CHECK(std::get<3>(tuple)[0] == "123");
  CHECK(
    std::is_same_v<
    std::tuple_element_t<
    2,
    std::tuple<int, string, vector<int>, vector<string>>
    >,
    vector<int>
    >
  );
}

template<size_t index, typename... T>
std::tuple_element_t<index, std::tuple<std::remove_reference_t<T>...>> get(T&&... arguments) {
  return std::get<index>(std::forward_as_tuple(std::forward<T>(arguments)...));
}

TEST_CASE("Test get") {
  vector<string> temp{ "+101", "+999", "-100" };
  CHECK(get<0>(1, string{ "531" }, temp) == 1);
  CHECK(get<1>(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" }) == string{ "531" });
  vector<string> result = get<2>(1, string{ "531" }, vector<string>{ "+101", "+999", "-100" });
  CHECK(result.size() == 3);
  CHECK(result[0] == "+101");
  CHECK(result[1] == "+999");
  CHECK(result[2] == "-100");
}

template<typename... T>
size_t IntTypeCount() {
  size_t int_count = 0;
  for (size_t i = 0; i < sizeof...(T); ++i) {
    if (std::is_same_v<
      int,
      std::tuple_element_t<i, std::tuple<T...>>
    >) {
      ++i;
    }
  }
  return int_count;
}

template<typename... T>
struct IntCount;

template<typename T, typename... U>
struct IntCount<T, U...> : std::integral_constant<size_t, (std::is_same_v<T, int> ? 1 : 0) + IntCount<U...>::value> {};

template<>
struct IntCount<> : std::integral_constant<size_t, 0> {};

TEST_CASE("Test Int Type Count") {
  // CHECK(IntTypeCount<int, int, string>() == 2);
  CHECK(IntCount<>::value == 0);
  CHECK(IntCount<int>::value == 1);
  CHECK(IntCount<int, int>::value == 2);
  CHECK(IntCount<string, int>::value == 1);
  CHECK(IntCount<int, int, vector<int>, string>::value == 2);
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
  ( (result.at(I) = ParseGeneric(std::get<I>(arguments))), ... );
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
  CHECK(result[1] == 321);
  CHECK(result[2] == -85);
  CHECK(test_vector_2.size() == 3);
  CHECK(test_vector_2[0] == "-99");
  CHECK(test_vector_2[1] == "+15");
  CHECK(test_vector_2[2] == "-1");
}
