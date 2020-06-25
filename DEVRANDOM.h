#pragma once
//
//    FILE: DEVRANDOM.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: Arduino library for a /dev/random stream - usefull for testing
//     URL: https://github.com/RobTillaart/DEVRANDOM
//
// HISTORY:
// 0.1.0    2020-06-23 initial version

#include "Arduino.h"

class DEVRANDOM : public Stream
{
public:
  DEVRANDOM()
  {
    _next = random(256);
    _seed = 0;
    _mode = 0;
    _pin = 0;
  };

  int available() { return 1; };
  
  int peek()      { return _next; };
  int read()      
  {
    uint8_t x = _next;
    _next = _rnd();
    return x;
  };

  size_t write(const uint8_t data)
  {
    _seed = (_seed << 8) + data;
    randomSeed(_seed);
    return 1;
  };

  void useAR(uint8_t pin) { _mode = 2; _pin = pin; };
  void useHW(uint8_t pin) { _mode = 1; _pin = pin; pinMode(_pin, INPUT); };
  void useSW()            { _mode = 0; };

private:
  uint8_t  _next;
  uint32_t _seed;
  uint8_t  _mode;
  uint8_t  _pin;
  
  int _rnd()
  {
    if (_mode == 0 ) return random(256);
    if (_mode == 1 ) return _hardware();
    if (_mode == 2 ) return _analog();
    return 0;
  }
  
  int _hardware()
  {
    uint8_t val = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
      val <<= 1;
      if (digitalRead(_pin)) val++;  // TODO register optimized reading for speed?
    }
    return val ^ _seed;
  }

  int _analog()
  {
    uint8_t val = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
      val <<= 1;
      if (analogRead(_pin) & 1) val++;
    }
    return val ^ _seed;
  }
};

// TODO alternative random nummer generator so all platforms behave same.

// -- END OF FILE --
