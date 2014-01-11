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
    unsigned long* _receive_time;
    char*          _last_buf;
    int8_t*        _last_queue;
    boolean*       _in_queue;
    uint8_t        _queue_head;
    uint8_t        _queue_tail;
    
    int8_t         _scroll;
    int8_t         _cursor;
    Mode           _mode;
    unsigned long  _show_time;
    uint8_t        _urgent;

    void updateScroll();
    boolean check(uint8_t i, unsigned long now);
    void getLast(char* buf, int8_t i);

  public:
    Tracker(char* modules, uint8_t width);

    int8_t process(char* buf, uint8_t urgent);
    void move(int8_t direction);

    boolean message(char* buf);
    boolean status(char* buf);
    
    boolean urgent();
};

inline boolean Tracker::urgent() {
  return _mode == MODE_URGENT;  
}

#endif /* TRACKER_H_ */

