#ifndef BLINK_LED_H_
#define BLINK_LED_H_

#include <Arduino.h>

class BlinkLed {
  private:
    uint8_t       _pin;
    unsigned long _time;
    boolean       _state;

  public:
    BlinkLed(uint8_t pin);

    void blink(unsigned int interval);
};

#endif /* BLINK_LED_H_ */
