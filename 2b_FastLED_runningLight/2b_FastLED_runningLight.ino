#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    25
#define BRIGHTNESS  100
#define DELAY       25

CRGB leds[NUM_LEDS];
static const uint8_t interval = 0;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;



/****************************************************************
 * b. Config Colors with color presets, see bottom of code
 ****************************************************************/
extern const TProgmemPalette16 testColors_b PROGMEM;


/****************************************************************
 * c. Config Colors with color presets (fast method)
 ****************************************************************/
 
CRGBPalette16 testColors_c = CRGBPalette16( CRGB::Black, CRGB::White, CRGB::Black);


void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    //.setCorrection(TypicalLEDStrip) // cpt-city palettes have different color balance
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);


  /*********************************************************************
   * choose color preset a, b or c as currentPalette
   *********************************************************************/
  
  currentPalette = testColors_b;  // choose for b
  // currentPalette = testColors_c;  // choose for c


  currentBlending = LINEARBLEND;
  FastLED.clear ();
}


void loop()
{
  Serial.println("hey");
  static uint8_t colorIndex = 0; // start out at 0
  colorIndex = colorIndex + 1; /* motion speed */
  
  animLight(colorIndex); 
  FastLED.show();
  FastLED.delay(DELAY);
}

void animLight(uint8_t colorIndex) {
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    colorIndex += 10;
  }
}



/******************************************* 
 * b. Config Colors with color presets  
 *******************************************/

const TProgmemPalette16 testColors_b PROGMEM =
{
    CRGB::Red,
    CRGB::Orange,
    CRGB::Black
};
