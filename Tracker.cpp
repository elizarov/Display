#include "Tracker.h"
  
const unsigned long TRACKER_TIMEOUT = 5 * 60000L; // 5 min
const unsigned long SHOW_TIMEOUT = 10000; // 10 sec
const unsigned long BLINK_PERIOD = 300; // 300ms

Tracker::Tracker(char* modules, uint8_t width) : 
  _n(strlen(modules)),
  _modules(modules),
  _width(width),
  _last_time((unsigned long*)malloc(_n * sizeof(unsigned long))),
  _last_buf((char*)malloc(_n * (width + 1))),
  _mode(MODE_REGULAR)
{}

void Tracker::updateScroll() {
  if (_cursor >= _scroll + _width)
    _scroll = _cursor - _width + 1;
  else if (_cursor < _scroll)
    _scroll = _cursor;
}

int8_t Tracker::process(char* buf, boolean urgent) {
  if (buf[1] != ':')
    return -1;
  char c = buf[0];
  for (int8_t i = 0; i < _n; i++)
    if (c == _modules[i]) {
      _last_time[i] = millis();
      if (!_last_time[i])
        _last_time[i] = 1;
      strncpy(_last_buf + (i * (_width + 1)), buf, _width);
      if (urgent) {
        _cursor = i;
        _mode = MODE_URGENT;
        _show_time = millis();
        updateScroll();
      }
      return i;
    }
  return -1;  
}

char* Tracker::last(int8_t i) {
  char* buf = _last_buf + (i * (_width + 1));
  buf[_width] = 0;
  return buf;
}

boolean Tracker::check(uint8_t i, unsigned long now) {
  if (_last_time[i]) {
    boolean ok = now - _last_time[i] < TRACKER_TIMEOUT;
    if (!ok)
      _last_time[i] = 0;
    return ok;
  }
  return false;
}

int8_t Tracker::status(char* buf) {
  unsigned long now = millis();
  boolean show = _mode != MODE_REGULAR && now - _show_time < SHOW_TIMEOUT;
  if (!show)
    _mode = MODE_REGULAR;
  int8_t cursor = show ? _cursor : -1;
  int8_t i = 0;
  for (; i < _width && i + _scroll < _n; i++) {
    buf[i] = i + _scroll == cursor ? 
      ((now - _show_time) % (2 * BLINK_PERIOD) < BLINK_PERIOD ? _mode : _modules[i + _scroll]) :
      (check(i + _scroll, now) ? _modules[i + _scroll] : ' ');
  }
  buf[i] = 0;
  return cursor;
}

void Tracker::move(int8_t direction) {
  unsigned long now = millis();
  // on first press -- leave cursor at last position
  // otherwise -- search
  if (_show_time || !check(_cursor, now)) {
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
  _show_time = now;
  updateScroll();
}


