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
CRGBPalette16 myPalette = CRGBPalette16( CRGB::Black, CRGB::White);

static uint8_t heatIndex = 0; // start out at 0



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

    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 5;
    }

  
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Black
};
