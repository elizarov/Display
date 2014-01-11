#include "LCDLog.h"
#include "BlinkLed.h"
#include "TimeoutLed.h"
#include "ActionButton.h"
#include "Parser.h"
#include "Tracker.h"
#include "DS18B20.h"

// ---------------- constant definitions ----------------

const unsigned long BAUD = 115200;

const uint8_t WIDTH = 16;
const uint8_t HEIGHT = 2;

const uint8_t RS_PIN = 2;
const uint8_t RW_PIN = 3;
const uint8_t EN_PIN = 4;
const uint8_t D4_PIN = 5;
const uint8_t D5_PIN = 6;
const uint8_t D6_PIN = 7;
const uint8_t D7_PIN = 8;
const uint8_t BL_PIN = 9;

const uint8_t BACKLIGHT = 128;

const uint8_t BLINK_LED_PIN = LED_BUILTIN;
const uint8_t RECEIVE_LED_PIN = 12;

const uint8_t LEFT_BUTTON_PIN = 10;
const uint8_t RIGHT_BUTTON_PIN = 11;

const unsigned int REGULAR_BLINK_INTERVAL = 1000; // 1 sec
const unsigned int URGENT_BLINK_INTERVAL = 200; // 200 ms
const unsigned int RECEIVE_LED_INTERVAL = 200; // 200 ms

const uint8_t LIGHT_SENSOR_PIN = A0;
const uint8_t DS18B20_PIN = A1;
const uint8_t SPEAKER_PIN = A2;

const unsigned long INITIAL_DUMP_INTERVAL = 2000L;   // 2 sec
const unsigned long PERIODIC_DUMP_INTERVAL = 60000L; // 1 min
const unsigned long PERIODIC_DUMP_SKEW = 5000L;      // 5 sec 

const unsigned int BOOT_TONE_FREQUENCY = 1000;
const unsigned long BOOT_TONE_DURATION = 300;

const unsigned int URGENT_TONE_FREQUENCY = 1500;
const unsigned long URGENT_TONE_DURATION = 300;

const unsigned int URGENT2_TONE_FREQUENCY = 2000;
const unsigned long URGENT2_TONE_DURATION = 1000;

// ---------------- object definitions ----------------

LCDLog lcd(RS_PIN, RW_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN, WIDTH, HEIGHT);
BlinkLed blinkLed(BLINK_LED_PIN);
TimeoutLed receiveLed(RECEIVE_LED_PIN);
ActionButton leftButton(LEFT_BUTTON_PIN);
ActionButton rightButton(RIGHT_BUTTON_PIN);
Parser parser(WIDTH);
Tracker tracker("0123456789PRUWBCDT", WIDTH);
DS18B20 ds18b20(DS18B20_PIN);

//------- DUMP STATE -------

const char HIGHLIGHT_CHAR = '*';

boolean firstDump = true; 
Timeout dump(INITIAL_DUMP_INTERVAL);
char dumpLine[] = "[D: +??.? l0000;u00000000]* ";

byte indexOf(byte start, char c) {
  for (byte i = start; dumpLine[i] != 0; i++)
    if (dumpLine[i] == c)
      return i;
  return 0;
}

#define POSITIONS0(P0,C2,POS,SIZE)                 \
        byte POS = P0;                             \
      	byte SIZE = indexOf(POS, C2) - POS;

#define POSITIONS(C1,C2,POS,SIZE)                  \
        POSITIONS0(indexOf(0, C1) + 1,C2,POS,SIZE)

byte highlightPos = indexOf(0, HIGHLIGHT_CHAR);
byte trackerEndPos = indexOf(0, ';');

POSITIONS0(indexOf(0, '+'), ' ', tempPos, tempSize)
POSITIONS('l', ';', lightPos, lightSize)
POSITIONS('u', ']', uptimePos, uptimeSize)

const unsigned long DAY_LENGTH_MS = 24 * 60 * 60000L;

unsigned long daystart = 0;
int updays = 0;

inline void prepareDecimal(int x, int pos, byte size, byte fmt = 0) {
  formatDecimal(x, &dumpLine[pos], size, fmt);
}

typedef FixNum<int, 1> temp1_t;

