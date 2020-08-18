#include "catch.hpp"
#include <tuple>
#include <array>

using std::string;
using std::vector;

namespace
{
  template<typename... T>
  bool EmptyVariadicTypes(T...) {
    return true;
  }
}

TEST_CASE("Test Empty Varadic Types") {
  CHECK(EmptyVariadicTypes());
}

namespace
{
  template<typename T, typename... U>
  bool NoneEmptyVariadicTypes() {
    return true;
  }
}

TEST_CASE("Test None Empty Variadic Types") {
  // CHECK(NoneEmptyVariadicTypes());
}

namespace
{
  template<typename... T>
  std::enable_if_t<std::conjunction_v<std::is_same<int, T>...>, size_t>
    VariadicInt(T... arguments) {
    return sizeof...(T);
  }
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

namespace
{
  template<typename... T>
  size_t IntTypeCountError() {
    size_t int_count = 0;
    for (size_t i = 0; i < sizeof...(T); ++i) {
      if constexpr (std::is_same_v<
        int,
        std::tuple_element_t<i, std::tuple<T...>>
      >) {
        ++i;
      }
    }
    return int_count;
  }

  template<typename... T>
  size_t IntTypeCount() {
    std::array<int, sizeof...(T)> int_count = { (std::is_same_v<T, int> ? 1 : 0)... };
    return std::count(std::begin(int_count), std::end(int_count), 1);
  }

  template<typename... T>
  struct IntCount;

  template<typename T, typename... U>
  struct IntCount<T, U...> : std::integral_constant<size_t, (std::is_same_v<T, int> ? 1 : 0) + IntCount<U...>::value> {};

  template<>
  struct IntCount<> : std::integral_constant<size_t, 0> {};
}

TEST_CASE("Test Int Type Count") {
  // CHECK(IntTypeCountError<int>() == 0);
  CHECK(IntTypeCount<>() == 0);
  CHECK(IntTypeCount<int>() == 1);
  CHECK(IntTypeCount<int, uintptr_t>() == 1);
  CHECK(IntTypeCount<string>() == 0);
  CHECK(IntTypeCount<int, int, vector<int>, string>() == 2);
  CHECK(IntCount<>::value == 0);
  CHECK(IntCount<int>::value == 1);
  CHECK(IntCount<int, int>::value == 2);
  CHECK(IntCount<string, int>::value == 1);
  CHECK(IntCount<int, int, vector<int>, string>::value == 2);
}

namespace
{
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

namespace
{
  template<size_t index, typename... T>
  std::tuple_element_t<index, std::tuple<std::remove_reference_t<T>...>> get(T&&... arguments) {
    return std::get<index>(std::forward_as_tuple(std::forward<T>(arguments)...));
  }
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

namespace
{
  int ParseGeneric(int argument) {
    return argument;
  }

  int ParseGeneric(const string& argument) {
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
    ((result.at(I) = ParseGeneric(std::get<I>(arguments))), ...);
    return result;
  }

  template<typename... T>
  std::array<int, sizeof...(T)> Parse(T&&... arguments) {
    return ParseImpl<T...>(std::forward_as_tuple(std::forward<T>(arguments)...), std::index_sequence_for<T...>{});
  }
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

namespace
{
  template<typename... T>
  vector<int> UnPackParameters(T... params) {
    return { (params + 1)... };
  }

  class Base1 {
  public:
    Base1(string value)
      : value_(std::move(value)) {

    }

    string GetValue() { return value_; }

  private:
    string value_;
  };

  class Base2 {
  public:
    Base2(int value)
      : value_(value) {

    }

    int GetValue() { return value_; }

  private:
    int value_;
  };

  template<typename... T>
  class UnPackClass : public T... {
  public:
    UnPackClass(const T&... bases) : T(bases)... {

    }

    auto GetValue() {
      return std::make_tuple(T::GetValue()...);
    }
  };
}

TEST_CASE("Test Parameter Pack") {
  vector<int> int_unpacked = UnPackParameters(-14, 99, 7, 31);
  CHECK(int_unpacked.size() == 4);
  CHECK(int_unpacked[3] == 32);
  CHECK(int_unpacked[1] == 100);
  UnPackClass<Base1, Base2> test_unpack_class(string{ "Zoom" }, 5);
  std::tuple<string, int> result = test_unpack_class.GetValue();
  CHECK(std::get<0>(result) == "Zoom");
  CHECK(std::get<1>(result) == 5);
}

















namespace
{
  void Conclusion() {
    "和编译器抗争到底";
  }
}
