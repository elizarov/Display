#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

#include "EEObj.h"
#include "Tracker.h"

class Config {
public:
  EEByte<char>        thisDisplayTag;
  EEByte<byte>        _reserved[15];
  EEByte<byte>        hiddenTag[Tracker::N_TAGS];
};

extern Config config;

#endif /* CONFIG_H_ */