inline void prepareTemp1(DS18B20::temp_t x, int pos, int size) {
  temp1_t x1 = x;
  x1.format(&dumpLine[pos], size, temp1_t::SIGN);
}

const char DUMP_REGULAR = 0;
const char DUMP_FIRST = HIGHLIGHT_CHAR;

char* makeDump(char dumpType) {
  // prepare temperature
  DS18B20::temp_t temp = ds18b20.value();
  if (temp.valid())
    prepareTemp1(temp, tempPos, tempSize);
    
  // prepare light 
  prepareDecimal(analogRead(LIGHT_SENSOR_PIN), lightPos, lightSize);  
  
  // prepare uptime
  long time = millis();
  while ((time - daystart) > DAY_LENGTH_MS) {
    daystart += DAY_LENGTH_MS;
    updays++;
  }
  prepareDecimal(updays, uptimePos, uptimeSize - 6);
  time -= daystart;
  time /= 1000; // convert seconds
  prepareDecimal(time % 60, uptimePos + uptimeSize - 2, 2);
  time /= 60; // minutes
  prepareDecimal(time % 60, uptimePos + uptimeSize - 4, 2);
  time /= 60; // hours
  prepareDecimal((int) time, uptimePos + uptimeSize - 6, 2);

  // print
  if (dumpType == DUMP_REGULAR) {
    dumpLine[highlightPos] = 0;
  } else {
    byte i = highlightPos;
    dumpLine[i++] = dumpType;
    if (dumpType != HIGHLIGHT_CHAR)
      dumpLine[i++] = HIGHLIGHT_CHAR; // must end with highlight (signal) char
    dumpLine[i++] = 0; // and the very last char must be zero
  }
  dump.reset(PERIODIC_DUMP_INTERVAL + random(-PERIODIC_DUMP_SKEW, PERIODIC_DUMP_SKEW));
  firstDump = false;
  // print dump
  dumpLine[trackerEndPos] = ';';
  Serial.println(dumpLine);
  // submit dump to tracker and print to lcd
  dumpLine[trackerEndPos] = 0;
  return dumpLine + 1;
}

inline char* dumpState() {
  if (dump.check())
    return makeDump(firstDump ? DUMP_FIRST : DUMP_REGULAR);
  return 0;
}

// ---------------- MAIN CODE ----------------

void setup() {
  tone(SPEAKER_PIN, BOOT_TONE_FREQUENCY, BOOT_TONE_DURATION);
  Serial.begin(BAUD);
  analogWrite(BL_PIN, BACKLIGHT);
  lcd.println(F("Display init..."));
  lcd.print(F("DS18B20: "));
  ds18b20.setup();
  lcd.println(ds18b20.value().valid() ? F("OK") : F("FAIL"));
}

inline void processButtons() {
  if (leftButton.check())
    tracker.move(-1);
  if (rightButton.check())
    tracker.move(1);
}

inline void updateLCD() {
  char buf[WIDTH + 1];  
  // print 1st (message) line
  if (tracker.message(buf)) {
    lcd.setCursor(0, 0);
    lcd.print(buf);
    lcd.clearToRight();
  }
  // print 2nd (status) line
  if (tracker.status(buf)) {
    lcd.setCursor(0, 1);
    lcd.print(buf);
    lcd.clearToRight();
  }
}

void process(char* buf, uint8_t urgent) {
  if (!buf)
    return; // no message
  int8_t received = tracker.process(buf, urgent);
  if (received < 0)
    return; // not tracked message
  receiveLed.light(RECEIVE_LED_INTERVAL);
  if (urgent > 1)
    tone(SPEAKER_PIN, URGENT2_TONE_FREQUENCY, URGENT2_TONE_DURATION);
  else if (urgent > 0)  
    tone(SPEAKER_PIN, URGENT_TONE_FREQUENCY, URGENT_TONE_DURATION);
}

inline void parseSerialInput() {
  while (Serial.available()) {
    if (parser.parse(Serial.read()))
      process(parser.buffer(), parser.urgent());
  }
}

void loop() {
  processButtons();
  updateLCD();
  parseSerialInput();
  ds18b20.read();
  process(dumpState(), 0);
  blinkLed.blink(tracker.urgent() ? URGENT_BLINK_INTERVAL : REGULAR_BLINK_INTERVAL);
  receiveLed.check();
}
