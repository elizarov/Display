#include "DebounceButton.h"

const unsigned long DEBOUNCE_DELAY = 20; // 20ms

DebounceButton::DebounceButton(uint8_t pin) : 
  _pin(pin)
{
  digitalWrite(pin, 1); // pull up
}

boolean DebounceButton::state() {
  boolean state = !digitalRead(_pin);
  unsigned long now = millis();
  boolean debounceTimePassed = now - _last >= DEBOUNCE_DELAY;
  if (state != _state) {
    if (debounceTimePassed) {
      _state = state;
      _last = now;
    }
  } else {
    if (debounceTimePassed)
      _last = now - DEBOUNCE_DELAY; // protect from rollover
  }
  return _state;
}
