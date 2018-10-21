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

/* FastLED end */





void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
  /* FastLed setup start */

  // insert FastLED code

  /* FastLed setup end */
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
  else if ((char)payload[1] == '0') black();

}

void white(){
  Serial.println("white");
}

void turquoise(){
  Serial.println("turquoise");
}

void black(){
  Serial.println("black");
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





 
