#include "catch.hpp"
#include <variant>

union TestUnion {
  long value_long;
  // std::string value_string;
  int value_int;
};

TEST_CASE("Test Union") {
  TestUnion union_one;
  (void)union_one;
}

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
  void ReleaseImpl(void* buffer, int index, size_t i) {
    if (index == i) {
      T* buffer_typed = reinterpret_cast<T*>(buffer);
      delete buffer_typed;
    }
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

  template<typename T>
  void* CopyConstructImpl(void* source) {
    return new T(*reinterpret_cast<T*>(source));
  }

  template<typename... T, size_t... I>
  void* CopyConstructUnpack(int index, void* source, std::index_sequence<I...>) {
    void* result = nullptr;
    ((I == index ? (result = CopyConstructImpl<std::tuple_element_t<I, std::tuple<T...>>>(source)) : nullptr), ...);
    return result;
  }

  template<typename... T>
  void* CopyConstruct(int index, void* source) {
    void* result = CopyConstructUnpack<T...>(index, source, std::index_sequence_for<T...>{});
    assert(result);
    return result;
  }

  template<typename T>
  int AssignImpl(void* source, void* dest) {
    *reinterpret_cast<T*>(dest) = *reinterpret_cast<T*>(source);
    return 0;
  }

  template<typename... T, size_t... I>
  void AssignUnpack(int index, void* source, void* dest, std::index_sequence<I...>) {
    // Optimization does not apply.
    std::initializer_list<int>{ (I == index ? (AssignImpl<std::tuple_element_t<I, std::tuple<T...>>>(source, dest)) : 0)...};
  }

  template<typename... T>
  void Assign(int index, void* source, void* dest) {
    AssignUnpack<T...>(index, source, dest, std::index_sequence_for<T...>{});
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
      if (another.buffer_) {
        buffer_ = CopyConstruct<T...>(another.type_index_, another.buffer_);
      }
    }

    Variant& operator=(const Variant& another) {
      if (this == &another) {
        return *this;
      }
      if (type_index_ == another.type_index_) {
        if (type_index_ == -1) {
          assert(buffer_ == nullptr);
          return *this;
        }
        Assign<T...>(type_index_, another.buffer_, buffer_);
        return *this;
      }
      if (type_index_ != -1) {
        Release<T...>(buffer_, type_index_);
        buffer_ = nullptr;
      }
      if (another.type_index_ != -1) {
        buffer_ = CopyConstruct<T...>(another.type_index_, another.buffer_);
      }
      type_index_ = another.type_index_;
      return *this;
    }

    ~Variant() {
      if (type_index_ != -1) {
        Release<T...>(buffer_, type_index_);
      }
    }

    template<typename U>
    Variant(const U& value)
      : type_index_(TypeIndex<U, 0, T...>::value)
      , buffer_(nullptr) {
      static_assert(TypeIndex<U, 0, T...>::value != -1);
      buffer_ = new U(value);
    }

    template<typename U>
    Variant& operator=(const U& another) {
      static_assert(TypeIndex<U, 0, T...>::value != -1);
      constexpr int new_index = TypeIndex<U, 0, T...>::value;
      int old_index = type_index_;
      if (old_index == new_index) {
        typedef typename std::tuple_element_t<new_index, std::tuple<TypeHelper<T>...>>::type CurrentType;
        CurrentType* current_buffer = reinterpret_cast<CurrentType*>(buffer_);
        (*current_buffer) = another;
      } else if (old_index != -1) {
        Release<T...>(buffer_, old_index);
        buffer_ = nullptr;
      }
      buffer_ = new U(another);
      type_index_ = new_index;
      return *this;
    }

    int index() const {
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

class ClassOfDestrutor {
public:
  ClassOfDestrutor()
    : value(new int{5}) {

  }

  ~ClassOfDestrutor() {
    delete value;
  }
  int* value;
};

union TestUnion {
  long value_long;
  // std::string value_string;
  int value_int;
};

TEST_CASE("Test Union") {
  TestUnion union_one;
  (void)union_one;
}

TEST_CASE("Test Variant") {
  TestVariant<int, std::string, std::vector<std::string>> test_variant_1(std::string{ "TestTest222" });
  CHECK(test_variant_1.index() == 1);
  CHECK(Get<1>(test_variant_1) == "TestTest222");
  test_variant_1 = std::vector<std::string>{ "1", "T2T2T2", "Three33333Three", "ZZZABCD" };
  CHECK(test_variant_1.index() == 2);
  TestVariant<int, std::string, std::vector<std::string>> test_variant_2 = test_variant_1;
  CHECK(test_variant_2.index() == 2);
  CHECK(Get<2>(test_variant_2)[0] == "1");
  CHECK(Get<2>(test_variant_2)[1] == "T2T2T2");
  CHECK(Get<2>(test_variant_2)[3] == "ZZZABCD");
  {
    TestVariant<int, std::string, std::vector<std::string>> test_variant_3{ std::string{"3Test3"} };
    test_variant_2 = test_variant_3;
  }
  CHECK(test_variant_2.index() == 1);
  CHECK(Get<1>(test_variant_2) == "3Test3");
  // CHECK(Get<test_variant_2.index()>());
  // CHECK(std::get<std::vector<std::string>>(test_variant_2)[2] == "Meeting");
}
