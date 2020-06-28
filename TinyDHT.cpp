/*!
 * @file TinyDHT.cpp
 *
 * @mainpage Adafruit TinyDHT Sensor Library
 *
 * @section intro_sec Introduction
 *
 * Integer version of the Adafruit DHT library for the
 * Trinket and Gemma mini microcontrollers
 *
 * @section license License
 *
 * MIT license
 *
 * @section author Author
 *
 * Written by Adafruit Industries
 */

#include "TinyDHT.h"

DHT::DHT(uint8_t pin, uint8_t type, uint8_t count) {
  _pin = pin;
  _type = type;
  _count = count;
  firstreading = true;
}

void DHT::begin(void) {
  // set up the pins!
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);
  _lastreadtime = 0;
}

// boolean S == Scale.  True == Farenheit; False == Celcius
int16_t DHT::readTemperature(bool S) {
  int16_t f;

  if (read()) {
    switch (_type) {
    case DHT11:
      f = (int16_t)data[2];
      if (S)
        f = convertCtoF(f);

      return f;
    case DHT22:
    case DHT21:
      f = (int16_t)(data[2] & 0x7F);
      f *= 256;
      f += (int16_t)data[3];
      f /= 10;
      if (data[2] & 0x80)
        f *= -1;
      if (S)
        f = convertCtoF(f);

      return f;
    }
  }
  /* Serial.print("Read fail"); */
  return BAD_TEMP; // Bad read, return value (from TinyDHT.h)
}

int16_t DHT::convertCtoF(int16_t c) { return (c * 9) / 5 + 32; }

uint8_t DHT::readHumidity(void) { //  0-100 %
  uint8_t f;
  uint16_t f2; // bigger to allow for math operations
  if (read()) {
    switch (_type) {
    case DHT11:
      f = data[0];
      return f;
    case DHT22:
    case DHT21:
      f2 = (uint16_t)data[0];
      f2 *= 256;
      f2 += data[1];
      f2 /= 10;
      f = (uint8_t)f2;
      return f;
    }
  }
  /* Serial.print("Read fail"); */
  return BAD_HUM; // return bad value (defined in TinyDHT.h)
}

boolean DHT::read(void) {
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  unsigned long currenttime;

  // pull the pin high and wait 250 milliseconds
  digitalWrite(_pin, HIGH);
  delay(250);

  currenttime = millis();
  if (currenttime < _lastreadtime) {
    // ie there was a rollover
    _lastreadtime = 0;
  }
  if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
    return true; // return last correct measurement
    // delay(2000 - (currenttime - _lastreadtime));
  }
  firstreading = false;
  /*
    Serial.print("Currtime: "); Serial.print(currenttime);
    Serial.print(" Lasttime: "); Serial.print(_lastreadtime);
  */
  _lastreadtime = millis();

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // now pull it low for ~20 milliseconds
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(20);
  cli();
  digitalWrite(_pin, HIGH);
  delayMicroseconds(40);
  pinMode(_pin, INPUT);

  // read in timings
  for (i = 0; i < MAXTIMINGS; i++) {
    counter = 0;
    while (digitalRead(_pin) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = digitalRead(_pin);

    if (counter == 255)
      break;

    // ignore first 3 transitions
    if ((i >= 4) && (i % 2 == 0)) {
      // shove each bit into the storage bytes
      data[j / 8] <<= 1;
      if (counter > _count)
        data[j / 8] |= 1;
      j++;
    }
  }

  sei();

  /*
  Serial.println(j, DEC);
  Serial.print(data[0], HEX); Serial.print(", ");
  Serial.print(data[1], HEX); Serial.print(", ");
  Serial.print(data[2], HEX); Serial.print(", ");
  Serial.print(data[3], HEX); Serial.print(", ");
  Serial.print(data[4], HEX); Serial.print(" =? ");
  Serial.println(data[0] + data[1] + data[2] + data[3], HEX);
  */

  // check we read 40 bits and that the checksum matches
  if ((j >= 40) &&
      (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
    return true;
  }

  return false;
}
