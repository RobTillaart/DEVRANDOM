//
//    FILE: DEVRANDOM_compile_time_random_seed.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo compile time random seed
//    DATE: 2020-06-23
//    (c) : MIT
//


#include "DEVRANDOM.h"

#define LOOPS 10000

// Choose random number generator 1..5
#define RND_5

#ifdef RND_1
#define PROGSIZE 2990
#define MEMSIZE 329
random_t dr(0);
#endif  // ??? #elseifdef
#ifdef RND_2
#define PROGSIZE 1000
digitalRandom_t dr(1);
#endif
#ifdef RND_3
#define PROGSIZE 1000
analogRandom_t dr(0, A0);
#endif
#ifdef RND_4
#define PROGSIZE 1000
marsaglia_t dr(0, 0);
#endif
#ifdef RND_5
#define PROGSIZE 1000
const uint32_t buffer[] = { 1, 0, 0, 0 };
tinyMersenneTwister_t dr(buffer, sizeof(buffer) / sizeof(buffer[0]));
#endif

uint32_t i = 0, start, stop;
volatile int x;


void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.println();

  //  setup compile time random seed
  //dr.print(__FILE__); promblemA: can't inherited Stream
  //dr.print(__DATE__);
  //dr.print(__TIME__);
  Serial.print(F("\nSketch uses "));
  Serial.print(PROGSIZE);
  Serial.println(F(" bytes"));
  Serial.print(F("Class: "));
  Serial.println(dr.getName());
}


void loop() {
  i++;
  if (dr.available()) {
    uint32_t ztime = micros();
    for (uint32_t n = 0; n < LOOPS; n++) {
      dr.read();
    }
    ztime = micros() - ztime;
    Serial.print(dr.getName());
    Serial.print(F(" Loops: "));
    Serial.print(LOOPS);
    Serial.print(F(", Micro seconds: "));
    Serial.print(ztime);
    Serial.print(F(", RN per second: "));
    Serial.println((1000000.0*LOOPS)/ztime,0);
  }
  delay(100);
}


// -- END OF FILE --
