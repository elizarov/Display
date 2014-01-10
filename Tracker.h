#ifndef TRACKER_H_
#define TRACKER_H_

#include <Arduino.h>

class Tracker {
  private:
    enum Mode {
      MODE_REGULAR = 0,
      MODE_URGENT = '*',
      MODE_MANUAL = 0xff
    };

    uint8_t        _n;
    uint8_t        _width;
    char*          _modules;
    unsigned long* _last_time;
    char*          _last_buf;
    int8_t         _scroll;
    int8_t         _cursor;
    Mode           _mode;
    unsigned long  _show_time;

    void updateScroll();
    boolean check(uint8_t i, unsigned long now);

  public:
    Tracker(char* modules, uint8_t width);

    int8_t process(char* buf, boolean urgent);
    int8_t status(char* buf);
    void move(int8_t direction);
    char* last(int8_t i);
    boolean urgent();
};

inline boolean Tracker::urgent() {
  return _mode == MODE_URGENT;
}

#endif /* TRACKER_H_ */

