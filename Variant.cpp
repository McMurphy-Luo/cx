#include "catch.hpp"
#include <variant>

namespace
{
  template<typename T>
  struct TypeHelper {
    using type = T;
    int id;
  };

  template<typename T, int index, typename... U>
  struct TypeIndex;

  template<typename T, int index, typename U>
  struct TypeIndex<T, index, U>
    : std::integral_constant<
        int,
        std::is_same_v<T, U> ? index : -1
      > {};

  template<typename T, int index, typename U, typename... V>
  struct TypeIndex<T, index, U, V...> 
    : std::integral_constant<
        int,
        std::is_same_v<T, U> ? index : + TypeIndex<T, index + 1, V...>::value
      > {};

  template<typename... T>
  class Variant {
  public:
    Variant()
      : type_supported_()
      , type_index_(-1)
      , buffer_(nullptr) {

    }

    ~Variant() {

    }

    template<typename U>
    Variant(const U& value)
      : buffer_(nullptr) {
      int constexpr type_index = TypeIndex<U, 0, T...>::value;
      static_assert(type_index != -1);
      typedef typename std::tuple_element_t<type_index, std::tuple<TypeHelper<T>...>>::type CurrentType;
      buffer_ = new CurrentType(value);
      type_index_ = type_index;
    }

    template<typename U>
    Variant(U&& value)
      : buffer_(nullptr) {
      int constexpr type_index = TypeIndex<U, 0, T...>::value;
      static_assert(type_index != -1);
      typedef typename std::tuple_element_t<type_index, std::tuple<TypeHelper<T>...>>::type CurrentType;
      buffer_ = new CurrentType(value);
      type_index_ = type_index;
    }

    template<typename U>
    Variant& operator=(const U& another) {
      constexpr int index = TypeIndex<U, 0, T...>::value;
      int old_index = type_index_;
      static_assert(index != -1);
      if (old_index == index) {
        typedef typename std::tuple_element_t<index, std::tuple<TypeHelper<T>...>>::type CurrentType;
        CurrentType* current_buffer = reinterpret_cast<CurrentType*>(buffer_);
        (*current_buffer) = another;
        return *this;
      }
      if (old_index != -1) {
        typedef typename std::tuple_element_t<index, std::tuple<TypeHelper<T>...>>::type CurrentType;
        CurrentType* current_buffer = reinterpret_cast<CurrentType*>(buffer_);
        delete current_buffer;
      }
      buffer_ = new U(another);
      type_index_ = index;
      return *this;
    }

    int index() {
      return type_index_;
    }

    template<std::size_t index>
    typename std::tuple_element_t<
      index,
      std::tuple<TypeHelper<T>...>
    >::type value() const {
      typedef typename std::tuple_element_t<index, std::tuple<TypeHelper<T>...>>::type ResultType;
      return *reinterpret_cast<ResultType*>(buffer_);
    }

    std::tuple<TypeHelper<T>...> type_supported_;
    int type_index_;
    void* buffer_;
  };
}

TEST_CASE("Test Type Index") {
  CHECK(TypeIndex<int, 0, std::string, int>::value == 1);
  CHECK(TypeIndex<int, 0, int, int>::value == 0);
  CHECK(TypeIndex<int, 0, std::string, std::vector<int>, int, std::string, uintptr_t>::value == 2);
  CHECK(TypeIndex<uintptr_t, 0, uintptr_t, std::vector<int>, int, std::string, uintptr_t>::value == 0);
  CHECK(TypeIndex<std::pair<bool, int>, 0, uintptr_t, std::vector<int>, int, std::string, uintptr_t>::value == -1);
}

template<size_t index, typename... T>
auto Get(const Variant<T...>& argument) {
  return argument.value<index>();
}

template<size_t index, typename... T>
auto Get(const std::variant<T...>& argument) {
  return std::get<index>(argument);
}

template<typename... T>
using TestVariant = Variant<T...>;

TEST_CASE("Test Variant") {
  TestVariant<int, std::string, std::vector<std::string>> test_variant(std::string{ "TestTest222" });
  CHECK(test_variant.index() == 1);
  CHECK(Get<1>(test_variant) == "TestTest222");
  test_variant = std::vector<std::string>{ "1", "Meeting", "Chat", "Phone" };
}
