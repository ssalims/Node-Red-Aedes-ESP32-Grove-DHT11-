
/* https://create.arduino.cc/projecthub/officine/interfacing-arduino-mkr-or-esp-via-mqtt-node-red-101-4833bc
 *  http://pdacontrolen.com/esp8266-public-broker-mqtt-mosca-io-node-red/
 *  MQTT Client Temperature Onewire
* More information about projects PDAControl 
* PDAControl English http://pdacontrolen.com 
* Mas informacion sobre proyectos PDAControl
* PDAControl Espanol http://pdacontrolen.com 
* Channel Youtube https://www.youtube.com/c/JhonValenciaPDAcontrol/videos 
* 
* under messageReceived (callback method) add actions to be done when a msg is received.
 * to publish, call client.publish(topic,msg)
*/


//#include <ESP8266WiFi.h> //if you are using nodemcu
#include <WiFi.h> //if you are using esp32
#include <PubSubClient.h>

///////GROVE DHT11//////////
#include "DHT.h"
#define DHTTYPE DHT11   

#define DHTPIN 26  // connect signal to pin 26  
DHT dht(DHTPIN, DHTTYPE);  
///////////////////////////


unsigned long lastMillis = 0;

// Update these with values suitable for your network.
const char* ssid = "YOUR SSID";
const char* password = "YOUR SSID PASSWORD";
const char* mqtt_server = "NODE-RED IP ADDRESS"; /// NodeRED IP Address
int mqtt_port = 1889; //Must match with broker's MQTT port - dont use 1880 

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() { 
  Serial.begin(115200);
  // Start up the library
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  setup_wifi(); //function call
 
  Serial.println("WiFi Connected...");
  Serial.print("MQTT Server IP Address:");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(String(mqtt_port)); 
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP()); 
  Serial.println("Modbus RTU Master Online"); 

  pinMode(LED_BUILTIN, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // Start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.println("Callback"); // Flag
  /* To display message sent from broker */
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  /* ------------------------------------ */

  /* This section is for controlling output: LED, servo etc */
  Serial.println();
  if ((char)payload[0] == 't') {
    Serial.println("ON");
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
     Serial.println("OFF");
    digitalWrite(LED_BUILTIN, LOW);
  }

 /* ------------------------------------ */
}
void reconnect() {
  
 // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
 // Attempt to connect
    if (client.connect("ESP32Client")) {
        Serial.println("Broker connected to ESP32");
        client.subscribe("event"); // Topic at ESP32
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
  ///////////////////////////
  float temp_hum_val[2] = {0}; // grove_dht set nilai 2 array = 0
  dht.readTempAndHumidity(temp_hum_val); // baca temp & humid
  ///////////////////////////
 Serial.print(temp_hum_val[0]);
 Serial.print(" ");
 Serial.println(temp_hum_val[1]);
  char tempValue[15], humidValue[15];
  dtostrf(temp_hum_val[0],4, 0, humidValue); // convert float to char 
  dtostrf(temp_hum_val[1],4, 0, tempValue); // convert float to char 
  client.publish("labA/rack1/humid", humidValue); /// send char
  client.publish("labA/rack1/temp", tempValue); /// send char 
 
  if (!client.connected()) {
    reconnect(); //function call
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
      lastMillis = millis();
      client.publish("/hello", "world order"); //PUBLISH TO TOPIC /hello MSG world order
      //Serial.print (" HelLo World ");
    }  
 
  client.loop();

  delay(10000); 
}
