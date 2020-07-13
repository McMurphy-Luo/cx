#include "catch.hpp"

int Noop(int argument) {
  return argument;
}

class TestClass {
public:
  static bool constructor_is_called;
  static bool copy_constructor_is_called;
  static bool copy_assignment_is_called;
  static bool move_constructor_is_called;
  static bool move_assignment_is_called;

  static bool CheckConstruction(
    bool constructor,
    bool copy_constructor, bool copy_assignment,
    bool move_constructor, bool move_assignment) {
    return
      constructor == constructor_is_called
      &&
      copy_constructor == copy_constructor_is_called
      &&
      copy_assignment == copy_assignment_is_called
      &&
      move_constructor == move_constructor_is_called
      &&
      move_assignment == move_assignment_is_called;
  }

  static void ResetConstructionFlags() {
    constructor_is_called = false;
    copy_constructor_is_called = false;
    copy_assignment_is_called = false;
    move_constructor_is_called = false;
    move_assignment_is_called = false;
  }

public:
  TestClass(int int_member)
    : int_member_(int_member) {
    constructor_is_called = true;
  }

  TestClass(const TestClass& another)
    : int_member_(another.int_member_){
    copy_constructor_is_called = true;
  }

  TestClass& operator=(const TestClass& another) {
    if (this == &another) {
      return *this;
    }
    int_member_ = another.int_member_;
    copy_assignment_is_called = true;
    return *this;
  }

  TestClass(TestClass&& rhs) noexcept
    : int_member_(rhs.int_member_)  {
    rhs.int_member_ = -1;
    move_constructor_is_called = true;
  }

  TestClass& operator=(TestClass&& rhs) {
    if (this == &rhs) {
      return *this;
    }
    int_member_ = rhs.int_member_;
    rhs.int_member_ = -1;
    move_assignment_is_called = true;
    return *this;
  }

public:
  int int_member_;
};

bool TestClass::constructor_is_called = false;
bool TestClass::copy_constructor_is_called = false;
bool TestClass::copy_assignment_is_called = false;
bool TestClass::move_constructor_is_called = false;
bool TestClass::move_assignment_is_called = false;

namespace
{
  bool function_takes_lvalue_reference_int_is_called = false;
  bool function_takes_rvalue_reference_int_is_called = false;
}

void FunctionTakesReferenceOverloadedInt(const int& argument, int value) {
  CHECK(argument == value);
  function_takes_lvalue_reference_int_is_called = true;
}

void FunctionTakesReferenceOverloadedInt(int&& argument, int value) {
  CHECK(argument == value);
  function_takes_rvalue_reference_int_is_called = true;
}

void CheckFunctionCheckReferencesIntCalledFlags(bool l_value_reference, bool r_value_reference) {
  CHECK(function_takes_lvalue_reference_int_is_called == l_value_reference);
  CHECK(function_takes_rvalue_reference_int_is_called == r_value_reference);
}

void ResetFunctionTakesReferencesIntCalledFlags() {
  function_takes_lvalue_reference_int_is_called = false;
  function_takes_rvalue_reference_int_is_called = false;
}

void FunctionOnlyTakesLValueReference(int& argument, int value) {
  CHECK(argument == value);
}

void FunctionOnlyTakesRValueReference(int&& argument, int value) {
  CHECK(argument == value);
}

void FunctionOnlyTakesConstLValueReference(const int& argument, int value) {
  CHECK(argument == value);
}

namespace
{
  void Summary() {
    "Summary";
    "1: lvalue 和 rvalue 不存在交集"; // https://docs.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=vs-2019#:~:text=%20The%20C%2B%2B17%20standard%20defines%20expression%20value%20categories,object%20or%20bit-field%20whose%20resources%20can...%20More%20
    "2: 右值引用和const左值引用可以延长临时对象的生命周期" // https://en.cppreference.com/w/cpp/language/lifetime
      "{左值, 右值} * {左值引用, 右值引用, const 左值引用}";
    "3: 左值引用-> 左值";
    "4: 右值引用-> 右值";
    "5: const左值引用 -> 右值, 左值";
  }
}

