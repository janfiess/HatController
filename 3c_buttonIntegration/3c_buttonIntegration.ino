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
#include <EEPROM.h>

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

/* EEPROM Start */

int eeprom_address = 0;
byte eeprom_value;
String eeprom_value_string;

/* EEPROM end */


/*custom variables */
long lastTimestamp = 0;
boolean doAnimateAllLightsEqually = true;
boolean doFadeToColor = false;
boolean doAnimateRunningLight = false;
int color_r, color_g, color_b, animSpeed;



/***********************************
 * Button
 ***********************************/

const int buttonPin = 14;
int buttonState = 0;
int prev_buttonState = 0;
int button_counter = 0;
long button_timestamp = 0;
boolean btn_fourClicks_setId = false;
 








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
   * choose color preset as currentPalette
   *********************************************************************/
  
  currentPalette = testColors_c;


  currentBlending = LINEARBLEND;
  FastLED.clear ();

  /* FastLed setup end */


  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  /* EEPROM Start */
  
  EEPROM.begin(512);
  eeprom_value = EEPROM.read(eeprom_address);
  eeprom_value_string += String(eeprom_value);

  /* EEPROM end */

  pinMode(buttonPin, INPUT);
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

  if (strcmp(topic,"janfiess/anim")==0){
    Serial.println("Topic janfiess/anim");
    if ((char)payload[1] == '1') lightAnim1();
    else if ((char)payload[1] == '2') lightAnim2();
    else if ((char)payload[1] == '3') runningLight1();
  }

  if (strcmp(topic,"janfiess/static")==0){
    Serial.println("Topic janfiess/static");
    if ((char)payload[1] == '1') toBlack();
    else if ((char)payload[1] == '2') toYellow();
    else if ((char)payload[1] == '3') toGreen();
  }

  if (strcmp(topic,"janfiess/setId")==0){
    String receivedString = "";
    for (int i = 0; i < length; i++) {
      receivedString += (char)payload[i];
    }

    String board_id = receivedString.substring(1);
    // Serial.println(board_id);
    setId(board_id);
  }

  /* Topic is the id of the device */
  String id_topic_string = "janfiess/" + eeprom_value_string;
  int str_len = id_topic_string.length() + 1;
  char id_topic_char_array[str_len];
  id_topic_string.toCharArray(id_topic_char_array, str_len);
  
  if (strcmp(topic, id_topic_char_array)==0){
    Serial.println("Topic: " + id_topic_string);
    client.publish("janfiess/msg", id_topic_char_array);
    
    
    if ((char)payload[1] == '1') {
      // do sth
    }
  }
  
  
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

void getId(){
  
  eeprom_value = EEPROM.read(eeprom_address);

  //Serial.println("getId");
  //Serial.print(eeprom_address);
  //Serial.print("\t");
  //Serial.print(eeprom_value, DEC);
  //Serial.println();

  String eeprom_value_string_msg = "Requested device id: ";  
  eeprom_value_string_msg += String(eeprom_value);
  Serial.println(eeprom_value_string_msg);
  int str_len = eeprom_value_string_msg.length() + 1;
  char eeprom_value_char_array[str_len];
  eeprom_value_string_msg.toCharArray(eeprom_value_char_array, str_len);

  client.publish("janfiess/msg", eeprom_value_char_array);
  
  
}

void setId(String boardId_string){
  // if button pressed four times
  if(btn_fourClicks_setId == false) return;

  /* 
   * Unsubscribe from the topic named by the old ID
   */
   
  String old_id_topic_string = "janfiess/" + eeprom_value_string;
  int old_str_len = old_id_topic_string.length() + 1;
  char old_id_topic_char_array[old_str_len];
  old_id_topic_string.toCharArray(old_id_topic_char_array, old_str_len);
  
  client.unsubscribe(old_id_topic_char_array);
  
  /* 
   * save the new device id in EEPROM 
   */
   
  byte boardId = boardId_string.toInt();
  // Serial.println("setId");
  
  EEPROM.write(eeprom_address, boardId);
  EEPROM.commit();
  eeprom_value_string = String(boardId);

  /*
   * Publish new device ID
   * Subscribe to athe topic named by the new ID
   */

  String id_topic_string = "janfiess/" + eeprom_value_string;
  int str_len = id_topic_string.length() + 1;
  char id_topic_char_array[str_len];
  id_topic_string.toCharArray(id_topic_char_array, str_len);

  String id_payload_string = "Set device id: " + eeprom_value_string;
  int payloadstr_len = id_payload_string.length() + 1;
  char id_payload_char_array[payloadstr_len];
  id_payload_string.toCharArray(id_payload_char_array, payloadstr_len);
  
  client.publish("janfiess/msg", id_payload_char_array);
  Serial.println("Set device id: " + eeprom_value_string);
  btn_fourClicks_setId = false;
  
  
  client.subscribe(id_topic_char_array); // subscribe to device id (standard: 102)

/*
  reconnect();
  */
  
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("janfiess/test", "hello world");
      // ... and resubscribe
      client.subscribe("janfiess/anim");
      client.subscribe("janfiess/static");
      client.subscribe("janfiess/setId");
      // eeprom_value_string

      String id_topic_string = "janfiess/" + eeprom_value_string;
      int str_len = id_topic_string.length() + 1;
      char id_topic_char_array[str_len];
      id_topic_string.toCharArray(id_topic_char_array, str_len);
      client.subscribe(id_topic_char_array); // subscribe to device id (standard: 102)
    
    
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

  /***************************************
   * Button
   ***************************************/

  buttonState = digitalRead(buttonPin);

  if(btn_fourClicks_setId == true){
    if((millis() - button_timestamp) >= 20000){
      
      btn_fourClicks_setId = false; 
      Serial.println("Time to set id has expired");
      client.publish("janfiess/msg", "Time to set id has expired");
    }
  }
  

  if(buttonState == prev_buttonState) return;
  prev_buttonState = buttonState;
  if(buttonState == 1){

    Serial.println("btn pressed");
    if((millis() - button_timestamp) < 700){
      button_counter++;
      button_timestamp = millis();
      Serial.print("Button counter: ");
      Serial.println(button_counter);
    }
    else if((millis() - button_timestamp) >= 700){
      button_counter = 1;
      button_timestamp = millis();
      btn_fourClicks_setId = false;
    }
   
    if(button_counter == 2){
      lightAnim1(); 
    }
    else if(button_counter == 3){
      getId();
       
    }
    else if(button_counter == 4){
      btn_fourClicks_setId = true;
      client.publish("janfiess/msg", "Set new device id: -t janfiess/setId -m \"m113\"");  
    }
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
