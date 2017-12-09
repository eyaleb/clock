#include <FastLED.h>
#define HOUR_PIN    8 // time setting input
#define MINUTE_PIN  9 // time setting input
#define SECOND_PIN  10 // time setting input
#define LED_PIN     5
#define NUM_LEDS    180
#define BRIGHTNESS  16  // 64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
// MILLI_PER_SEC should be 1000, but smaller numbers make debugging easier/more interesting.
#define MILLIS_PER_SEC 1000
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 1
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
uint8_t hour, minute, second, change_hour, change_minute, change_second;
unsigned long lastupdate, now;
void setup() {
  int i;
    Serial.begin(9600);
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    pinMode(HOUR_PIN, INPUT);
    pinMode(MINUTE_PIN, INPUT);
    pinMode(SECOND_PIN, INPUT);
    change_hour = change_minute = change_second = 0;
    currentBlending = LINEARBLEND;
    hour = 9;
    minute = 0;
    second = 0;
    for (i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    lastupdate = millis() / MILLIS_PER_SEC;
}

void hexprint(uint8_t val) {
  if (val < 16) Serial.print('0');
  Serial.print(val,HEX);
}

void loop() {
    check_buttons();
    now = millis();
    now /= MILLIS_PER_SEC;
    if (now != lastupdate) {
      if (change_hour || change_minute || change_second) {
        if (change_hour) {
          hour++;
          change_hour = 0;
          Serial.print("Hour changed by button\n");
        }
        if (change_minute) {
          minute++;
          change_minute = 0;
          Serial.print("Minute changed by button\n");
        }
        if (change_second) {
          second = 0;
          change_second = 0;
          Serial.print("Second changed by button\n");
        }
      }
      second += now-lastupdate;
      lastupdate = now;
      if (second >= 60) {
        minute += second/60;
        second = second % 60;
        if (minute >= 60) {
         hour += minute / 60;
         minute %= 60;
         if (hour > 12) {
             hour -= 12;
          }
        }
      }
      display_time(hour, minute, second);
    }
    delay((unsigned long)(MILLIS_PER_SEC/2));
}

uint8_t canon(int index) {
  int return_val = index;
  if (index < 0) {
    return_val += NUM_LEDS;
  } else if (index >= NUM_LEDS) {
    return_val -= NUM_LEDS;
  }
  return return_val;
}

int pos (int led)
{
//  while (led < 0)
//    led += 60;
//  while (led >= 60)
//    led -= 60;
  return (NUM_LEDS-1) - led * (NUM_LEDS / 60);
}

void display_time(uint8_t hour, uint8_t minute, uint8_t second) {
    int i, hourled;
    for (i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }

    for (i = 0; i < 60; i += 5) { // hour markers
        leds[pos(i)].red   = 64;
        leds[pos(i)].green = 64;
        leds[pos(i)].blue  = 64;
    }
   
    hourled = hour * 5 + minute/12;
    leds[canon(pos(hourled)-1)].red = 255;
    leds[canon(pos(hourled)  )].red = 255;
    leds[canon(pos(hourled)+1)].red = 255;
    leds[pos(minute)].green = 255;
    leds[pos(second)].blue  = 255;
    
    FastLED.show();
}
void check_buttons() {
  return;
  if (digitalRead(HOUR_PIN) == HIGH) {
    Serial.print("Hour button HIGH\n");
    change_hour = 1;
  } else if (digitalRead(MINUTE_PIN) == HIGH) {
    Serial.print("Minute button HIGH\n");
    change_minute = 1;
  } else if (digitalRead(SECOND_PIN) == HIGH) {
    Serial.print("Second button HIGH\n");
    change_second = 1;
  }
}
 
