#pragma once
//
//    FILE: DEVRANDOM.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.2.2
// PURPOSE: Arduino library for a /dev/random stream - useful for testing
//     URL: https://github.com/RobTillaart/DEVRANDOM


#include "Arduino.h"


#define  DEVRANDOM_LIB_VERSION      (F("0.2.2"))


#define  DEVRANDOM_MODE_RANDOM       0
#define  DEVRANDOM_MODE_DIGITALREAD  1
#define  DEVRANDOM_MODE_ANALOGREAD   2
#define  DEVRANDOM_MODE_MARSAGLIA    3

// TineMT constants
#define MIN_LOOP 8
#define PRE_LOOP 8
#define TINYMT32_MEXP 127
#define TINYMT32_SH0 1
#define TINYMT32_SH1 10
#define TINYMT32_SH8 8
#define TINYMT32_MASK UINT32_C(0x7fffffff)
#define TINYMT32_MUL (1.0f / 16777216.0f)

/**
 * tinymt32 internal state vector and parameters
 */
struct TINYMT32_T {
    uint32_t status[4];
    uint32_t mat1;
    uint32_t mat2;
    uint32_t tmat;
};

typedef struct TINYMT32_T tinymt32_t;

class DEVRANDOM : public Stream
{
public:
  DEVRANDOM();
  DEVRANDOM(const char * str);
  DEVRANDOM(const uint32_t value);
  DEVRANDOM(const float value);

  // Tiny Mersenne-Twister
  tinymt32_t tinymt;

  int     available();
  int     peek();
  int     read();
  uint32_t _tinymersennetwister();

  //      keep CI happy as parent class flush is virtual.
  void    flush();

  //      for reseeding, including via print() and println().
  size_t  write(const uint8_t data);
  size_t  write(const uint8_t * buffer, size_t size);
  size_t  write(const uint32_t * buffer, uint8_t size);

  //      build in random is the default mode.
  void    useRandom();
  void    useDigitalRead(uint8_t pin);
  void    useAnalogRead(uint8_t pin);
  void    useMarsaglia();
  void    useTinyMersenneTwister();

  uint8_t getMode();


  //  OBSOLETE
  //  void    useAR(uint8_t pin) { useAnalogRead(pin);  };
  //  void    useHW(uint8_t pin) { useDigitalRead(pin); };
  //  void    useSW()            { useRandom();         };


private:
  uint8_t  _next = 0;
  uint32_t _seed = 0;
  uint8_t  _mode = 0;
  uint8_t  _pin  = 0;

  //  Marsaglia 'constants'
  uint32_t _m_w  = 1;
  uint32_t _m_z  = 2;

  int      _rnd();
  int      _digitalRead();
  int      _analogRead();
  uint32_t _marsaglia();
  //uint32_t _tinymersennetwister();
  void period_certification(tinymt32_t * random);
  void tinymt32_next_state(tinymt32_t * random);
  uint32_t tinymt32_temper(tinymt32_t * random);

};


//  -- END OF FILE --

