/****************************************************************************
 * 
 * HatController
 * Jan Fiess, Fall 2018
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

/* FastLED start */

// insert FastLED code

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

/*
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1."
#endif
*/

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
extern const TProgmemPalette16 testColors_c PROGMEM;

 
/*CRGBPalette16 testColors_c = CRGBPalette16( CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black); */


static uint8_t colorIndex = 0; // start out at 0

/* FastLED end */

/* EEPROM Start */

String eeprom_deviceId_string;
int msgCount = 0;

/* EEPROM end */


/*custom variables */
int lastTimestamp = 0;
boolean doAnimateAllLightsEqually = false;
boolean doFadeToColor = true;
boolean doAnimateRunningLight = false;
int color_r, color_g, color_b, animSpeed;
int wifiSucks_counter = 0;
int lastTimestamp_wifi = 0;
int str_len; // for creating char arrays
char prefix[]= "hat/";

short initTime = 0;



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
  delay(2000); // 3 second delay for recovery
  // pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);


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
  
  //currentPalette = testColors_b;
  currentBlending = LINEARBLEND;
  FastLED.clear ();

  /* EEPROM Start */
  
  EEPROM.begin(512);
  eeprom_deviceId_string = String(EEPROM.read(0));
  Serial.println("id: " + eeprom_deviceId_string);

  


  /* EEPROM end */

  pinMode(buttonPin, INPUT);

  // client.publish("hat/msg", "_________restarted");

  /* FastLed setup end */

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  initTime = millis();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiSucks_counter++;
    if(wifiSucks_counter > 10){
      wifiSucks_counter = 0;
      ESP.restart();
    }  
  }

  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP addr: "));
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("["));
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if (strcmp(topic,"hat/all")==0){

    if(msgCount < 2){ // execute only the latest retained message on start 
      if(EEPROM.read(1) >= 1){
        triggerAnims((char)payload[1]);
        EEPROM.write(1, 1); // (EEPROM Speicherstelle -> 1,    broadcast -> 1, unicast -> 0)
        EEPROM.commit();
      }
    } else if(msgCount >= 2){
      triggerAnims((char)payload[1]);
      EEPROM.write(1, 1); // (EEPROM Speicherstelle -> 1,    broadcast -> 1, unicast -> 0)
      EEPROM.commit();
    }   
    msgCount++;
  }




  /* Topic is the id of the device: "hat/[deviceId]" */
  String id_topic_string = prefix + eeprom_deviceId_string;

  str_len = id_topic_string.length() + 1;
  char id_topic_char_array[str_len];
  id_topic_string.toCharArray(id_topic_char_array, str_len);
  
  if (strcmp(topic, id_topic_char_array)==0){

    if(msgCount < 2){ // execute as first cmd only the latest retained message on start, not both of them
      if(EEPROM.read(1) == 0){
        triggerAnims((char)payload[1]);
        EEPROM.write(1, 0); // (EEPROM Speicherstelle -> 1,    broadcast -> 1, unicast -> 0)
        EEPROM.commit();
      }
    } else if(msgCount >= 2){
      triggerAnims((char)payload[1]);
      EEPROM.write(1, 0); // (EEPROM Speicherstelle -> 1,    broadcast -> 1, unicast -> 0)
      EEPROM.commit();
    } 
    msgCount++;
  }

  if (strcmp(topic,"hat/setId")==0){
    String receivedString = "";
    for (int i = 0; i < length; i++) {
      receivedString += (char)payload[i];
    }

    String board_id = receivedString.substring(1);
    // Serial.println(board_id);
    setId(board_id);
  }
}

void triggerAnims(char payload){
  if      (payload == '1') toBlack();
  else if (payload == '2') toWhite();
  else if (payload == '3') runningLight1();
  else if (payload == '4') lightAnim1();
  else if (payload == '5') toOrange();
}



void lightAnim1(){
  doFadeToColor = false;
  doAnimateRunningLight = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_b;
}

void lightAnim2(){
  doFadeToColor = false;
  doAnimateRunningLight = false;
  doAnimateAllLightsEqually = true;
  colorIndex = 0;
  currentPalette = testColors_c;
} 

void toOrange(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 60 * BRIGHTNESS /255;
  color_g = 35 * BRIGHTNESS /255;
  color_b = 0 * BRIGHTNESS /255;
  animSpeed = 1;
}

void toWhite(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 155 * BRIGHTNESS /255;
  color_g = 155 * BRIGHTNESS /255;
  color_b = 155 * BRIGHTNESS /255;
  animSpeed = 5;
}

void toBlack(){
  doAnimateAllLightsEqually = false;
  doAnimateRunningLight = false;
  doFadeToColor = true;
  color_r = 0;
  color_g = 0;
  color_b = 0;
  animSpeed = 10;;
}

void runningLight1(){
  doFadeToColor = false;
  doAnimateRunningLight = true;
  doAnimateAllLightsEqually = false;
  colorIndex = 0;
  currentPalette = testColors_b;
  // Serial.println("runningLight1");
} 

