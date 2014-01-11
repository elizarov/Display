#include "Tracker.h"
  
const unsigned long TRACKER_TIMEOUT = 5 * 60000L; // 5 min
const unsigned long MIN_TIME = 2000; // 2 sec
const unsigned long CURSOR_SHOW_TIME = 10000; // 10 sec
const unsigned long BLINK_PERIOD = 300; // 300ms

Tracker::Tracker(char* modules, uint8_t width) : 
  _n(strlen(modules)),
  _modules(modules),
  _width(width),
  _receive_time((unsigned long*)malloc(_n * sizeof(unsigned long))),
  _last_buf((char*)malloc(_n * width)),
  _last_queue((int8_t*)malloc(_n + 1)), // n+1 item to easily distinguish empty and full queue
  _in_queue((boolean*)malloc(_n)),
  _mode(MODE_REGULAR)
{
  for (int8_t i = 0; i < _n; i++) {
    _receive_time[i] = 0;
    _in_queue[i] = false;  
  }
}

void Tracker::updateScroll() {
  if (_cursor >= _scroll + _width)
    _scroll = _cursor - _width + 1;
  else if (_cursor < _scroll)
    _scroll = _cursor;
}

int8_t Tracker::process(char* buf, uint8_t urgent) {
  if (buf[1] != ':')
    return -1;
  char c = buf[0];
  for (int8_t i = 0; i < _n; i++)
    if (c == _modules[i]) {
      unsigned long now = millis();
      if (!now)
        now = 1;
      _receive_time[i] = now; // when received
      strncpy(_last_buf + (i * _width), buf, _width);
      if (urgent && urgent >= _urgent) {
        // display immediately
        _mode = MODE_URGENT;
        _show_time = 0;
        _urgent = urgent;
        _cursor = i;
        updateScroll();
      } else if (!_in_queue[i]) {
        // enqueue for later display
        _in_queue[i] = true;
        _last_queue[_queue_tail++] = i;
        if (_queue_tail > _n)
          _queue_tail = 0;
      }
      return i;
    }
  return -1;  
}

boolean Tracker::check(uint8_t i, unsigned long now) {
  if (_receive_time[i]) {
    boolean ok = now - _receive_time[i] < TRACKER_TIMEOUT;
    if (!ok)
      _receive_time[i] = 0;
    return ok;
  }
  return false;
}

void Tracker::getLast(char* buf, int8_t i) {
  char* last = _last_buf + (i * _width);
  strncpy(buf, last, _width);
  buf[_width] = 0;
}

boolean Tracker::message(char* buf) {
  unsigned long now = millis();
  if (!now)
    now = 1;
  if (_mode != MODE_REGULAR) {
    if (!_show_time) {
      // show for a first time
      _show_time = now;
      getLast(buf, _cursor);
      return true;
    }  
    if (now - _show_time < CURSOR_SHOW_TIME)
      return false; // continue showing
    // clear mode to regular ... 
    _mode = MODE_REGULAR;
    _show_time = 0;
    _urgent = 0;
  }
  if (_show_time && now - _show_time < MIN_TIME)
    return false; // have not shown prev item for min time yet
  // show from queue
  if (_queue_head == _queue_tail)
    return false; // queue is empty - show nothing new to show
  int8_t i = _last_queue[_queue_head++];
  if (_queue_head > _n)
    _queue_head = 0;
  _in_queue[i] = false;
  _show_time = now;
  getLast(buf, i);
  return true;
}

boolean Tracker::status(char* buf) {
  long now = millis();
  int8_t cursor = _mode != MODE_REGULAR ? _cursor : -1;
  int8_t i = 0;
  for (; i < _width && i + _scroll < _n; i++) {
    buf[i] = i + _scroll == cursor ? 
      ((now - _show_time) % (2 * BLINK_PERIOD) < BLINK_PERIOD ? _mode : _modules[i + _scroll]) :
      (check(i + _scroll, now) ? _modules[i + _scroll] : ' ');
  }
  buf[i] = 0;
  return true; // always update status -- do not keep track on when it actually changes
}

void Tracker::move(int8_t direction) {
  unsigned long now = millis();
  // on first press -- leave cursor at last position
  // otherwise -- search
  if (_mode == MODE_MANUAL || !check(_cursor, now)) {
    boolean found = false;
    for (int8_t att = 0; att < _n; att++) {
      _cursor += direction;
      if (_cursor < 0)
        _cursor = _n - 1;
      else if (_cursor >= _n)
        _cursor = 0;
      if (check(_cursor, now)) {
        found = true;
        break;
      }
    }
    if (!found)
      return;
  }
  _mode = MODE_MANUAL;
  _show_time = 0;
  _urgent = 0;
  updateScroll();
}