TEST_CASE("Test LValue Reference") {
  int value = 9;
  int& lvalue_reference = value;
  CHECK(lvalue_reference == 9);
  int&& rvalue_reference = 2 + 3;
  CHECK(rvalue_reference == 5);
  const int& const_lvalue_reference = 5;

  int& test_lvalue_reference_1 = value; // 左值引用可以绑定左值
  value = 99;
  CHECK(test_lvalue_reference_1 == 99);
  // int& test_lvalue_reference_2 = 5; // 5 literal 不是左值

  // int& test_lvalue_reference_3 = 2 + 3; // 左值引用不能绑定到右值
  // FunctionOnlyTakesLValueReference(5 + 6, 11); // 左值引用不能绑定到右值

  int& test_lvalue_reference_4 = lvalue_reference; // 左值引用可以绑定左值引用
  lvalue_reference = 2;
  CHECK(test_lvalue_reference_4 == 2);

  int& test_lvalue_reference_5 = rvalue_reference; // 左值引用可以绑定到具名的右值引用，因为具名右值引用是左值
  rvalue_reference = 10;
  CHECK(test_lvalue_reference_5 == 10);

  // int& test_lvalue_reference_6 = const_lvalue_reference;
}

TEST_CASE("Test Const LValue References") {
  int value = 9;
  int& lvalue_reference = value;
  CHECK(lvalue_reference == 9);
  int&& rvalue_reference = 2 + 3;
  CHECK(rvalue_reference == 5);
  const int& const_lvalue_reference = 15;

  const int& test_const_lvalue_reference_1 = value; // const 左值引用绑定左值
  value = 12;
  CHECK(test_const_lvalue_reference_1 == 12);

  const int& test_const_lvalue_reference_2 = 2 + 3; // const 左值引用也能延长临时变量的生命周期，const左值引用绑定到右值
  CHECK(test_const_lvalue_reference_2 == 5);

  const int& test_const_lvalue_reference_3 = lvalue_reference;
  lvalue_reference = 128;
  CHECK(test_const_lvalue_reference_3 == 128);

  const int& test_const_lvalue_reference_4 = rvalue_reference;
  rvalue_reference = 66;
  CHECK(test_const_lvalue_reference_4 == 66);

  const int& test_const_lvalue_reference_5 = const_lvalue_reference;
  CHECK(test_const_lvalue_reference_5 == 15);
}

TEST_CASE("Test RValue Reference") {
  int value = 9;
  int& lvalue_reference = value;
  CHECK(lvalue_reference == 9);
  int&& rvalue_reference = 3 + 18;
  CHECK(rvalue_reference == 21);
  const int& const_lvalue_reference = 65;

  // int&& rvalue_reference_1 = value; 右值引用不能绑定到左值
  int&& rvalue_reference_2 = 99 + 2; // 右值引用可以绑定到右值，能延长临时对象的生命周期
  CHECK(rvalue_reference_2 == 101);
  
  // int&& rvalue_reference_3 = lvalue_reference; // 右值引用不能绑定左值引用，因为左值引用是左值
  // int&& rvalue_reference_4 = rvalue_reference; // 右值引用不能绑定右值引用，因为具名右值引用是左值
  // int&& rvalue_reference_5 = const_lvalue_reference; // 右值引用不能绑定const 左值引用
}

TEST_CASE("Test Reference Functions") {
  int value = -5;
  int& lvalue_reference = value;
  int&& rvalue_reference = 5 + 6; // 右值引用不能绑定到左值
  const int& const_lvalue_reference = 19;

  FunctionOnlyTakesLValueReference(value, -5);
  // FunctionOnlyTakesLValueReference(3 + 9, 12);
  FunctionOnlyTakesLValueReference(lvalue_reference, -5);
  FunctionOnlyTakesLValueReference(rvalue_reference, 11);
  // FunctionOnlyTakesLValueReference(const_lvalue_reference, 11);

  // FunctionOnlyTakesRValueReference(value, -5); // 右值引用不能绑定到左值
  FunctionOnlyTakesRValueReference(5 + 6, 11);
  // FunctionOnlyTakesRValueReference(lvalue_reference, -5); // 右值引用不能绑定左值引用
  // FunctionOnlyTakesRValueReference(const_lvalue_reference, 11); // 右值引用不能绑定const左值引用
  // FunctionOnlyTakesRValueReference(rvalue_reference, 15); // 右值引用不能绑定左值

  FunctionOnlyTakesConstLValueReference(value, -5);
  FunctionOnlyTakesConstLValueReference(3 + 19, 22);
  FunctionOnlyTakesConstLValueReference(lvalue_reference, -5);
  FunctionOnlyTakesConstLValueReference(rvalue_reference, 11);
  FunctionOnlyTakesConstLValueReference(const_lvalue_reference, 19);

  /*===================================================================================================*/

  ResetFunctionTakesReferencesIntCalledFlags();
  FunctionTakesReferenceOverloadedInt(3 + 3, 6);
  CheckFunctionCheckReferencesIntCalledFlags(false, true); // 调用右值引用版本

  ResetFunctionTakesReferencesIntCalledFlags();
  FunctionTakesReferenceOverloadedInt(Noop(99), 99);
  CheckFunctionCheckReferencesIntCalledFlags(false, true); // 调用右值引用版本

  int&& test_rvalue_reference = 9 + 12;
  ResetFunctionTakesReferencesIntCalledFlags();
  FunctionTakesReferenceOverloadedInt(test_rvalue_reference, 21);
  CheckFunctionCheckReferencesIntCalledFlags(true, false); // 调用左值引用版本

  ResetFunctionTakesReferencesIntCalledFlags();
  FunctionTakesReferenceOverloadedInt(test_rvalue_reference = 9, 9);
  CheckFunctionCheckReferencesIntCalledFlags(true, false); // 调用左值引用版本

  // test_rvalue_reference为左值
  // 借助编译原理的思路+标准的定义才能区分左值和右值

  test_rvalue_reference = 10;
  ResetFunctionTakesReferencesIntCalledFlags();
  FunctionTakesReferenceOverloadedInt(std::move(test_rvalue_reference), 10); // std::move 无条件转换为右值
  CheckFunctionCheckReferencesIntCalledFlags(false, true); // 调用右值引用版本

  /**
   * template<typename... T>
   * void TestImpl(const T&) {
   *
   * }
   *
   * template<typename... T>
   * void TestImpl(T&&...) {
   *
   * }
   *
   * 如果没有std::foward
   * template<typename... T>
   * void Test(T&&... args) {
   *   TestImpl(args...); // 永远调用 void TestImpl(const T&);
   * }
   */
}

