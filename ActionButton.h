#ifndef ACTION_BUTTON_H_
#define ACTION_BUTTON_H_

#include <Arduino.h>
#include "DebounceButton.h"
#include "Timeout.h"

class ActionButton {
  private:
    DebounceButton _button;
    Timeout        _timeout;

  public:
    ActionButton(uint8_t pin);

    boolean check();
};

#endif /* ACTION_BUTTON_H_ */
