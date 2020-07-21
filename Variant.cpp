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

  template<typename T>
  int ReleaseImpl(void* buffer, int index, size_t i) {
    if (index == i) {
      T* buffer_typed = reinterpret_cast<T*>(buffer);
      delete buffer_typed;
    }
    return 0;
  }

  template<typename... T, size_t... I>
  void ReleaseUnpack(void* buffer, int index, std::index_sequence<I...>) {
    (ReleaseImpl<std::tuple_element_t<I, std::tuple<T...>>>(buffer, index, I), ...);
  }

  template<typename... T>
  void Release(void* buffer, int index) {
    assert(!buffer && index == -1 || buffer && index >= 0 && index <= sizeof...(T));
    ReleaseUnpack<T...>(buffer, index, std::index_sequence_for<T...>{});
  }

  template<typename... T>
  class Variant {
  public:
    Variant()
      : type_index_(-1)
      , buffer_(nullptr) {

    }

    Variant(const Variant& another)
      : type_index_(another.type_index_)
      , buffer_(nullptr) {

    }

    ~Variant() {
      if (type_index_ != -1) {
        Release<T...>(buffer_, type_index_);
      }
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
      constexpr int new_index = TypeIndex<U, 0, T...>::value;
      int old_index = type_index_;
      static_assert(new_index != -1);
      if (old_index == new_index) {
        typedef typename std::tuple_element_t<new_index, std::tuple<TypeHelper<T>...>>::type CurrentType;
        CurrentType* current_buffer = reinterpret_cast<CurrentType*>(buffer_);
        (*current_buffer) = another;
        return *this;
      }
      if (old_index != -1) {
        Release<T...>(buffer_, old_index);
        buffer_ = nullptr;
      }
      buffer_ = new U(another);
      type_index_ = new_index;
      return *this;
    }

    constexpr int index() const {
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

  private:
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

namespace
{
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
}

TEST_CASE("Test Variant") {
  TestVariant<int, std::string, std::vector<std::string>> test_variant_1(std::string{ "TestTest222" });
  CHECK(test_variant_1.index() == 1);
  CHECK(Get<1>(test_variant_1) == "TestTest222");
  test_variant_1 = std::vector<std::string>{ "1", "Meeting", "Chat", "Phone" };
  CHECK(test_variant_1.index() == 2);
  TestVariant<int, std::string, std::vector<std::string>> test_variant_2 = test_variant_1;
  CHECK(test_variant_2.index() == 2);
  CHECK(Get<2>(test_variant_2)[0] == "1");
  CHECK(Get<2>(test_variant_2)[1] == "Meeting");
  CHECK(Get<2>(test_variant_2)[3] == "Phone");
}
