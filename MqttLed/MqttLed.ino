/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

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

#define COLOR_SATURATION 255
#define PLACEID '1'
#define MQTT_DEVICE "ESP8266Led2"


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

const uint16_t PixelCount = 10; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266
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


  // Switch on the LED if an 1 was received as first character
  if (((char)payload[0]) == PLACEID) {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LEDs on
    if (!switchStatus) { fadein(); }
    else { fadeout(); }
    Serial.println("PLACEID ist angekommen, mache Dinge");
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    if (switchStatus) { fadeout(); }
    Serial.println("Irgendwas anderes kam an");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
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
