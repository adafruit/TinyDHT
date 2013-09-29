#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* Tiny DHT library
Uses integer math to save space on Trinket/Gemma

MIT license
written by Adafruit Industries
*/

// how many timing transitions we need to keep track of. 2 * number bits + extra
#define MAXTIMINGS 85

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

// NAN code in DHT library takes space, define bad values here
#define BAD_HUM    -1  // Bad humitidy reading
#define BAD_TEMP -999  // Bad temperature reading

class DHT {
 private:
  uint8_t data[6];
  uint8_t _pin, _type, _count;
  boolean read(void);
  unsigned long _lastreadtime;
  boolean firstreading;

 public:
  DHT(uint8_t pin, uint8_t type, uint8_t count=6);
  void begin(void);
  int16_t readTemperature(bool S=false);
  int16_t convertCtoF(int16_t);
  uint8_t readHumidity(void);

};
#endif
