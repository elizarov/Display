#ifndef PARSER_H_
#define PARSER_H_

#include <Arduino.h>

//
// ... [ ... : ... ;  ... ] ... [*]\r\n
//  |     |     |      |     |   |    |
//  |     |     |      |     |   |    +- STATE_0D
//  |     |     |      |     |   +- urgent
//  |     |     |      |     +- STATE_OVER
//  |     |     |      +- STATE_BODY_REST
//  |     |     +- STATE_BODY
//  |     +- STATE_MAIN_TAG
//  +- STATE_OUT               
//

class Parser {
  private:
    enum State {
      STATE_OUT,
      STATE_MAIN_TAG,
      STATE_BODY,
      STATE_BODY_REST,
      STATE_OVER,
      STATE_0D
    };

    uint8_t       _width;
    State         _state;
    uint8_t       _urgent;
    char*         _buf;
    uint8_t       _size;

  public:
    Parser(uint8_t width);
  
    boolean parse(char c);
    char* buffer();
    uint8_t urgent();
};

inline uint8_t Parser::urgent() {
  return _urgent;
}

#endif /* PARSER_H_ */
