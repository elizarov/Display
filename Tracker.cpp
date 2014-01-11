#include "Tracker.h"
#include "Config.h"
  
const unsigned long TRACKER_TIMEOUT = 5 * 60000L; // 5 min
const unsigned long MIN_TIME = 2000; // 2 sec
const unsigned long CURSOR_SHOW_TIME = 10000; // 10 sec
const unsigned long BLINK_PERIOD = 300; // 300ms

Tracker::Tracker() :
  _mode(MODE_REGULAR)
{}

void Tracker::updateScroll() {
  if (_cursor >= _scroll + WIDTH) {
    // count unhidden tags
    int8_t cnt = 0;
    for (int8_t i = _cursor; i >= 0; i--)
      if (!config.hiddenTag[i].read())
        if (++cnt >= WIDTH) {
          _scroll = i;
          break;
        }
  } else if (_cursor < _scroll)
    _scroll = _cursor;
}

int8_t Tracker::indexOfTag(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'Z')
    return c - 'A' + 10;
  else
    return -1;
}

int8_t Tracker::process(char* buf, uint8_t urgent) {
  if (buf[1] != ':')
    return -1;
  int8_t i = indexOfTag(buf[0]);
  if (i < 0)
    return -1;
  // unhide in config
  if (config.hiddenTag[i].read())
    config.hiddenTag[i] = 0;
  // process 
  unsigned long now = millis();
  if (!now)
    now = 1;
  _receive_time[i] = now; // when received
  strncpy(_last_buf + (i * WIDTH), buf, WIDTH);
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
    if (_queue_tail > N_TAGS)
      _queue_tail = 0;
  }
  return i;
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
  char* last = _last_buf + (i * WIDTH);
  strncpy(buf, last, WIDTH);
  buf[WIDTH] = 0;
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
  if (_queue_head > N_TAGS)
    _queue_head = 0;
  _in_queue[i] = false;
  _show_time = now;
  getLast(buf, i);
  return true;
}

boolean Tracker::status(char* buf) {
  long now = millis();
  int8_t cursor = _mode != MODE_REGULAR ? _cursor : -1;
  int8_t i = _scroll; 
  int8_t p = 0;
  while (p < WIDTH) {
    while (i < N_TAGS && config.hiddenTag[i].read())
      i++;
    if (i >= N_TAGS)
      break;
    char c = i < 10 ? i + '0' : i - 10 + 'A';  
    buf[p++] = i == cursor ? 
      ((now - _show_time) % (2 * BLINK_PERIOD) < BLINK_PERIOD ? _mode : c) :
      (check(i, now) ? c : ' ');
    i++;  
  }
  buf[p] = 0;
  return true; // always update status -- do not keep track on when it actually changes
}

void Tracker::move(int8_t direction) {
  unsigned long now = millis();
  // on first press -- leave cursor at last position
  // otherwise -- search
  if (_mode == MODE_MANUAL || !check(_cursor, now)) {
    boolean found = false;
    for (int8_t att = 0; att < N_TAGS; att++) {
      _cursor += direction;
      if (_cursor < 0)
        _cursor = N_TAGS - 1;
      else if (_cursor >= N_TAGS)
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


