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

class DEVRANDOM_t //: public Stream 
{
  uint8_t  _next = 0;
  virtual int _rnd() {
    return 0;
  }
public:
  virtual int available() {
    return 1;
  }
  virtual int peek() {
    return _next;
  }
  virtual int read() {
    uint8_t x = _next;
    _next = _rnd();
    return x;
  }
  virtual const __FlashStringHelper* getName(){ return F("<no name>"); }
};

class random_t : public DEVRANDOM_t {
public:
  random_t(int _seed) {
    randomSeed(_seed);
  }
  virtual const __FlashStringHelper* getName(){ return F("random_t");}
private:
  virtual int _rnd() {
    return random(256);
  }
};

class digitalRandom_t : public DEVRANDOM_t {
public:
  digitalRandom_t(uint8_t _pin) {
    this->_pin = _pin;
    pinMode(_pin, INPUT);
  }
  virtual const __FlashStringHelper* getName(){ return F("digitalRandom_t");}
private:
  int _seed = 0;
  uint8_t _pin;
  virtual int _rnd() {
    uint8_t value = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
      value <<= 1;
      if (digitalRead(_pin)) value++;
    }
    return value ^ _seed;
  }
};

class analogRandom_t : public DEVRANDOM_t {
public:
  analogRandom_t(int _seed, uint8_t _pin) {
    this->_seed = _seed;
    this->_pin = _pin;
  }
  virtual const __FlashStringHelper* getName(){ return F("analogRandom_t");}
private:
  int _seed = 0;
  uint8_t _pin = 0;
  virtual int _rnd() {
    uint8_t value = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
      value <<= 1;
      if (analogRead(_pin) & 1) value++;
    }
    return value ^ _seed;
  }
};

class marsaglia_t : public DEVRANDOM_t {
  //  Marsaglia 'constants'
  uint32_t _m_w  = 1;
  uint32_t _m_z  = 2;
public:
  marsaglia_t(uint32_t _seed, uint32_t tmp) {
    _m_z = (_seed == 0) ? 1 : _seed;
    _m_w = ( tmp  == 0) ? 2 : tmp;
  }
  virtual const __FlashStringHelper* getName(){ return F("marsaglia_t");}
private:
  virtual int _rnd() {
    _m_z = 36969L * (_m_z & 65535L) + (_m_z >> 16);
    _m_w = 18000L * (_m_w & 65535L) + (_m_w >> 16);
    return (_m_z << 16) + _m_w;
  }
};

class tinyMersenneTwister_t : public DEVRANDOM_t {
public:
  tinyMersenneTwister_t(const uint32_t * buffer, uint8_t size) {
    _seed = buffer[0];
    tinymt.mat1 = buffer[1];
    tinymt.mat2 = buffer[2];
    tinymt.tmat = buffer[3];
    tinymt.status[0] = _seed;
    tinymt.status[1] = tinymt.mat1;
    tinymt.status[2] = tinymt.mat2;
    tinymt.status[3] = tinymt.tmat;
    for (unsigned int i = 1; i < MIN_LOOP; i++) {
        tinymt.status[i & 3] ^= i + UINT32_C(1812433253)
            * (tinymt.status[(i - 1) & 3]
               ^ (tinymt.status[(i - 1) & 3] >> 30));
    }
    period_certification(&tinymt);
    for (unsigned int i = 0; i < PRE_LOOP; i++) {
        tinymt32_next_state(&tinymt);
    }
  }
  virtual const __FlashStringHelper* getName(){ return F("tinyMersenneTwister_t");}
private:
  // Tiny Mersenne-Twister
  tinymt32_t tinymt;
  uint32_t _seed;
  virtual int _rnd() {
    tinymt32_next_state(&tinymt);
    return tinymt32_temper(&tinymt) & 0xFF;
  }
  void period_certification(tinymt32_t * random) {
    if ((random->status[0] & TINYMT32_MASK) == 0 &&
        random->status[1] == 0 &&
        random->status[2] == 0 &&
        random->status[3] == 0) {
        random->status[0] = 'T';
        random->status[1] = 'I';
        random->status[2] = 'N';
        random->status[3] = 'Y';
    }
  }
  void tinymt32_next_state(tinymt32_t * random) {
    uint32_t x;
    uint32_t y;

    y = random->status[3];
    x = (random->status[0] & TINYMT32_MASK)
        ^ random->status[1]
        ^ random->status[2];
    x ^= (x << TINYMT32_SH0);
    y ^= (y >> TINYMT32_SH0) ^ x;
    random->status[0] = random->status[1];
    random->status[1] = random->status[2];
    random->status[2] = x ^ (y << TINYMT32_SH1);
    random->status[3] = y;
    int32_t const a = -((int32_t)(y & 1)) & (int32_t)random->mat1;
    int32_t const b = -((int32_t)(y & 1)) & (int32_t)random->mat2;
    random->status[1] ^= (uint32_t)a;
    random->status[2] ^= (uint32_t)b;
  }
  uint32_t tinymt32_temper(tinymt32_t * random) {
    uint32_t t0, t1;
    t0 = random->status[3];
#if defined(LINEARITY_CHECK)
    t1 = random->status[0]
        ^ (random->status[2] >> TINYMT32_SH8);
#else
    t1 = random->status[0]
        + (random->status[2] >> TINYMT32_SH8);
#endif
    t0 ^= t1;
    if ((t1 & 1) != 0) {
        t0 ^= random->tmat;
    }
    return t0;
  }
};


//  -- END OF FILE --

