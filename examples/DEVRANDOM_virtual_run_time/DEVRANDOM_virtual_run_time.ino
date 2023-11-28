//
//    FILE: DEVRANDOM_compile_time_random_seed.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo compile time random seed
//    DATE: 2020-06-23
//    (c) : MIT
//

/*
How to choose a pseudo random number generator:
 * Randomnes (periode)
 * Speed
 * Use of PROGMEM/flash
 * Use of memory
*/

#include "DEVRANDOM.h"

#define LOOPS 10000

DEVRANDOM_t *rnglist[5];

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.println();

  rnglist[0] = new random_t(0);
  rnglist[1] = new digitalRandom_t(1);
  rnglist[2] = new analogRandom_t(0, A0);
  rnglist[3] = new marsaglia_t(0, 0);
  const uint32_t buffer[] = { 1, 0, 0, 0 };
  rnglist[4] = new tinyMersenneTwister_t(buffer, sizeof(buffer) / sizeof(buffer[0]));

  //  setup compile time random seed
  //dr.print(__FILE__); promblemA: can't inherited Stream
  //dr.print(__DATE__);
  //dr.print(__TIME__);
}

void loop() {
  for (uint8_t i = 0; i < sizeof(rnglist) / sizeof(rnglist[0]); i++) {
    DEVRANDOM_t *dr = rnglist[i];
    if (dr->available()) {
      uint32_t ztime = micros();
      for (uint32_t n = 0; n < LOOPS; n++) {
        dr->read();
      }
      ztime = micros() - ztime;
      Serial.print(dr->getName());
      Serial.print(F(" Loops: "));
      Serial.print(LOOPS);
      Serial.print(F(", Micro seconds: "));
      Serial.print(ztime);
      Serial.print(F(", RN per second: "));
      Serial.println((1000000.0 * LOOPS) / ztime, 0);
    }
  }
  Serial.println();
  delay(100);
}


// -- END OF FILE --
