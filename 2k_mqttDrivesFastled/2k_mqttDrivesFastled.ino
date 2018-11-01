/****************************************************************************
 * 
 * HatController
 * Jan Fiess, October 2018
 * 
 * sources 
 * - the example of the esp8266 example of the library PubSubClient,
 * - FastLED(http://fastled.io/docs/3.1/files.html)
 * - Fade towards color: see https://forum.arduino.cc/index.php?topic=569388.0 @ Complete original code
 * 
 ****************************************************************************/






#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "dreammakers";
const char* password = "dreammakers";
const char* mqtt_server = "192.168.0.68";


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

CRGBPalette16 currentPalette;
TBlendType    currentBlending;



/****************************************************************
 * Config Colors with color presets, see bottom of code
 ****************************************************************/
extern const TProgmemPalette16 testColors_b PROGMEM;


/****************************************************************
 * Config Colors with color presets (fast method)
 ****************************************************************/
 
CRGBPalette16 testColors_c = CRGBPalette16( CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black);






static uint8_t colorIndex = 0; // start out at 0

/* FastLED end */


/*custom variables */
long lastTimestamp = 0;
boolean doAnimateAllLightsEqually = true;
boolean doFadeToColor = false;
boolean doAnimateRunningLight = false;
int color_r, color_g, color_b, animSpeed;









void setup() {
  delay(3000); // 3 second delay for recovery
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  // Serial.println("huhu");
  

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
  
  currentPalette = testColors_b;


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
  else if ((char)payload[1] == '6') runningLight1();
}



void lightAnim1(){
  doFadeToColor = false;
  doAnimateRunningLight = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_b;
  Serial.println("lightAnim1");
}

void lightAnim2(){
  doFadeToColor = false;
  doAnimateRunningLight = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_c;
  Serial.println("lightAnim2");
} 

void toGreen(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 0 * BRIGHTNESS /255;
  color_g = 35 * BRIGHTNESS /255;
  color_b = 5 * BRIGHTNESS /255;
  animSpeed = 1;
  Serial.println("toGreen");
}

void toYellow(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 155 * BRIGHTNESS /255;
  color_g = 155 * BRIGHTNESS /255;
  color_b = 0 * BRIGHTNESS /255;
  animSpeed = 5;
  Serial.println("toYellow");
}

void toBlack(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 0;
  color_g = 0;
  color_b = 0;
  animSpeed = 10;
  Serial.println("toBlack");
}

void runningLight1(){
  doFadeToColor = false;
  doAnimateRunningLight = true;
  doAnimateAllLightsEqually = false;
  colorIndex = 0;
  currentPalette = testColors_b;
  Serial.println("runningLight1");
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

  colorIndex = colorIndex + 1; /* motion speed */
  
  if(doAnimateAllLightsEqually) animAllLightsEqually(colorIndex);
  if(doFadeToColor) fadeToColor();
  if(doAnimateRunningLight) animateRunningLight(colorIndex); 
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

void animAllLightsEqually(uint8_t colorIndex) {
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
  }
}

void animateRunningLight(uint8_t colorIndex){
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    colorIndex += 10; // lower number, wider running light, 0 equals fill_solid(leds, NUM_LEDS, color);  
  }
}

/* FastLED end */


/* FastLED start */


/******************************************* 
 * Config Colors with color presets  
 *******************************************/

const TProgmemPalette16 testColors_b PROGMEM =
{
    CRGB::White,
    CRGB::Black
};

/* FastLED end */


/*
 * Fade towards color start
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
