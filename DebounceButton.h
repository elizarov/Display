#ifndef DEBOUNCE_BUTTON_H_
#define DEBOUNCE_BUTTON_H_

#include <Arduino.h>

class DebounceButton {
  private:
    uint8_t       _pin;
    unsigned long _last;
    boolean       _state;

  public:
    DebounceButton(uint8_t pin);

    boolean state();
};

#endif /* DEBOUNCE_BUTTON_H_ */
