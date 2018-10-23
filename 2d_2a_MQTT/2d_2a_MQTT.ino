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
#define NUM_LEDS    50
#define BRIGHTNESS  100
#define DELAY       25

CRGB leds[NUM_LEDS];
static const uint8_t interval = 0;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;


/****************************************************************
 * a. Config Colors with free RGB parameters, see bottom of code
 ****************************************************************/
 
extern const TProgmemRGBGradientPalettePtr testColors_a_palette PROGMEM;
CRGBPalette16 testColors_a( testColors_a_palette );


/****************************************************************
 * b. Config Colors with color presets, see bottom of code
 ****************************************************************/
extern const TProgmemPalette16 testColors_b PROGMEM;


/****************************************************************
 * c. Config Colors with color presets (fast method)
 ****************************************************************/
 
CRGBPalette16 testColors_c = CRGBPalette16( CRGB::Black, CRGB::White, CRGB::Black);


/* FastLED end */





void setup() {

  

  /* FastLed setup start */

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
  
  currentPalette = testColors_a;  // choose for a
  // currentPalette = testColors_b;  // choose for b
  // currentPalette = testColors_c;  // choose for c


  currentBlending = LINEARBLEND;
  FastLED.clear ();

  

  /* FastLed setup end */




  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
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

  if ((char)payload[1] == '1') white();
  else if ((char)payload[1] == '2') turquoise();
  else if ((char)payload[1] == '3') black();

}

void white(){
  currentPalette = testColors_a;
  Serial.println("A");
}

void turquoise(){
  currentPalette = testColors_b;
  Serial.println("B");
}

void black(){
  currentPalette = testColors_c;
  Serial.println("C");
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

  
  /* FastLED start */

  // insert FastLED code

  // Serial.println("hey");
  animLight(); 
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

void animLight() {
  static uint8_t colorIndex = 0; // start out at 0
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

DEFINE_GRADIENT_PALETTE( testColors_rgb ) {
    0,     0,   0,   0,
   64,    90, 200,   0,
  128,     0,   0,   0,
  172,   222,   0, 200,
  255,     0,   0,   0};

const TProgmemRGBGradientPalettePtr testColors_a_palette = testColors_rgb;


/******************************************* 
 * b. Config Colors with color presets  
 *******************************************/

const TProgmemPalette16 testColors_b PROGMEM =
{
    CRGB::Red,
    CRGB::Black
};

/* FastLED end */
