/*---- LEDs Strip ----*/
#include <FastLED.h>
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define COLOR_ORDER GRB
// Define a type of LED strips
#define LED_TYPE    WS2811
// Define digital pins that LED strips are connected to:
#define DATA_PIN_PLANT    4
#define DATA_PIN_PHONE_1    5
#define DATA_PIN_PHONE_2    6
// Define arrays for each LED strip"
const int NUM_LEDS_LONG = 31;
const int NUM_LEDS_SHORT = 11;
CRGB plant_led[NUM_LEDS_LONG];
CRGB phone_1[NUM_LEDS_SHORT];
CRGB phone_2[NUM_LEDS_SHORT];
// Define variable to increment luminosity and change hue
int intensity = 50;
int hue = 100;


/*---- IR Sensors ----*/
const int IR_PHONE_1 = 7;
const int IR_PHONE_2 = 8;

/*---- Time Control ----*/
unsigned long prevMs = 0;
int delayIncreaseIntensity = 2000;
int delayChangeHue = 4000;


void graduallyIncreaseIntensity() {
  unsigned long nowMs = millis();
  if (nowMs > prevMs + delayIncreaseIntensity) {
    if (intensity < 255) {
      intensity++;
      prevMs = nowMs;
    }
  }
  FastLED.show();
}

void lightJustPhoneSlot(int phone_one, int phone_two) {
  for (int i = 2 ; i < 9; i++) {
    phone_1[i] = CHSV(100, 255, phone_one);
    phone_2[i] = CHSV(100, 255, phone_two);
  }
  for (int i = 0; i < 31; i++) {
    plant_led[i] = CHSV(100, 255, 0);
  }
  FastLED.show();
}

void setLedsColor(int newHue = 100) {
  for (int i = 0; i < 31; i++) {
    plant_led[i] = CHSV(newHue, 255, intensity);
  }
  for (int i = 2 ; i < 9; i++) {
    phone_1[i] = CHSV(newHue, 255, intensity);
    phone_2[i] = CHSV(newHue, 255, intensity);
  }
}

void changeColorOnMaxIntensity() {
  unsigned long nowMs = millis();
  if (nowMs > prevMs + delayChangeHue) {
    if (intensity == 255) {
      // Reset hue to 1 if it reaches 255
      hue = hue == 255 ? 1 : hue;
      setLedsColor(hue);
      prevMs = nowMs;
    }
  }
  FastLED.show();
}

void setup() {
  // LEDs
  FastLED.addLeds<LED_TYPE, DATA_PIN_PLANT, COLOR_ORDER>(plant_led, NUM_LEDS_LONG).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN_PHONE_1, COLOR_ORDER>(phone_1, NUM_LEDS_SHORT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN_PHONE_2, COLOR_ORDER>(phone_2, NUM_LEDS_SHORT).setCorrection(TypicalLEDStrip);
  // 3 seconds delay for recovery
  delay(3000);

  // IR Sensors
  pinMode(IR_PHONE_1, INPUT);
  pinMode(IR_PHONE_2, INPUT);
}

void loop() {
  if (!digitalRead(IR_PHONE_1) && !digitalRead(IR_PHONE_2)) {
    setLedsColor();
    graduallyIncreaseIntensity();
    changeColorOnMaxIntensity();
  } else if (!digitalRead(IR_PHONE_1) && digitalRead(IR_PHONE_2)) {
    lightJustPhoneSlot(255, 0);
  } else if (digitalRead(IR_PHONE_1) && !digitalRead(IR_PHONE_2)) {
    lightJustPhoneSlot(0, 255);
  } else {
    intensity = 0;
    setLedsColor();
    FastLED.show();
  }
}
