#include <NeoPixelBrightnessBus.h>

#define MATRIX_WIDTH 11
#define MATRIX_HEIGHT 17
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 20
#define LED_PIN D4

#define X_TIME 10
#define ONE_WAY_TIME 42

NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(NUM_LEDS, LED_PIN);
NeoTopology <RowMajorAlternatingLayout> topo(MATRIX_WIDTH, MATRIX_HEIGHT);

typedef void (*Animation)();

// Available animations
Animation animations[] = {&animation1};

void setup() {
  Serial.begin(9600);
  strip.Begin();
  strip.SetBrightness(BRIGHTNESS);
  strip.Show();
}

void loop() {
  Serial.println("Loop");
  (*animations[0])();
}

void animation1() {
  static uint8_t xA;
  static uint8_t xB;
  static unsigned long xMillis;
  static long onewayMillis;
  static uint8_t hue;
  static uint8_t y;
  static uint8_t x;
  static uint8_t gegenstrecke = MATRIX_HEIGHT - 1;

  unsigned long currentMillis = millis();
  if (currentMillis - xMillis > X_TIME) {
    xMillis = currentMillis;
    ++x;
    if (x > 5) {
      x = 0;
      ++y;
      //fadeToBlackBy(leds, NUM_LEDS, 20);
      for (uint16_t i = 0; i < NUM_LEDS; i++) {
        HsbColor c = strip.GetPixelColor(i);
        strip.SetPixelColor(i, HsbColor(c.H, c.S, c.B - c.B * (20.0 / 256.0)));
      }
      if (y > MATRIX_HEIGHT - 1) {
        y = 0;
      }
    }
    xA = 5 - x;
    xB = 5 + x;
  }
  if (currentMillis - onewayMillis > ONE_WAY_TIME) {
    onewayMillis = currentMillis;
    if (gegenstrecke == 0) {
      gegenstrecke = MATRIX_HEIGHT - 1;
    } else {
      --gegenstrecke;
    }
  }

  ++hue;
  strip.SetPixelColor(topo.Map(xA, y), HsbColor(hue / 256.0, 1.0, 1.0) );
  strip.SetPixelColor(topo.Map(xB, y), HsbColor(hue / 256.0, 1.0, 1.0) );
  strip.SetPixelColor(topo.Map(5, gegenstrecke), HsbColor(hue / 256, 1.0, 1.0) );
  strip.Show();
}
