#ifndef TIMEOUT_LED_H_
#define TIMEOUT_LED_H_

#include <Arduino.h>
#include "Timeout.h"

class TimeoutLed {
  private:
    uint8_t       _pin;
    Timeout       _timeout;

  public:
    TimeoutLed(uint8_t pin);

    void light(unsigned int interval);
    void check();
};

#endif /* TIMEOUT_LED_H_ */
