#include "TimeoutLed.h"

TimeoutLed::TimeoutLed(uint8_t pin) : 
  _pin(pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 0);
}

void TimeoutLed::light(unsigned int interval) {
  digitalWrite(_pin, 1);
  _timeout.reset(interval);
}

void TimeoutLed::check() {
  if (_timeout.check())
    digitalWrite(_pin, 0);
}
