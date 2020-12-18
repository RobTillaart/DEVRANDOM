//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2020-12-18
// PURPOSE: unit tests for the DEVRANDOM library
//          https://github.com/RobTillaart/DEVRANDOM
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// https://github.com/Arduino-CI/arduino_ci/blob/master/cpp/unittest/Assertion.h#L33-L42
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)
// assertNotNull(actual)

#include <ArduinoUnitTests.h>

#define assertEqualFloat(arg1, arg2, arg3)  assertOp("assertEqualFloat", "expected", fabs(arg1 - arg2), compareLessOrEqual, "<=", "actual", arg3)
#define assertEqualINF(arg)  assertOp("assertEqualINF", "expected", INFINITY, compareEqual, "==", "actual", arg)
#define assertEqualNAN(arg)  assertOp("assertEqualNAN", "expected", true, compareEqual, "==", "actual", isnan(arg))


#include "Arduino.h"
#include "DEVRANDOM.h"



unittest_setup()
{
}

unittest_teardown()
{
}

unittest(test_constructor)
{
  DEVRANDOM dr;
  
  assertEqual(DEVRANDOM_MODE_SW, dr.getMode());
  
  dr.useHW(4);
  assertEqual(DEVRANDOM_MODE_HW, dr.getMode());
  
  dr.useAR(0);
  assertEqual(DEVRANDOM_MODE_AR, dr.getMode());
  
  dr.useSW();
  assertEqual(DEVRANDOM_MODE_SW, dr.getMode());
}


unittest(test_generator_mode)
{
  DEVRANDOM dr;

  assertEqual(1, dr.available());

  fprintf(stderr, "peak == read test");
  for (int i = 0; i < 10; i++)
  {
    int n = dr.peek();
    assertEqual(n, dr.read());
  }
  
  dr.useHW();
  assertEqual(DEVRANDOM_MODE_HW, dr.getMode());
  
  dr.useAR();
  assertEqual(DEVRANDOM_MODE_AR, dr.getMode());
  
  dr.useSW();
  assertEqual(DEVRANDOM_MODE_SW, dr.getMode());
}

unittest_main()

// --------
