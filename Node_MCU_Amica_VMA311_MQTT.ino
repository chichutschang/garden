#include <ArduinoJson.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

const char* ssid="ASUS";
const char* password = "82Highland";
const char* mqtt_server = "192.168.1.150";

#define humidity_topic "sensor/humidity"
#define temperature_topic "sensor/temperature"

#define DHTTYPE DHT11
#define DHTPIN 2        //Connect the VMA311 sensor to Pin D4 of Node MCU

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi(){
  delay(10);
  //connect to Wi-Fi
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    if(client.connect("Sensors")){
      Serial.println("connected");
    } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds");
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff){
  return !isnan(newValue) && (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float tempC = 0.0;
float tempF = 0.0;
float humidity = 0.0;
float difference = 1.0;

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  long now = millis();
  if(now - lastMsg > 2000){
    lastMsg = now;

  float h = dht.readHumidity();
  float c = dht.readTemperature();      //Celsius
  float f = dht.readTemperature(true);  //Fahrenheit

  //do not publish temperature in Celsius
//  if(checkBound(c, tempC, difference)){
//    tempC = c;
//    Serial.print("Temperature: ");
//    Serial.print(String(tempC).c_str());
//    Serial.println("*C");
//    client.publish(temperature_topic, String(tempC).c_str(), true);
//    }

  //publish temperature in Fahrenheit if changes 1 degree
  if(checkBound(f, tempF, difference)){
    tempF = f;
    Serial.print("Temperature: ");
    Serial.print(String(tempF).c_str());
    Serial.println("*F");
    client.publish(temperature_topic, String(tempF).c_str(), true);
    }

  //publish humidity if changes 1 degree
  if(checkBound(h, humidity, difference)){
    humidity = h;
    Serial.print("Humidity: ");
    Serial.println(String(humidity).c_str());
    client.publish(humidity_topic, String(humidity).c_str(), true);
    }
  }
}
