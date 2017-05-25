/*
 ESP8266 MQTT lighting node
 Based on the Basic ESP8266 MQTT example

 1) Configure MQTT_SERVER, MQTT_USERNAME and MQTT_PASSWORD in isis.c
 2) If applicable, change PLACEID and MQTT_DEVICE
 
 The node will connect to MQTT_SERVER, subscribes to the topic "button"
 and listens for the payload sent. If the payload equals the Integer
 defined in PLACEID, it will turn on/off a chain of 10 WS2801 LEDs
 connected to the RX pin (#3). If the received payload is another
 PLACEID and the LEDs were previously on, it will change the LED colour
 to red for 500 ms and switch them off afterwards.

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <NeoPixelBus.h>
#include "isis.c"

#define COLOR_SATURATION 255      // 255 sets full LED brightness for one channel
#define PLACEID '1'               // Defines the payload for which to react
#define MQTT_DEVICE "ESP8266Led1" // Change this for every device!

// WifiManager allows us to easily connect the node to any WiFi network without
// hardcoding any credentials into the code
WiFiManager wifiManager;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

bool switchStatus = false;

RgbColor green(COLOR_SATURATION, 0, 0);
RgbColor red(0, COLOR_SATURATION, 0);
RgbColor blue(0, 0, COLOR_SATURATION);
RgbColor white(COLOR_SATURATION);
RgbColor black(0);

const uint16_t PixelCount = 10; // this example assumes 10 pixels
const uint8_t PixelPin = 3;     // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

void setup_wifi() {
  delay(10);
  
  wifiManager.autoConnect();

  Serial.print("Connected! IP address: ");
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


  // Switch on the LED if the payload equals the PLACEID
  if (((char)payload[0]) == PLACEID) {
    // if LEDs were previously off, turn them on. Otherwise,
    // turn them off
    if (!switchStatus) { fadein(); }
    else { fadeout(); }
    Serial.println("Received PLACEID, working.");
  } else {
    // Another node was triggered. If this node was previously
    // illuminated, turn it off.
    if (switchStatus) { fadeout(); }
    Serial.println("Some other node was triggered.");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_DEVICE, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Subscribing +/devices/+/up/button");
      // ... and resubscribe
      client.subscribe("+/devices/+/up/button");
      Serial.println("Subscribed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void fadeout() {
    // Set all colours to red for 500ms, then turn them off
    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, red);
    strip.SetPixelColor(2, red);
    strip.SetPixelColor(3, red);
    strip.SetPixelColor(4, red);
    strip.SetPixelColor(5, red);
    strip.SetPixelColor(6, red);
    strip.SetPixelColor(7, red);
    strip.SetPixelColor(8, red);
    strip.SetPixelColor(9, red);
    strip.Show();
    delay(500);

    strip.SetPixelColor(0, black);
    strip.SetPixelColor(1, black);
    strip.SetPixelColor(2, black);
    strip.SetPixelColor(3, black);
    strip.SetPixelColor(4, black);
    strip.SetPixelColor(5, black);
    strip.SetPixelColor(6, black);
    strip.SetPixelColor(7, black);
    strip.SetPixelColor(8, black);
    strip.SetPixelColor(9, black);
    strip.Show();
    switchStatus=false;
  }
  
void fadein()  {
    strip.SetPixelColor(0, green);
    strip.SetPixelColor(1, white);
    strip.SetPixelColor(2, green);
    strip.SetPixelColor(3, white);
    strip.SetPixelColor(4, green);
    strip.SetPixelColor(5, white);
    strip.SetPixelColor(6, green);
    strip.SetPixelColor(7, white);
    strip.SetPixelColor(8, green);
    strip.SetPixelColor(9, white);
    strip.Show();
    switchStatus=true;
  }

void setup() {
  Serial.begin(115200);

  strip.Begin();
  strip.Show();
  
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
