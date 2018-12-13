#include <FastLED.h>
#include <stdarg.h>

#define LED_PIN  6

#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define BRIGHTNESS 255

// Breite und Höhe
#define kMatrixWidth 11 //----------.Breite
#define kMatrixHeight 17 //----------Höhe

//Button Variablen
#define buttonPin1 10//Switch Oben und Mitte 
#define buttonPin2 11 //Switch unten und Mitte
#define buttonPin3 9 //Rot unten
#define buttonPin4 8 // Rot oben

//Zeit Variablen
#define xTime 10 //---------------------------Intervall 1
#define onewayTime 42 //----------------------Intervall 2

//------------------------------------------------------

// Param for different pixel layouts
#define kMatrixSerpentineLayout true
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

void animation();
void debugPrint(const char* output, ...);
const uint16_t XY(const uint8_t& x, const uint8_t& y);

//--------------------------------SETUP ------------------------
void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  Serial.begin(9600);
  debugPrint("Hello World!");
}

//----------------------------------LOOP -------------------
void loop() 
{ 
  //Wenn Schalter oben ist, starte Animation.
  if (digitalRead(buttonPin1) == HIGH) {
    animation();
  } 
  //LEDs ausschalten, wenn der switch unten ist (Pinkontakt 2+3)
  else if (digitalRead(buttonPin2) == HIGH) {
    powerOff();
  }
}

//-------------------------------Animationsfunktion ------------
void animation() {
  static uint8_t xA;
  static uint8_t xB;
  static unsigned long xMillis;
  static long onewayMillis;
  static uint8_t hue;
  static uint8_t y;
  static uint8_t x;
  static uint8_t gegenstrecke = kMatrixHeight-1;
  debugPrint("x: %d   y: %d   gegenstrecke: %d", x, y, gegenstrecke);
  
  unsigned long currentMillis = millis();
  if (currentMillis - xMillis > xTime) {
    xMillis = currentMillis;
    ++x;
    if (x > 5) {
      x = 0;
      ++y;
      fadeToBlackBy(leds, NUM_LEDS, 20);
      if (y > kMatrixHeight-1) {
        y = 0;
      }
    }
    xA = 5 - x;
    xB = 5 + x;
  }
  if (currentMillis - onewayMillis > onewayTime) {
    onewayMillis = currentMillis;
    if (gegenstrecke == 0) {
      gegenstrecke = kMatrixHeight-1;
    } else {
      --gegenstrecke;
    }
  }
    
  ++hue;
  leds[ XY(xA, y)] = CHSV( hue, 255, 255 );
  leds[ XY(xB, y)] = CHSV( hue, 255, 255 );
  leds[ XY(5, gegenstrecke)] = CHSV( hue, 255, 255 );
  FastLED.show();
}
//----------------------------- Debug Funktion ----------------

void debugPrint(const char* output, ...) {
  va_list args;
  char buffer[1024];
  if (digitalRead(buttonPin1) && digitalRead(buttonPin2) && (sprintf (buffer, output, args) >= 0) ) {
    Serial.println(buffer);
  }
}

//----------------------------------Power Off -------------
void powerOff() {
  FastLED.clear();
  FastLED.show();
}

//------------------------ Matrix definitionen ----------
const uint16_t XY(const uint8_t& x, const uint8_t& y) {
  uint16_t returnValue = y * kMatrixWidth;
  
  if( kMatrixSerpentineLayout && y & 0x01) {
    // Odd rows run backwards
    returnValue += kMatrixWidth - 1 - x;
  } else {
    // Even rows run forwards
    returnValue += x;
  }
  return returnValue;
}

