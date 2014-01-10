#include "LCDLog.h"

LCDLog::LCDLog(uint8_t rs, uint8_t rw, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
           uint8_t cols, uint8_t rows) :
  LiquidCrystal(rs, rw, enable, d0, d1, d2, d3),
  _cols(cols),
  _rows(rows)
{
  begin(cols, rows);
}

void LCDLog::clear() {
  LiquidCrystal::clear();
  _col = 0; 
  _row = 0;
}

void LCDLog::home() {
  LiquidCrystal::home();
  _col = 0; 
  _row = 0;
}

void LCDLog::setCursor(uint8_t col, uint8_t row) {
  LiquidCrystal::setCursor(col, row);
  _col = col;
  _row = row;
}

size_t LCDLog::write(uint8_t c) {
  switch (c) {
  case '\r':
    setCursor(0, _row);
    _clear_on_write = true;
    break;
  case '\n':
    if (_row + 1 >= _rows)
      setCursor(_col, 0);
    else
      setCursor(_col, _row + 1);
    break;
  default:
    if (_clear_on_write) {
      _clear_on_write = false;
      clearToRight();  
    }
    if (_col < _cols) {
      LiquidCrystal::write(c);
      _col++;
    }
  }
}

void LCDLog::clearToRight() {
  uint8_t col = _col;
  while (_col < _cols)
    write(' ');
  setCursor(col, _row);
}