TEST_CASE("Test Miscellanea") {
  int&& rvalue_reference = 5 + 6;
  CHECK(rvalue_reference == 11);
  int* p_rvalue_reference = &rvalue_reference;
  CHECK(p_rvalue_reference != nullptr);

  TestClass::ResetConstructionFlags();
  TestClass&& rvalue_reference_2 = TestClass{ 5 };
  CHECK(TestClass::CheckConstruction(true, false, false, false, false));
  CHECK(rvalue_reference_2.int_member_ == 5);
  TestClass* p_object = &rvalue_reference_2;
  CHECK(p_object != nullptr);

  TestClass::ResetConstructionFlags();
  const TestClass& const_lvalue_reference = TestClass(3);
  CHECK(TestClass::CheckConstruction(true, false, false, false, false));
  CHECK(const_lvalue_reference.int_member_ == 3);
  const TestClass* p_const_object = &const_lvalue_reference;
  CHECK(p_const_object->int_member_ == 3);

  TestClass::ResetConstructionFlags();
  TestClass value = rvalue_reference_2;
  CHECK(TestClass::CheckConstruction(false, true, false, false, false));

  TestClass::ResetConstructionFlags();
  value.int_member_ = 99;
  TestClass value_2 = std::move(value);
  CHECK(TestClass::CheckConstruction(false, false, false, true, false));
  CHECK(value_2.int_member_ == 99);
  CHECK(value.int_member_ == -1); // 移动构造函数不影响对象本身
  p_object = &value;
  CHECK(p_object->int_member_ == -1);

  TestClass::ResetConstructionFlags();
  TestClass value_3 = std::move(const_lvalue_reference);
  CHECK(TestClass::CheckConstruction(false, true, false, false, false));
  CHECK(const_lvalue_reference.int_member_ == 3);
  CHECK(value_3.int_member_ == 3);
}

template<typename T>
void ForwardReferenceFunction(T&& item) {
  using ItemType = std::remove_reference_t<T>;
  std::vector<ItemType> vector;
  vector.push_back(std::forward<T>(item));
}

TEST_CASE("Test Forward Reference") {
  TestClass object(9);
  TestClass::ResetConstructionFlags();
  ForwardReferenceFunction(object);
  CHECK(TestClass::CheckConstruction(false, true, false, false, false));
  CHECK(object.int_member_ == 9);

  TestClass object_2(9);
  TestClass::ResetConstructionFlags();
  ForwardReferenceFunction(std::move(object_2));
  CHECK(TestClass::CheckConstruction(false, true, false, false, false));
  // CHECK(TestClass::CheckConstruction(false, true, false, false, false)); // Todo: change move constructor to noexcept

  TestClass object_3(10);
  TestClass::ResetConstructionFlags();
  ForwardReferenceFunction(std::forward<TestClass>(object_2));
  CHECK(TestClass::CheckConstruction(false, true, false, false, false));
}

void Conclusion() {

}
