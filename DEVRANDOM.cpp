//
//    FILE: DEVRANDOM.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.2.2
// PURPOSE: Arduino library for a /dev/random stream - useful for testing
//     URL: https://github.com/RobTillaart/DEVRANDOM


#include "DEVRANDOM.h"


DEVRANDOM::DEVRANDOM()
{
  _next = _rnd();
};


DEVRANDOM::DEVRANDOM(const char * str)
{
  this->print(str);
  _next = _rnd();
};


DEVRANDOM::DEVRANDOM(const uint32_t value)
{
   this->print(value);
  _next = _rnd();
};


DEVRANDOM::DEVRANDOM(const float value)
{
  this->print(value, 6);
  _next = _rnd();
};


int DEVRANDOM::available()
{
  return 1;
};


int DEVRANDOM::peek()
{
  return _next;
};


int DEVRANDOM::read()
{
  uint8_t x = _next;
  _next = _rnd();
  return x;
};


//  keep CI happy as parent class flush is virtual.
void DEVRANDOM::flush()
{
};


size_t DEVRANDOM::write(const uint8_t data)
{
  return write(&data, 1);
};


size_t DEVRANDOM::write(const uint8_t * buffer, size_t size)
{
  uint32_t tmp = _seed;
  for (size_t i = 0; i < size; i++)
  {
    _seed = _seed * (_seed << 8) + buffer[i];
  }
  if (_mode == 0)
  {
    randomSeed(_seed);
  }
  if (_mode == 3)
  {
    _m_z = (_seed == 0) ? 1 : _seed;
    _m_w = ( tmp  == 0) ? 2 : tmp;
  }
  return size;
};


size_t DEVRANDOM::write(const uint32_t * buffer, uint8_t size)
{
  if (_mode == 4)
  {
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
  return 4;
}

void DEVRANDOM::useTinyMersenneTwister()
{
  _mode = 4;
};

void DEVRANDOM::useMarsaglia()
{
  _mode = 3;
};


void DEVRANDOM::useAnalogRead(uint8_t pin)
{
  _mode = 2;
  _pin = pin;
};


void DEVRANDOM::useDigitalRead(uint8_t pin)
{
  _mode = 1;
  _pin = pin;
  pinMode(_pin, INPUT);
};


void DEVRANDOM::useRandom()
{
  _mode = 0;
};


uint8_t DEVRANDOM::getMode()
{
  return _mode;
};


int DEVRANDOM::_rnd()
{
  if (_mode == 0 ) return random(256);
  if (_mode == 1 ) return _digitalRead();
  if (_mode == 2 ) return _analogRead();
  if (_mode == 3 ) return _marsaglia();
  if (_mode == 4 ) return _tinymersennetwister();
  return 0;
}


int DEVRANDOM::_digitalRead()
{
  uint8_t value = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    value <<= 1;
    if (digitalRead(_pin)) value++;
  }
  return value ^ _seed;
}


int DEVRANDOM::_analogRead()
{
  uint8_t value = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    value <<= 1;
    if (analogRead(_pin) & 1) value++;
  }
  return value ^ _seed;
}

//  An example of a simple pseudo-random number generator is the
//  Multiply-with-carry method invented by George Marsaglia.
//  two initializers (not null)
uint32_t DEVRANDOM::_marsaglia()
{
  _m_z = 36969L * (_m_z & 65535L) + (_m_z >> 16);
  _m_w = 18000L * (_m_w & 65535L) + (_m_w >> 16);
  return (_m_z << 16) + _m_w;  /* 32-bit result */
}

/**
 * This function certificate the period of 2^127-1.
 * @param random tinymt state vector.
 */
//static 
void DEVRANDOM::period_certification(tinymt32_t * random) {
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

/**
 * This function changes internal state of tinymt32.
 * Users should not call this function directly.
 * @param random tinymt internal status
 */
//inline static 
void DEVRANDOM::tinymt32_next_state(tinymt32_t * random) {
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

/**
 * This function outputs 32-bit unsigned integer from internal state.
 * Users should not call this function directly.
 * @param random tinymt internal status
 * @return 32-bit unsigned pseudorandom number
 */
//inline static 
uint32_t DEVRANDOM::tinymt32_temper(tinymt32_t * random) {
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

uint32_t DEVRANDOM::_tinymersennetwister()
{
	tinymt32_next_state(&tinymt);
	return tinymt32_temper(&tinymt);
}


//  -- END OF FILE --

