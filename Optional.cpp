#include "catch.hpp"
#include <optional>
#include <string>

using std::string;

template<typename T>
class Optional {
public:
  Optional()
    : has_(false)
    , buffer_() {

  }

  ~Optional() {
    if (has_) {
      reinterpret_cast<T*>(buffer_)->~T();
    }
  }

  Optional(const T& value)
    : has_(true)
    , buffer_() {
    new(buffer_) T(value);
  }

  Optional(T&& value)
    : has_(true)
    , buffer_() {
    new(buffer_) T(std::move(value));
  }

  Optional(const Optional& another)
    : has_(another.has_)
    , buffer_() {
    if (another.has_) {
      new(buffer_) T(another.value());
    }
  }

  Optional& operator=(const T& value) {
    if (!has_) {
      new(buffer_) T(value);
    } else {
      *reinterpret_cast<T*>(buffer_) = value;
    }
    has_ = true;
    return *this;
  }

  bool has() { return has_; }

  T value() const { return *(T*)(buffer_); }

private:
  bool has_;
  unsigned char buffer_[sizeof(T)];
};

TEST_CASE("Test Optional") {
  Optional<int> int_optional;
  CHECK(int_optional.has() == false);
  int_optional = 99;
  CHECK(int_optional.value() == 99);
  Optional<string> string_optional_1;
  CHECK(string_optional_1.has() == false);
  string_optional_1 = "ZoomTest";
  CHECK(string_optional_1.value() == "ZoomTest");
  Optional<string> string_optional_2 = string_optional_1;
  CHECK(string_optional_2.value() == "ZoomTest");
  string_optional_1 = "TestTestTest";
  CHECK(string_optional_1.value() == "TestTestTest");
  Optional<std::vector<string>> vector_optional = std::vector<string>{ "Test_1", "ABCD" };
  CHECK(vector_optional.value().size() == 2);
  CHECK(vector_optional.value().at(1) == "ABCD");
}
