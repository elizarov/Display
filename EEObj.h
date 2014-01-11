#ifndef EE_OBJ_H_
#define EE_OBJ_H_

#include <Arduino.h>
#include <avr/eeprom.h>

template<typename T> class EEByte {
public:
  EEByte();
  T read();
  EEByte<T>& operator = (T value);
private:
  EEByte(const EEByte<T>& other); // disable copy constructor
  byte _placeholder;
};

template<typename T> EEByte<T>::EEByte() {} // default constructor is empty

template<typename T> T EEByte<T>::read() {
  return (T)eeprom_read_byte((uint8_t*)this);
}

template<typename T> EEByte<T>& EEByte<T>::operator = (T value) {
  eeprom_write_byte((uint8_t*)this, (byte)value);
  return *this;
}


#endif /* EE_OBJ_H_ */