/****************************************************************************
 * 
 * sources 
 * - the example of the esp8266 example of the library PubSubClient,
 * - FastLED(http://fastled.io/docs/3.1/files.html)
 * - Palette Knife for FastLED (https://learn.adafruit.com/twinkling-led-parasol/assembly)
 * 
 ****************************************************************************/






#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Difix";
const char* password = "Difix88288828";
const char* mqtt_server = "192.168.0.10";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

uint8_t wait = 67;


/* FastLED start */

// insert FastLED code

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    25
#define BRIGHTNESS  100.0f
#define DELAY       25

CRGB leds[NUM_LEDS];
static const uint8_t interval = 0;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;


/****************************************************************
 * a. Config Colors with free RGB parameters, see bottom of code
 ****************************************************************/
 
// extern const TProgmemRGBGradientPalettePtr testColors_a_palette PROGMEM;
// CRGBPalette16 testColors_a( testColors_a_palette );


/****************************************************************
 * b. Config Colors with color presets, see bottom of code
 ****************************************************************/
extern const TProgmemPalette16 testColors_b PROGMEM;


/****************************************************************
 * c. Config Colors with color presets (fast method)
 ****************************************************************/
 
CRGBPalette16 testColors_c = CRGBPalette16( CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black);






static uint8_t colorIndex = 0; // start out at 0

/* FastLED end */


/*custom variables */
long lastTimestamp = 0;
boolean doAnimateAllLightsEqually = true;
boolean doFadeToColor = false;
int color_r, color_g, color_b, animSpeed;









void setup() {
  delay(3000); // 3 second delay for recovery
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println("huhu");
  

  /* FastLed setup start */


  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    //.setCorrection(TypicalLEDStrip) // cpt-city palettes have different color balance
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  /*********************************************************************
   * choose color preset a, b or c as currentPalette
   *********************************************************************/
  
  // currentPalette = testColors_a;  // choose for a
  currentPalette = testColors_b;  // choose for b
  // currentPalette = testColors_c;  // choose for c


  currentBlending = LINEARBLEND;
  FastLED.clear ();

  /* FastLed setup end */


  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
  if ((char)payload[1] == '1') lightAnim1();
  else if ((char)payload[1] == '2') lightAnim2();
  else if ((char)payload[1] == '3') toGreen();
  else if ((char)payload[1] == '4') toYellow();
  else if ((char)payload[1] == '5') toBlack();
}



void lightAnim1(){
  doFadeToColor = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_b;
  Serial.println("lightAnim1");
}

void lightAnim2(){
  doFadeToColor = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_c;
  Serial.println("lightAnim2");
} 

void toGreen(){
  doAnimateAllLightsEqually = false;
  doFadeToColor = true;
  color_r = 0 * BRIGHTNESS /255;
  color_g = 35 * BRIGHTNESS /255;
  color_b = 5 * BRIGHTNESS /255;
  animSpeed = 1;
  Serial.println("toGreen");
}

void toYellow(){
  doAnimateAllLightsEqually = false;
  doFadeToColor = true;
  color_r = 155 * BRIGHTNESS /255;
  color_g = 155 * BRIGHTNESS /255;
  color_b = 0 * BRIGHTNESS /255;
  animSpeed = 5;
  Serial.println("toYellow");
}

void toBlack(){
  doAnimateAllLightsEqually = false;
  doFadeToColor = true;
  color_r = 0;
  color_g = 0;
  color_b = 0;
  animSpeed = 10;
  Serial.println("toBlack");
}




void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("janfiess/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  /* Print Heap Memory in console */
  if((millis() - lastTimestamp) >= 10000){
    Serial.println(millis());
    Serial.printf("loop heap size: %u\n", ESP.getFreeHeap());
    lastTimestamp = millis();
  }

  
  /* FastLED start */

  // insert FastLED code

  // Serial.println("hey");
  if(doAnimateAllLightsEqually) animAllLightsEqually();
  if(doFadeToColor) fadeToColor(); 
  FastLED.show();
  FastLED.delay(DELAY);
  
  /* FastLED end */


  
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    // Serial.print("Publish message: ");
    // Serial.println(msg);
    // client.publish("outTopic", msg);
  } 
}


/* FastLED start */

void animAllLightsEqually() {
  
  CRGB color = ColorFromPalette(currentPalette, colorIndex, BRIGHTNESS, currentBlending);

  // fill the entire strip with the current color
  fill_solid(leds, NUM_LEDS, color);

  // slowly increase the heat
  EVERY_N_SECONDS(interval) {
    // stop incrementing at 255, we don't want to overflow back to 0
    if(colorIndex < 255) {
      colorIndex++;
    }
    if(colorIndex >= 255) {
      colorIndex = 0;
    }
  }
}

/* FastLED end */


/* FastLED start */

/******************************************* 
 * a. Config Colors with free RGB parameters  
 *******************************************/

/*
DEFINE_GRADIENT_PALETTE( testColors_rgb ) {
    0,     0,   0,   0,
   64,    90, 200,   0,
  128,     0,   0,   0,
  172,   222,   0, 200,
  255,     0,   0,   0};

const TProgmemRGBGradientPalettePtr testColors_a_palette = testColors_rgb;
*/

/******************************************* 
 * b. Config Colors with color presets  
 *******************************************/

const TProgmemPalette16 testColors_b PROGMEM =
{
    CRGB::Red,
    CRGB::Black
};

/* FastLED end */


/*
 * Fade towards color start 
 * cp.  https://forum.arduino.cc/index.php?topic=569388.0 @ Complete original code
 */

void fadeToColor(){
  CRGB bgColor( color_r, color_g, color_b);
  // fade all existing pixels toward bgColor by "1" (out of 255) -> 1 is slow, 255 is fast
  fadeTowardColor( leds, NUM_LEDS, bgColor, animSpeed);
}

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}

/*Fade towards color end */
