#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    50
#define BRIGHTNESS  100
#define DELAY       25

CRGB leds[NUM_LEDS];
static const uint8_t interval = 0;

DEFINE_GRADIENT_PALETTE( ib_jul01_gp ) {
    0,   0,  0,  0,
   64,   111,111,  0,
  128,   0,  0,  0,
  172,   0,200, 200,
  255,   0,  0,  0
};

CRGBPalette16 gTargetPalette( ib_jul01_gp );


void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    //.setCorrection(TypicalLEDStrip) // cpt-city palettes have different color balance
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
  sunrise(); 
  FastLED.show();
  FastLED.delay(DELAY);
}

void sunrise() {
  // current gradient palette color index
  static uint8_t heatIndex = 0; // start out at 0

  CRGB color = ColorFromPalette(gTargetPalette, heatIndex);

  // fill the entire strip with the current color
  fill_solid(leds, NUM_LEDS, color);

  // slowly increase the heat
  EVERY_N_SECONDS(interval) {
    // stop incrementing at 255, we don't want to overflow back to 0
    if(heatIndex < 255) {
      heatIndex++;
    }
    if(heatIndex >= 255) {
      heatIndex = 0;
    }
  }
}
