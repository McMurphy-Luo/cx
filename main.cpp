#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include <iostream>

namespace
{
  // reference https://en.cppreference.com/w/cpp/compiler_support/11
  void cpp11() {
    ("C99 preprocessor");
    ("static_assert");
    ("Right angle brackets");
    ("Extended friend declarations");
    ("long long");
    ("Compiler support for type traits");
    ("auto");
    ("Delegating constructors");
    ("extern template");
    ("constexpr");
    ("Template aliases");
    ("char16_tand char32_t");
    ("alignas");
    ("alignof");
    ("Defaultedand deleted functions");
    ("Strongly - typed enum");
    ("Atomic operations");
    ("nullptr");
    ("Explicit conversion operators");
    ("ref - qualifiers");
    ("Unicode string literals");
    ("Raw string literals");
    ("Inline namespaces");
    ("Inheriting constructors");
    ("Trailing function return types");
    ("Unrestricted unions");
    ("Variadic templates");
    ("Expression SFINAE");
    ("Local and unnamed types as template parameters");
    ("Thread - local storage");
    ("Dynamic initialization and destruction with concurrency(magic statics)");
    ("Garbage Collection and Reachability - Based Leak Detection");
    ("Initializer lists");
    ("Non - static data member initializers");
    ("Forward enum declarations");
    ("User - defined literals");
    ("Rvalue references");
    ("Lambda expressions");
    ("Range - for loop");
    ("noexcept");
    ("Defaulted move special member functions");
    ("override and final");
    ("decltype");
  }

  // reference https://en.cppreference.com/w/cpp/17
  void cpp17() {
    ("New auto rules for direct-list-initialization");
    ("static_assert with no message");
    ("typename in a template template parameter");
    ("Removing trigraphs");
    ("Nested namespace definition");
    ("Attributes for namespaces and enumerators");
    ("u8 character literals");
    ("Allow constant evaluation for all non-type template arguments");
    ("Fold Expressions");
    ("Unary fold expressions and empty parameter packs");
    ("Remove Deprecated Use of the register Keyword");
    ("Remove Deprecated operator++(bool)");
    ("Make exception specifications part of the type system");
    ("Aggregate initialization of classes with base classes");
    ("__has_include in preprocessor conditionals");
    ("DR: New specification for inheriting constructors (DR1941 et al)");
    ("Lambda capture of *this");
    ("Direct-list-initialization of enumerations");
    ("constexpr lambda expressions");
    ("Differing begin and end types in range-based for");
    ("[[fallthrough]] attribute");
    ("[[nodiscard]] attribute");
    ("[[maybe_unused]] attribute");
    ("Hexadecimal floating-point literals");
    ("Using attribute namespaces without repetition");
    ("Dynamic memory allocation for over-aligned data");
    ("Class template argument deduction");
    ("Non-type template parameters with auto type");
    ("Guaranteed copy elision");
    ("Replacement of class objects containing reference members");
    ("Stricter expression evaluation order");
    ("Structured Bindings");
    ("Ignore unknown attributes");
    ("constexpr if statements");
    ("init-statements for if and switch");
    ("Inline variables");
    ("Removing dynamic exception specifications");
    ("Pack expansions in using-declarations");
    ("DR: Matching of template template-arguments excludes compatible templates");
  }

  void Summary() {
    ("知识的搬运工");

    ("rvalue references");
    ("variadic templates / tuple / apply");

    ("<functional>");
    ("<regex>");
    ("<optional>");
    ("<chrono>");
    ("<string_view>");
    ("<any>");
    ("<filesystem>");
    ("<variant>");

    ("力有不逮，深感惭愧：");
    ("constexpr, auto");
    ("thread, memory model and atomic");
    ("attributes");
    ("others");
  }
}

int main(int argc, char* argv[]) {
  printf("Running main() from %s, C++ standard is %d\n", __FILE__, _MSVC_LANG);
  int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  flag |= _CRTDBG_LEAK_CHECK_DF;
  flag |= _CRTDBG_ALLOC_MEM_DF;
  _CrtSetDbgFlag(flag);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetBreakAlloc(-1);
  return Catch::Session().run(argc, argv);
}
