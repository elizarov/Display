#ifndef TRACKER_H_
#define TRACKER_H_

#include <Arduino.h>

class Tracker {         
  public:
    static const int8_t N_TAGS = 36;
    static const int8_t WIDTH = 16;

  private:
    enum Mode {
      MODE_REGULAR = 0,
      MODE_URGENT = '*',
      MODE_MANUAL = 0xff
    };

    unsigned long  _receive_time[N_TAGS];
    char           _last_buf[N_TAGS * WIDTH];
    int8_t         _last_queue[N_TAGS + 1];
    boolean        _in_queue[N_TAGS];
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
    Tracker();

    int8_t indexOfTag(char c);
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

