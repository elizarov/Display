#include "Parser.h"

Parser::Parser(uint8_t width) :
  _width(width),
  _state(STATE_OUT),
  _buf((char*)malloc(width + 1))
{}

boolean Parser::parse(char c) {
  boolean append = false;
  switch (c) {
  case '[':
    _state = STATE_MAIN_TAG;
    _size = 0;
    break;
  case ']':
    _state = _state == STATE_BODY || _state == STATE_BODY_REST ? STATE_OVER : STATE_OUT;
    _urgent = 0; // will count trailing '*' in STATE_OVER
    break;
  case '\r':
    _state = _state == STATE_OVER ? STATE_0D : STATE_OUT;
    break;
  case '\n':
    if (_state == STATE_0D) {
      // message is over with '\r\n'
      _state = STATE_OUT;
      return true;
    }
    _state = STATE_OUT;
    break;
  default:
    switch (_state) {
    case STATE_MAIN_TAG:
      if (c == ':') {
        if (_size > 0) {
          _state = STATE_BODY; 
          append = true;
        } else
          _state = STATE_OUT;
      } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
        append = true;
      } else
        _state = STATE_OUT;
      break;
    case STATE_BODY:
      if (c == ';')
        _state = STATE_BODY_REST;
      else
        append = true;
      break;
    case STATE_OVER:
      if (c == '*')
        _urgent++;
      else
        _urgent = 0;
      break;  
    case STATE_0D:
      _state = STATE_OUT; // anything but '\n' was seen after '\r'
      break;
    }
  }
  if (append && _size < _width)
    _buf[_size++] = c;
  return false;
}
 
char* Parser::buffer() {
  for (uint8_t i = _size; i < _width; i++)
    _buf[i] = ' ';
  _buf[_width] = 0;
  return _buf;
}

