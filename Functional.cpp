#include "catch.hpp"
#include <functional>

namespace
{
  class TestClass {
  public:
    TestClass(int value)
      : value_(value) {

    }

    int Calculate(int argument) {
      return value_ + argument;
    }

  private:
    int value_;
  };
}

TEST_CASE("Test Bind and PlaceHolders") {
  TestClass test_object_1(5);
  std::function<int(int)> calculator = std::bind(&TestClass::Calculate, &test_object_1, std::placeholders::_1);
  CHECK(calculator(1) == 6);
  CHECK(calculator(13) == 18);
}

TEST_CASE("Test Lambda Expression") {
  TestClass test_object_1(5);


}
