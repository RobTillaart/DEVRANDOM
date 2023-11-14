//
//    FILE: DEVRANDOM_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//    DATE: 2020-06-23
//

// USE WITH SERIAL PLOTTER


#include "DEVRANDOM.h"

DEVRANDOM dr;

volatile int a, b, c, d;

// Run the Linux version from command line (seed==1):
//   ./check32 0 0 0
// and get the expected result:
uint32_t validtmt[5] = { 1604930872, 1643955479, 1857048859, 192927567, 3800340839 };

void setup() {
  uint32_t ms = micros();
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.println();

  Serial.print("Boot ms: ");
  Serial.println(ms);
  ms = micros();
  // Init with seed = 1, and mat1, mat2 and tmat = 0
  uint32_t buffer[4] = { 1, 0, 0, 0 };
  dr.useTinyMersenneTwister();
  dr.write(buffer, sizeof(buffer)/sizeof(buffer[0]));
  Serial.print("TMT init ms: ");
  Serial.println(micros()-ms);

  uint8_t *p = (uint8_t *)&dr.tinymt;
  for (uint8_t i = 0; i < sizeof(dr.tinymt); i++) {
    Serial.print(*p++, HEX);
  }
  Serial.println();

  Serial.println("SOFTWARE\tDIGITAL\tANALOG\tMARSAGLIA");
}

int cnt = 0;

void loop() {

  dr.useRandom();
  a = dr.read();
  a = dr.read();
  dr.useDigitalRead(4);
  b = dr.read();
  b = dr.read();
  dr.useAnalogRead(A0);
  c = dr.read();
  c = dr.read();
  dr.useMarsaglia();
  d = dr.read();
  d = dr.read();

  Serial.print(a);
  Serial.print("\t");
  Serial.print(b);
  Serial.print("\t");
  Serial.print(c);
  Serial.print("\t");
  Serial.print(d);
  //  Serial.println();

  dr.useTinyMersenneTwister();
  uint32_t tmt = dr._tinymersennetwister();
  Serial.print("\ttmt: ");
  Serial.print(tmt, DEC);
  if (validtmt[cnt] == tmt) {
    Serial.print(" OK tmt");
  } else {
    Serial.print(" Error tmt");
  }
  Serial.println();
  if (++cnt == 5)
    while (1)
      ;
}


// -- END OF FILE --
