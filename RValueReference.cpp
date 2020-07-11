#include "catch.hpp"

class TestRValueReference {
public:
  static bool constructor_is_called;
  static bool copy_constructor_is_called;
  static bool copy_assignment_is_called;
  static bool move_constructor_is_called;
  static bool move_assignment_is_called;

  static void CheckConstruction(
    bool constructor,
    bool copy_constructor, bool copy_assignment,
    bool move_constructor, bool move_assignment) {
    CHECK(constructor == constructor_is_called);
    CHECK(copy_constructor == copy_constructor_is_called);
    CHECK(copy_assignment == copy_assignment_is_called);
    CHECK(move_constructor == move_constructor_is_called);
    CHECK(move_assignment == move_assignment_is_called);
  }

  static void ResetConstructionFlags() {
    constructor_is_called = false;
    copy_constructor_is_called = false;
    copy_assignment_is_called = false;
    move_constructor_is_called = false;
    move_assignment_is_called = false;
  }

public:
  TestRValueReference(int int_member)
    : int_member_(int_member) {
    constructor_is_called = true;
  }

  TestRValueReference(TestRValueReference&& rhs)
    : int_member_(rhs.int_member_) {
    rhs.int_member_ = -1;
    move_constructor_is_called = true;
  }

  TestRValueReference& operator=(TestRValueReference&& rhs) {
    if (this == &rhs) {
      return *this;
    }
    int_member_ = rhs.int_member_;
    rhs.int_member_ = -1;
    move_assignment_is_called = true;
  }

public:
  int int_member_;
};

bool TestRValueReference::constructor_is_called = false;
bool TestRValueReference::copy_constructor_is_called = false;
bool TestRValueReference::copy_assignment_is_called = false;
bool TestRValueReference::move_constructor_is_called = false;
bool TestRValueReference::move_assignment_is_called = false;

namespace
{
  bool function_takes_lvalue_reference_is_called = false;
  bool function_takes_rvalue_reference_is_called = false;
}

void FunctionTakesReferenceOverloaded(const TestRValueReference& argument, int value) {
  CHECK(argument.int_member_ == value);
  function_takes_lvalue_reference_is_called = true;
}

void FunctionTakesReferenceOverloaded(TestRValueReference&& argument, int value) {
  CHECK(argument.int_member_ == value);
  function_takes_rvalue_reference_is_called = true;
}

void ResetFunctionTakesReferencesCalledFlags() {
  function_takes_lvalue_reference_is_called = false;
  function_takes_rvalue_reference_is_called = false;
}

void CheckFunctionCheckReferencesCalledFlags(bool l_value_reference, bool r_value_reference) {
  CHECK(function_takes_lvalue_reference_is_called == l_value_reference);
  CHECK(function_takes_rvalue_reference_is_called == r_value_reference);
}

TEST_CASE("Test Pointer to RValue Reference") {
  int&& test_rvalue_reference = 5 + 6;
  CHECK(test_rvalue_reference == 11);
  int* p_rvalue_reference = &test_rvalue_reference;
  CHECK(p_rvalue_reference != nullptr);
}

TEST_CASE("Test RValue Reference") {
  TestRValueReference::ResetConstructionFlags();
  ResetFunctionTakesReferencesCalledFlags();

  FunctionTakesReferenceOverloaded(TestRValueReference{ 6 }, 6);
  TestRValueReference::CheckConstruction(true, false, false, false, false);
  TestRValueReference::ResetConstructionFlags();
  CheckFunctionCheckReferencesCalledFlags(false, true);
  ResetFunctionTakesReferencesCalledFlags();

  TestRValueReference&& object_rvalue_reference = TestRValueReference{5};
  TestRValueReference::CheckConstruction(true, false, false, false, false);
  TestRValueReference::ResetConstructionFlags();

  FunctionTakesReferenceOverloaded(object_rvalue_reference, 5);
  CheckFunctionCheckReferencesCalledFlags(true, false);
  ResetFunctionTakesReferencesCalledFlags();

  object_rvalue_reference.int_member_ = 10;
  FunctionTakesReferenceOverloaded(std::move(object_rvalue_reference), 10);
  CheckFunctionCheckReferencesCalledFlags(false, true);
  ResetFunctionTakesReferencesCalledFlags();

  object_rvalue_reference.int_member_ = 12;
  FunctionTakesReferenceOverloaded(std::forward<TestRValueReference>(object_rvalue_reference), 12);
  CheckFunctionCheckReferencesCalledFlags(false, true);
  ResetFunctionTakesReferencesCalledFlags();
}
