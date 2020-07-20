#include "catch.hpp"
#include <regex>

TEST_CASE("Test Regular Expression") {
  std::regex email_validation("\\b[\\w\\.-]+@[\\w\\.-]+\\.\\w{2,4}\\b");
  CHECK(!std::regex_match("lsdjfldf", email_validation));
  CHECK(std::regex_match("Test@mail.com", email_validation));
  CHECK(!std::regex_match("123oijlkjjzoixiujoi23@lskldjfl@sdljfls.com", email_validation));
  CHECK(!std::regex_match("1@mail", email_validation));

  std::regex number_finder{ "\\d+" };
  std::string source_string{ "Zoom2020Test07c++" };
  std::sregex_token_iterator match_iterator = std::sregex_token_iterator(source_string.begin(), source_string.end(), number_finder, 0);
  std::sregex_token_iterator end;
  std::vector<std::string> matches;
  std::copy(match_iterator, end, std::back_inserter(matches));
  CHECK(matches.size() == 2);
  CHECK(matches.at(0) == "2020");
  CHECK(matches.at(1) == "07");
}
