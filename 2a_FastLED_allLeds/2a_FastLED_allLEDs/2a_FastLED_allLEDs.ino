#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>



#define LED_PIN     2
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define DELAY 30


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

//choose one of the following palettes below at currentPalette = xx
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
CRGBPalette16 myPalette = CRGBPalette16( CRGB::Black, CRGB::White, CRGB::Black);
CRGBPalette16 myPalette2 = bhw1_29_gp;

static uint8_t colorIndex = 0; // start out at 0



void setup() {
  Serial.begin(9600);
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    currentPalette = myPalette;
    currentBlending = LINEARBLEND;
    FastLED.clear ();
}


void loop()
{
    Serial.println("huhu");
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
    
    FastLED.show();
    FastLED.delay(DELAY);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
   /*
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 5;
    }
    */

    CRGB color = ColorFromPalette(currentPalette, colorIndex);
    
    // fill the entire strip with the current color
    fill_solid(leds, NUM_LEDS, color);

       // slowly increase the heat
    EVERY_N_SECONDS(1) {
      // stop incrementing at 255, we don't want to overflow back to 0
      if(colorIndex < 255) {
        colorIndex++;
      }
      if(colorIndex >= 255) {
        colorIndex = 0;
      }
    }

  
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Black
};


// Gradient palette "bhw1_29_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_29.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_29_gp ) {
    0,   1, 10, 19,
  132, 126,201,255,
  255,   1, 10, 19};
