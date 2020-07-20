#include "catch.hpp"
#include <string_view>

TEST_CASE("Test String View") {
  std::string test_string_1{ "TestZoom" };
  std::string_view test_string_view(test_string_1);
  CHECK(test_string_view == "TestZoom");
  test_string_1 = "TestTestTest";
  CHECK(test_string_view == "TestTest");
  test_string_view = "Zoom"; // Copy constructor
  CHECK(test_string_view == "Zoom");
  CHECK(test_string_1 == "TestTestTest");
  std::string test_string_2 { test_string_view };
  CHECK(test_string_2 == "Zoom");
}

TEST_CASE("Test String View Issue") {
  std::string test_string_1{ "TestTestTest" };
  std::string_view view_1{ test_string_1 };
  test_string_1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  CHECK(view_1 != "ABCDEFGHIJKLMNOP");

  std::string_view view_2;
  size_t size = 0;
  {
    std::string test_string_2{ "qwesdfdwerdsfsdfsdfawtecvzxv" };
    size = test_string_2.size();
    view_2 = test_string_2;
  }
  CHECK(view_2.size() == size);
  CHECK(view_2 != "qwesdfdwerdsfsdfsdfawtecvzxv");
}
