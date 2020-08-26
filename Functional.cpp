#include "catch.hpp"
#include <functional>

namespace
{
  class TestClass {
  public:
    TestClass(int value)
      : value_(std::make_unique<int>(value)) {

    }

    TestClass(const TestClass& another)
      : value_(std::make_unique<int>(*another.value_)) {

    }

    TestClass& operator=(const TestClass& another) {
      if (this == &another) {
        return *this;
      }
      value_ = std::make_unique<int>(*another.value_);
      return *this;
    }

    TestClass(TestClass&& another) = default;

    TestClass& operator=(TestClass&& another) = default;

    int Calculate(int argument) const {
      return *value_ + argument;
    }

    bool IsValid() const {
      return !!value_;
    }

  private:
    std::unique_ptr<int> value_;
  };
}

TEST_CASE("Test Bind and PlaceHolders") {
  TestClass test_object_1(5);
  std::function<int(int)> calculator = std::bind(&TestClass::Calculate, &test_object_1, std::placeholders::_1);
  CHECK(calculator(1) == 6);
  CHECK(calculator(13) == 18);
  calculator = std::bind(std::mem_fn(&TestClass::Calculate), &test_object_1, std::placeholders::_1);
  CHECK(calculator(99) == 104);
}

// Prefer lambda to std::bind whenever
// Effective Modern C++ Rule 34
TEST_CASE("Test Lambda Expression") {
  TestClass test_object_1(5);
  std::function<int(int)> calculator = [&factor = test_object_1](int argument) -> int {
    return factor.Calculate(argument);
  };
  test_object_1 = TestClass{ 9 };
  CHECK(calculator(12) == 21);
  CHECK(calculator(80) == 89);
  calculator = [factor = test_object_1](int argument) -> int {
    return factor.Calculate(argument);
  };
  test_object_1 = 30;
  CHECK(calculator(9) == 18);
  CHECK(calculator(11) == 20);
  // Use init capture to move object into closures
  // Effective Modern C++ rule 32
  calculator = [factor = std::move(test_object_1)](int argument) -> int {
    return factor.Calculate(argument);
  };
  CHECK(!test_object_1.IsValid());
}

class TestCaptureThis {
public:
  TestCaptureThis(int value)
    : value_(value) {

  }

  TestCaptureThis(const TestCaptureThis&) = delete;
  TestCaptureThis& operator=(const TestCaptureThis&) = delete;

  ~TestCaptureThis() {
    value_ = -1;
  }
  /*
  std::function<int()> Capture() const {
    return [this]() -> int {
      return value_;
    };
  }
  
  std::function<int()> Capture() const {
    return [this]() -> int {
      return this->value_;
    };
  }

  std::function<int()> Capture(int value_) const {
    return [this]() -> int {
      return value_;
    };
  }
  */
  // Prefer 
  std::function<int()> Capture() const {
    return [t = this]() -> int {
      return t->value_;
    };
  }
  
  int value_;
};

TEST_CASE("Test Capture This") {
  TestCaptureThis test_object_1(11);
  std::function<int()> lambda = test_object_1.Capture();
  CHECK(lambda() == 11);
  {
    TestCaptureThis test_object_2(5);
    lambda = test_object_2.Capture();
  }
  CHECK(lambda() == -1);
}
