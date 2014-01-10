#include "ActionButton.h"

const unsigned long REPEAT_DELAY = 1000; // 1sec
const unsigned long REPEAT_INTERVAL = 200; // 200ms

ActionButton::ActionButton(uint8_t pin) :
  _button(pin)
{}

boolean ActionButton::check() {
  if (_button.state()) {
    // button down
    if (_timeout.enabled()) {
      // was down -- repeat via timeout
      if (_timeout.check()) {
        _timeout.reset(REPEAT_INTERVAL);
        return true;
      }
    } else {
      // first time down
      _timeout.reset(REPEAT_DELAY);
      return true;
    }
  } else {
    // button up
    _timeout.disable();
  }
  return false;
}