void getId(){
  String eeprom_deviceId_string = "Device id: ";  
  eeprom_deviceId_string += String(EEPROM.read(0));
  Serial.println(eeprom_deviceId_string);

  str_len = eeprom_deviceId_string.length() + 1;
  char eeprom_deviceId_charArray[str_len];
  eeprom_deviceId_string.toCharArray(eeprom_deviceId_charArray, str_len);

  client.publish("hat/getId", eeprom_deviceId_charArray);
  eeprom_deviceId_charArray[0] = 0;
}

void setId(String boardId_string){
  
  // if button pressed four times
  if(btn_fourClicks_setId == false) return;

  /* 
   * Unsubscribe from "hat/[oldDeviceId]
   */
   
  String old_topic_id_static_string = prefix + eeprom_deviceId_string;
  str_len = old_topic_id_static_string.length() + 1;
  char old_topic_id_static_charArray[str_len];
  old_topic_id_static_string.toCharArray(old_topic_id_static_charArray, str_len);
  
  client.unsubscribe(old_topic_id_static_charArray);
  old_topic_id_static_charArray[0] = 0;


  
  /* 
   * save the new device id in EEPROM 
   */


   // Serial.println(boardId_string);
   
  byte boardId = boardId_string.toInt();
  // Serial.println("setId");
  
  EEPROM.write(0, boardId);
  EEPROM.commit();
  eeprom_deviceId_string = String(boardId);

  /*
   * Publish new device ID
   * Subscribe to the topic named by the new ID
   */

  String newDeviceId_string = "New ID: " + eeprom_deviceId_string;
  Serial.println(newDeviceId_string);

  str_len = newDeviceId_string.length() + 1;
  char newDeviceId_charArray[str_len];
  newDeviceId_string.toCharArray(newDeviceId_charArray, str_len);

  client.publish("hat/msg", newDeviceId_charArray);
  newDeviceId_charArray[0] = 0;
  
  
  btn_fourClicks_setId = false;


  /* subscribe to "hat/[deviceId]" */

  String topic_id_anim_string = prefix + eeprom_deviceId_string;

  str_len = topic_id_anim_string.length() + 1;
  char topic_id_anim_charArray[str_len];
  topic_id_anim_string.toCharArray(topic_id_anim_charArray, str_len);
  
  
  client.subscribe(topic_id_anim_charArray); // subscribe to device id 
  topic_id_anim_charArray[0] = 0;
}

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print(F("Attempting MQTT conn..."));
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println(F("connected"));
      // Once connected, publish an announcement...
      // client.publish("hat/test", "hello world");
      // ... and resubscribe
      
      client.subscribe("hat/all");
      client.subscribe("hat/setId");
      //client.subscribe("hat/#");


      /* subscribe to "hat/[deviceId]" */
      
      String topic_id_anim_string = prefix + eeprom_deviceId_string;
      
      str_len = topic_id_anim_string.length() + 1;
      char topic_id_anim_charArray[str_len];
      topic_id_anim_string.toCharArray(topic_id_anim_charArray, str_len);
      
      client.subscribe(topic_id_anim_charArray); 
      
    
    } else {
      if((millis() - lastTimestamp_wifi) > 5000){
        lastTimestamp_wifi = millis();
        Serial.print(F("failed, rc="));
        Serial.print(client.state());
        Serial.println(F(" retry in 5s"));
        wifiSucks_counter++;
        if(wifiSucks_counter > 2){ // retart ESP8266 when lost wifi too long
          wifiSucks_counter = 0;
          ESP.restart();
        }
      }    
    }
  }
}

void loop() {

  /* Print Heap Memory in console */
  if((millis() - lastTimestamp) >= 15000){

    String heapsize_string = "Heap: " + String(ESP.getFreeHeap());
    Serial.println(heapsize_string);

    /*
    int str_len = heapsize_string.length() + 1;
    char heapsize_char_array[str_len];
    heapsize_string.toCharArray(heapsize_char_array, str_len);
    client.publish("hat/msg", heapsize_char_array);
    heapsize_char_array[0] = 0;
    */
    
    lastTimestamp = millis();
  }

  
  if(((millis()-initTime) >= 5000) && ((millis()-initTime) < 5200)){
    msgCount = 2;
    // Serial.println(millis()-initTime);
  }

  
  /* FastLED start */

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



  /***************************************
   * Button
   ***************************************/

  buttonState = digitalRead(buttonPin);

  if(btn_fourClicks_setId == true){
    if((millis() - button_timestamp) >= 20000){
      
      btn_fourClicks_setId = false; 
      // Serial.println("set id time expired");
      client.publish("hat/msg", "Set id expired");
    }
  }
  

  if(buttonState == prev_buttonState) return;
  prev_buttonState = buttonState;
  if(buttonState == 1){

    // Serial.println("btn pressed");
    if((millis() - button_timestamp) < 700){
      button_counter++;
      button_timestamp = millis();
    }
    else if((millis() - button_timestamp) >= 700){ // erster klick
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
      client.publish("hat/msg", "Set id");  
    }
    else if(button_counter == 7){
      btn_fourClicks_setId = false;
      Serial.println(F("restart"));
      ESP.restart();
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

const TProgmemPalette16 testColors_c PROGMEM =
{
    CRGB::Green,
    CRGB::Orange,
    CRGB::Orange,
    CRGB::Orange,
    CRGB::Green,
    CRGB::Green
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
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount){
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
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount){
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount){
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}

/*Fade towards color end */
