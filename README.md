# DEVNULL

Arduino library to wrap a random generator in a stream

## Description

The library implements a stream class that mimics the /dev/random
device of a linux system. It can be used for testing with streams.

**Stream interface**
* **available()** There is always 1 next byte available
* **peek()** will give you next byte
* **read()** will give you next byte and generate a new one.
* **write()** data will be used for reseeding the random number generator (RNG)
(SW mode only, in HW mode and AN mode the seed is used to XOR the value, so it
does have some effect);

**Random generator selection**

* **useSW()** use a software random number generator. This is the default.
By default the build-in random number generator is used. 
This can be replaced by a RNG of your choice. 

* **useDR(pin)** use digitalRead to read 8 bits from a defined pin.
One can build a hardware RNG that flips between 0 and 1 very rapidly and unpredictaby.
Connect this signal to the pin and it will be read and generate a random byte.
The seed value from the write is used as a XOR byte.

* **useAR(pin)** use the analogRead to read 8 bits
This can be fed with an analog noise source.
The seed value from the write is used as a XOR byte.


As **write()** reseeds the RNG, printing to DEVRANDOM will also reseed the RNG.

As shown in the example one can use fscanf to read larger datatypes, 
```cpp
  uint32_t x;
  fscanf((FILE*) &dr, "%lu", &x);
  Serial.println(x);
```
However float is not supported by UNO and strings (%s) generate garbage.
So a password generator is a bit more difficult (and a good exercise).

## Operation

See example
