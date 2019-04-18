#include <NeoPixelBrightnessBus.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
//comment for commit

//comment 2
#define STASSID "Toolbox"
#define STAPSK  ""

#define MATRIX_WIDTH 11
#define MATRIX_HEIGHT 17
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 30
#define LED_PIN D4
#define SWITCH_PIN D1
#define BUTTON_TOP_PIN D2
#define BUTTON_BOT_PIN D3

#define X_TIME 10
#define ONE_WAY_TIME 42

NeoPixelBrightnessBus<NeoBrgFeature, NeoEsp8266Uart1800KbpsMethod> strip(NUM_LEDS, LED_PIN);
NeoTopology <RowMajorAlternatingLayout> topo(MATRIX_WIDTH, MATRIX_HEIGHT);

WiFiServer wifiServer(5000);

// switchOn stores the last switch state, ledsOn the actual state.
// Opening/Closing via SpaceAPI overwrites the state until switch is toggled again.
bool switchOn;
bool ledsOn;

// Calculate animation by progress [0.1, 1.0)
typedef void (*AnimationFunction)(float);

typedef struct Animation {
  AnimationFunction callback;
  uint16_t durationMillis;
} Animation;

// Available animations
Animation animations[] = { Animation{&colorChangeAnimation, 2000}, Animation{ &animation1, 2000}};
int currentAnimation = 0;

unsigned long currentAnimationStart = millis();

void setup() {
  Serial.begin(9600);
  Serial.println("ToolboxTreppe: https://github.com/ToolboxBodensee/fastLED-Treppe");

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUTTON_TOP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BOT_PIN, INPUT_PULLUP);

  strip.Begin();
  strip.SetBrightness(BRIGHTNESS);
  strip.Show();

  Serial.println("WiFi connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    progressAnimation();
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  wifiServer.begin();

  ArduinoOTA.setHostname("treppeLed");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("ArduinoOTA initialized");
}

void loop() {
  if (switchOn != digitalRead(SWITCH_PIN)) {
    switchOn = digitalRead(SWITCH_PIN);
    ledsOn = switchOn;
  }
  if (ledsOn) {
    if ((millis() - currentAnimationStart) >= animations[currentAnimation].durationMillis) currentAnimationStart = millis();
    float progress = (float) (millis() - currentAnimationStart) / animations[currentAnimation].durationMillis;

    animations[currentAnimation].callback(progress);

  } else {
    strip.ClearTo(RgbColor(0));
    currentAnimationStart = millis();
  }
  strip.Show();

  if (!digitalRead(BUTTON_TOP_PIN)) {
    Serial.println("+1");
    currentAnimation = ++currentAnimation > sizeof(animations) / sizeof(Animation) - 1 ? sizeof(animations) / sizeof(Animation) - 1 : currentAnimation;
    Serial.println(currentAnimation);
    while (!digitalRead(BUTTON_TOP_PIN)) {
      delay(10);
    }
  }

  if (!digitalRead(BUTTON_BOT_PIN)) {
    Serial.println("-1");

    currentAnimation = --currentAnimation < 0 ? 0 : currentAnimation;
    Serial.println(currentAnimation);

    while (!digitalRead(BUTTON_BOT_PIN)) {
      delay(10);
    }
  }

  WiFiClient client = wifiServer.available();
  if (client) {
    Serial.println("TCP client connected");
    char receiveBuffer[24];
    int i = 0;
    while (client.connected()) {
      while (client.available() > 0 && i < sizeof(receiveBuffer)) {
        receiveBuffer[i] = client.read();
        Serial.print("Received Byte: 0x");
        Serial.println(receiveBuffer[i], HEX);
        i++;
      }
      client.stop();

      if (stringHasPrefix(receiveBuffer, "open")) {
        Serial.println("SpaceAPI notify, turning LEDs on");
        ledsOn = true;
      } else if (stringHasPrefix(receiveBuffer, "closed")) {
        Serial.println("SpaceAPI notify, turning LEDs off");
        ledsOn = false;
      }
    }
  }

  ArduinoOTA.handle();
}

void colorChangeAnimation(float progress) {
  static int steps = 10;
  static HslColor currentColor = HslColor(0, 1, 0.5);
  static HslColor nextColor = HslColor(1.0 / steps, 1, 0.5);
  static float prevProgress = 0;



  // Detect progress overflow
  if (progress < prevProgress) {

    currentColor = nextColor;
    float nextH = (nextColor.H + (1.0 / steps));
    if (nextH > 1) {
      nextH--;
    }
    nextColor.H = nextH;
  }
  prevProgress = progress;

  float p = NeoEase::ExponentialInOut(progress);

  RgbColor color = RgbColor::LinearBlend(currentColor, nextColor, p);
  strip.ClearTo(color);
}

void animation1(float progress) {
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

void progressAnimation() {
  static int x;
  static int y;
  static unsigned long lastChange;
  static int state;
  if (millis() - lastChange > 100) {
    switch (state) {
      case 0:
        if (x < MATRIX_WIDTH - 1) {
          x++;
        } else {
          state = 1;
        }
        break;
      case 1:
        if (y < MATRIX_HEIGHT - 1) {
          y++;
        } else {
          state = 2;
        }
        break;
      case 2:
        if (x > 0) {
          x--;
        } else {
          state = 3;
        }
        break;
      case 3:
        if (y > 0) {
          y--;
        } else {
          state = 0;
        }
        break;
    }
    lastChange = millis();
  }
  strip.ClearTo(RgbColor(0));
  strip.SetPixelColor(topo.Map(x, y), RgbColor(255, 255, 255));
  strip.Show();
}

bool stringHasPrefix(char *s, char* prefix) {
  while (*prefix != 0) {
    if (*s == 0 || *s != *prefix) {
      return false;
    }
    s++;
    prefix++;
  }
}
