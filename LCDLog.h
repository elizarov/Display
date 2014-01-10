#ifndef LCD_LOG_H_
#define LCD_LOG_H_

#include <Arduino.h>
#include <LiquidCrystal.h>

class LCDLog : public LiquidCrystal {
  private:
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _row;
    uint8_t _col;
    boolean _clear_on_write;

  public:
    LCDLog(uint8_t rs, uint8_t rw, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
           uint8_t cols, uint8_t rows);

    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row); 
    virtual size_t write(uint8_t c);

    void clearToRight();
};

#endif /* LCD_LOG_H_ */
